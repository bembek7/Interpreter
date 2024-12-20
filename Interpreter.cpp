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
		currentScope = std::make_shared<Scope>();
		currentScope->valueExpectedInCurrentFunction = true;
		// evaluate arguments
		InterpretFunDef(mainFunction);
	}
	else
	{
		throw; // error
	}
}

void Interpreter::InterpretFunDef(const FunctionDefiniton* const funDef, std::vector<Value> arguments)
{
	std::wstring argumentsString;
	for (const auto& arg : arguments)
	{
		argumentsString += arg.ToString() + L" ";
	}
	Print(L"Function: " + funDef->identifier + L" Arguments: " + argumentsString);
	if (funDef->parameters.size() != arguments.size())
	{
		throw; // error
	}
	for (size_t i = 0; i < arguments.size(); ++i)
	{
		currentScope->variables.push_back({ funDef->parameters[i].paramMutable, funDef->parameters[i].identifier,  arguments[i] });
	}

	InterpretBlock(funDef->block.get());
}

void Interpreter::InterpretBlock(const Block* const block)
{
	++currentDepth;
	auto newScope = std::make_shared<Scope>();
	newScope->higherScope = currentScope;
	newScope->valueExpectedInCurrentFunction = newScope->higherScope->valueExpectedInCurrentFunction;
	currentScope = std::move(newScope);
	for (const auto& statement : block->statements)
	{
		statement->InterpretThis(*this);
		if (dynamic_cast<Return*>(statement.get()))
		{
			break;
		}
	}
	currentScope = currentScope->higherScope;
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
	if (function)
	{
		previousScopes.push(currentScope);
		currentScope = std::make_shared<Scope>();
		currentScope->valueExpectedInCurrentFunction = valueExpected;
		std::vector<Value> arguments;
		for (const auto& arg : functionCall->arguments)
		{
			arguments.push_back(EvaluateExpression(arg.get()));
		}
		InterpretFunDef(function, arguments);
		currentScope = previousScopes.top();
		previousScopes.pop();
	}
	else
	{
		throw;// error
	}
}

void Interpreter::InterpretWhileLoop(const WhileLoop* const whileLoop)
{
	auto conditionExpression = EvaluateExpression(whileLoop->condition.get());
	Print(L"While " + conditionExpression.ToString());
	while (conditionExpression.ToBool())
	{
		InterpretBlock(whileLoop->block.get());

		conditionExpression = EvaluateExpression(whileLoop->condition.get());
	}
}

void Interpreter::InterpretReturn(const Return* const returnStatement)
{
	if (currentScope->valueExpectedInCurrentFunction)
	{
		if (returnStatement->expression)
		{
			lastReturnedValue = EvaluateExpression(returnStatement->expression.get());
			Print(L"Return " + lastReturnedValue->ToString());
		}
		else
		{
			throw; // error
		}
	}
	else
	{
		Print(L"Return");
	}
}

void Interpreter::InterpretConditional(const Conditional* const conditional)
{
	auto conditionExpression = EvaluateExpression(conditional->condition.get());
	Print(L"Conditional " + conditionExpression.ToString());
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
	if (currentScope->VariableAlreadyExists(declaration->identifier))
	{
		throw; // error
	}
	currentScope->variables.push_back(Variable(declaration->varMutable, declaration->identifier));
	if (declaration->expression)
	{
		auto value = EvaluateExpression(declaration->expression.get());
		currentScope->variables.back().value = value;
		Print(L"Declaration " + declaration->identifier + L" = " + value.ToString());
	}
	else
	{
		Print(L"Declaration " + declaration->identifier);
	}
}

void Interpreter::InterpretAssignment(const Assignment* const assignment)
{
	auto variable = currentScope->GetVariable(assignment->identifier);
	if (!variable)
	{
		throw; // error
	}
	variable->value = EvaluateExpression(assignment->expression.get());
	Print(L"Assignment " + assignment->identifier + L" = " + variable->value->ToString());
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
	Value currentValue = false;
	for (const auto& conjunction : expression->conjunctions)
	{
		if (expression->conjunctions.size() > 1)
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
		if (conjunction->relations.size() > 1)
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
		switch (*relation->relationOperator)
		{
		case RelationOperator::Equal:
			return first == second;
			break;
		case RelationOperator::NotEqual:
			return first != second;
			break;
		case RelationOperator::Greater:
			return first > second;
			break;
		case RelationOperator::GreaterEqual:
			return first >= second;
			break;
		case RelationOperator::Less:
			return first < second;
			break;
		case RelationOperator::LessEqual:
			return first <= second;
			break;
		default:
			//throw;  error
			break;
		}
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
	auto first = EvaluateFactor(multiplicative->factors.front().get());
	auto currentValue = first;
	for (size_t i = 0; i < multiplicative->operators.size(); ++i)
	{
		switch (multiplicative->operators[i])
		{
		case MultiplicationOperator::Multiple:
			currentValue *= EvaluateFactor(multiplicative->factors[i + 1].get());
			break;
		case  MultiplicationOperator::Divide:
			currentValue /= EvaluateFactor(multiplicative->factors[i + 1].get());
			break;
		default:
			throw; // error
			break;
		}
	}
	return currentValue;
}

Value Interpreter::EvaluateFactor(const Factor* const factor)
{
	std::optional<Value> evaluatedVal = std::nullopt;
	if (std::holds_alternative<std::wstring>(factor->factor))
	{
		auto variable = currentScope->GetVariable(std::get<std::wstring>(factor->factor));
		if (!variable)
		{
			throw; // error
		}
		if (variable->value)
		{
			return *variable->value;
		}
		throw; //error
	}
	else if (std::holds_alternative<Literal>(factor->factor))
	{
		evaluatedVal = EvaluateLiteral(std::get<Literal>(factor->factor));
	}
	else if (auto stdExpr = std::get_if<std::unique_ptr<StandardExpression>>(&factor->factor))
	{
		evaluatedVal = EvaluateStandardExpression(stdExpr->get());
	}
	else if (auto funcCall = std::get_if<std::unique_ptr<FunctionCall>>(&factor->factor))
	{
		InterpretFunctionCall(funcCall->get(), true);
		evaluatedVal = lastReturnedValue;
		if (!evaluatedVal)
		{
			throw; // error
		}
	}
	if (evaluatedVal)
	{
		return (factor->logicallyNegated) ? !(*evaluatedVal) : *evaluatedVal;
	}
	throw; // error
}

Value Interpreter::EvaluateLiteral(const Literal& literal)
{
	if (std::holds_alternative<int>(literal.value))
	{
		return Value(std::get<int>(literal.value));
	}
	if (std::holds_alternative<bool>(literal.value))
	{
		return Value(std::get<bool>(literal.value));
	}
	if (std::holds_alternative<std::wstring>(literal.value))
	{
		return Value(std::get<std::wstring>(literal.value));
	}
	if (std::holds_alternative<float>(literal.value))
	{
		return Value(std::get<float>(literal.value));
	}

	throw; //
}

void Interpreter::Print(const std::wstring& msg) const noexcept
{
	std::wstring tabulation(currentDepth, L'\t');
	std::wcout << tabulation << msg << std::endl;
}