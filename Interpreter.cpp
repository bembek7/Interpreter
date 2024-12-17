#include "Interpreter.h"
#include <iostream>

void Interpreter::Interpret(const Program* const program)
{
	const FunctionDefiniton* mainFunction = nullptr;
	for (const auto& funDef : program->funDefs)
	{
		if (funDef->identifier == L"Main")
		{
			mainFunction = funDef.get();
		}
		knownFunctions.push_back(funDef.get());
	}
	if (mainFunction)
	{
		InterpretFunDef(mainFunction, false);
	}
	else
	{
		throw; // error
	}
}

void Interpreter::InterpretFunDef(const FunctionDefiniton* const funDef, bool valueExpected, std::vector<Value> arguments)
{
	Print(L"Function: " + funDef->identifier + L" Arguments: ");
	auto newScope = std::make_shared<Scope>();
	newScope->higherScope = currentScope;
	currentScope = std::move(newScope);
	valueExpectedInCurrentFunction = valueExpected;
	if (funDef->parameters.size() != arguments.size())
	{
		throw; // error
	}
	for (size_t i = 0; i < arguments.size(); ++i)
	{
		currentScope->variables.push_back({ funDef->parameters[i].paramMutable, funDef->parameters[i].identifier /*argumnent*/ });
	}

	InterpretBlock(funDef->block.get());
}

void Interpreter::InterpretBlock(const Block* const block)
{
	++currentDepth;
	for (const auto& statement : block->statements)
	{
		statement->InterpretThis(*this);
		if (dynamic_cast<Return*>(statement.get()))
		{
			break;
		}
	}
	--currentDepth;
}

void Interpreter::InterpretFunctionCallStatement(const FunctionCallStatement* const functionCallStatement)
{
	Print(L"FunctionCallStatement");
	InterpretFunctionCall(functionCallStatement->funcCall.get(), false);
}

void Interpreter::InterpretFunctionCall(const FunctionCall* const functionCall, const bool valueExpected)
{
	auto function = GetFunctionDefintion(functionCall->identifier);
	// evaluate artguments
	if (function)
	{
		InterpretFunDef(function, valueExpected /*arguments*/);
	}
	else
	{
		throw;// error
	}
}

void Interpreter::InterpretWhileLoop(const WhileLoop* const whileLoop)
{
	Print(L"While");
	auto conditionExpression = EvaluateExpression(whileLoop->condition.get());
	while (conditionExpression.ToBool())
	{
		InterpretBlock(whileLoop->block.get());
	}
}

void Interpreter::InterpretReturn(const Return* const returnStatement)
{
	Print(L"Return");
	if (valueExpectedInCurrentFunction)
	{
		if (returnStatement->expression)
		{
			auto returnValue = EvaluateExpression(returnStatement->expression.get());
		}
		else
		{
			throw; // error
		}
	}
}

void Interpreter::InterpretConditional(const Conditional* const conditional)
{
	Print(L"Conditional");
	auto conditionExpression = EvaluateExpression(conditional->condition.get());
	if (conditionExpression.ToBool())
	{
		InterpretBlock(conditional->ifBlock.get());
	}
	else
	{
		InterpretBlock(conditional->elseBlock.get());
	}
}

void Interpreter::InterpretDeclaration(const Declaration* const declaration)
{
	Print(L"Declaration");
	if (currentScope->VariableAlreadyExists(declaration->identifier))
	{
		throw; // error
	}
	currentScope->variables.push_back(Variable(declaration->varMutable, declaration->identifier));
	if (declaration->expression)
	{
		auto value = EvaluateExpression(declaration->expression.get());
		currentScope->variables.back().value = value;
	}
}

void Interpreter::InterpretAssignment(const Assignment* const assignment)
{
	Print(L"Assignment");
	auto variable = currentScope->GetVariable(assignment->identifier);
	if (!variable)
	{
		throw; // error
	}
	variable->value = EvaluateExpression(assignment->expression.get());
}

Interpreter::Variable* Interpreter::Scope::GetVariable(const std::wstring& identifier) noexcept
{
	for (auto& var : variables)
	{
		if (var.identifier == identifier)
		{
			return &var;
		}
	}
	if (higherScope)
	{
		return higherScope->GetVariable(identifier);
	}
	return nullptr;
}

bool Interpreter::Scope::VariableAlreadyExists(const std::wstring& identifier) const noexcept
{
	for (auto& var : variables)
	{
		if (var.identifier == identifier)
		{
			return true;
		}
	}
	if (higherScope)
	{
		return higherScope->VariableAlreadyExists(identifier);
	}
	return false;
}

const FunctionDefiniton* Interpreter::GetFunctionDefintion(const std::wstring& identifier) const noexcept
{
	for (const auto function : knownFunctions)
	{
		if (function->identifier == identifier)
		{
			return function;
		}
	}
	return nullptr;
}

Value Interpreter::EvaluateExpression(const Expression* const expression)
{
	return expression->EvaluateThis(*this);
}

Value Interpreter::EvaluateStandardExpression(const StandardExpression* const expression)
{
	Print(L"Standard expression being evaluated");
	Value currentValue = false;
	for (const auto& conjunction : expression->conjunctions)
	{
		if (expression->conjunctions.size() > 0)
		{
			currentValue |= EvaluateConjunction(conjunction.get());
			if (currentValue.ToBool())
			{
				return true;
			}
		}
		else
		{
			currentValue = EvaluateConjunction(conjunction.get());
		}
	}
	return currentValue;
}

Value Interpreter::EvaluateConjunction(const Conjunction* const conjunction)
{
	Value currentValue = true;
	for (const auto& relation : conjunction->relations)
	{
		if (conjunction->relations.size() > 0)
		{
			currentValue &= EvaluateRelation(relation.get());
			if (!currentValue.ToBool())
			{
				return false;
			}
		}
		else
		{
			currentValue = EvaluateRelation(relation.get());
		}
	}
	return currentValue;
}

Value Interpreter::EvaluateRelation(const Relation* const relation)
{
	auto first = EvaluateAdditive(relation->firstAdditive.get());
	if (relation->relationOperator)
	{
		auto second = EvaluateAdditive(relation->secondAdditive.get());
		//switch (*relation->relationOperator)
		//{
		//case RelationOperator::Equal:
		//	return first == second;
		//	break;
		//case RelationOperator::NotEqual:
		//	return first != second;
		//	break;
		//case RelationOperator::Greater:
		//	return first > second;
		//	break;
		//case RelationOperator::GreaterEqual:
		//	return first >= second;
		//	break;
		//case RelationOperator::Less:
		//	return first < second;
		//	break;
		//case RelationOperator::LessEqual:
		//	return first <= second;
		//	break;
		//default:
		//	throw; // error
		//	break;
		//}
	}
	return first;
}

Value Interpreter::EvaluateAdditive(const Additive* const additive)
{
	auto first = EvaluateMultiplicative(additive->multiplicatives.front().get());
	auto currentValue = first;
	for (size_t i = 0; i < additive->operators.size(); ++i)
	{
		switch (additive->operators[i])
		{
		case AdditionOperator::Plus:
			currentValue += EvaluateMultiplicative(additive->multiplicatives[i + 1].get());
			break;
		case AdditionOperator::Minus:
			currentValue -= EvaluateMultiplicative(additive->multiplicatives[i + 1].get());
			break;
		default:
			throw; // error
			break;
		}
	}
	return (additive->negated) ? -currentValue : currentValue;
}

Value Interpreter::EvaluateMultiplicative(const Multiplicative* const multiplicative)
{
	return Value(false);
}

void Interpreter::Print(const std::wstring& msg) const noexcept
{
	std::wstring tabulation(currentDepth, L'\t');
	std::wcout << tabulation << msg << std::endl;
}