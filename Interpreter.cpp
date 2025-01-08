#include "Interpreter.h"
#include <iostream>
#include "InterpreterException.h"
#include "StringConversion.h"

void Interpreter::Interpret(const Program* const program)
{
	currentPosition = { 0, 0 };
	try
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
			InterpretFunDef(mainFunction);
		}
		else
		{
			throw InterpreterException("Main function not found.", currentPosition);
		}
	}
	catch (const Value::ValueException& ve)
	{
		std::cout << ve.what() << "[line:" << currentPosition.line << ", column : " << currentPosition.column << "] " << std::endl;
	}
	catch (const std::runtime_error& e)
	{
		std::cout << e.what();
	}
}

void Interpreter::InterpretFunDef(const FunctionDefiniton* const funDef, const std::vector<Value>& arguments)
{
	currentPosition = funDef->startingPosition;
	std::wstring argumentsString;
	for (const auto& arg : arguments)
	{
		argumentsString += arg.ToPrintString() + L" ";
	}
	Print(L"Function: " + funDef->identifier + L" Arguments: " + argumentsString);
	if (funDef->parameters.size() != arguments.size())
	{
		std::stringstream ss;
		ss << "Function expects " << funDef->parameters.size() << " arguments, but got " << arguments.size() << ".";
		throw InterpreterException(ss.str().c_str(), currentPosition);
	}
	for (size_t i = 0; i < arguments.size(); ++i)
	{
		currentScope->variables.push_back({ funDef->parameters[i].paramMutable, funDef->parameters[i].identifier,  arguments[i] });
	}

	InterpretBlock(funDef->block.get());
}

void Interpreter::InterpretFunction(const Value::Function* const function, const std::vector<Value>& arguments)
{
	auto allArguments = function->boundArguments;
	
	allArguments.insert(allArguments.end(), arguments.begin(), arguments.end());

	auto expectedParametersNum = (function->composedOf) ? (function->composedOf->parameters.size() - function->composedOf->boundArguments.size()) : function->parameters.size();
	if (expectedParametersNum != allArguments.size())
	{
		std::stringstream ss;
		ss << "Function expects " << function->parameters.size() << " arguments, but got " << allArguments.size() << ".";
		throw InterpreterException(ss.str().c_str(), currentPosition);
	}
	
	if (function->composedOf)
	{
		CallFunction(function->composedOf.get(), allArguments, true);

		allArguments.clear();
		allArguments.push_back(*lastReturnedValue);
	}

	std::wstring argumentsString;
	for (const auto& arg : allArguments)
	{
		argumentsString += arg.ToPrintString() + L" ";
	}
	Print(L"Function from variable, Arguments: " + argumentsString);

	for (size_t i = 0; i < allArguments.size(); ++i)
	{
		currentScope->variables.push_back({ function->parameters[i].paramMutable, function->parameters[i].identifier,  allArguments[i] });
	}

	InterpretBlock(function->block);
}

void Interpreter::InterpretBlock(const Block* const block)
{
	currentPosition = block->startingPosition;
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
	currentPosition = functionCallStatement->startingPosition;
	Print(L"FunctionCallStatement");
	InterpretFunctionCall(functionCallStatement->funcCall.get(), false);
}

void Interpreter::InterpretFunctionCall(const FunctionCall* const functionCall, const bool valueExpected)
{
	currentPosition = functionCall->startingPosition;
	auto function = GetFunctionDefintion(functionCall->identifier);
	const Value::Function* functionFromVariable = nullptr;
	if (!function)
	{
		const auto var = currentScope->GetVariable(functionCall->identifier);
		if (var)
		{
			const auto& functionValue = var->value;
			if (functionValue)
			{
				functionFromVariable = functionValue->GetFunction();
			}
		}
	}

	if (function || functionFromVariable)
	{
		std::vector<Value> arguments;
		for (const auto& arg : functionCall->arguments)
		{
			arguments.push_back(EvaluateExpression(arg.get()));
		}
		if (function)
		{
			previousScopes.push(currentScope);
			currentScope = std::make_shared<Scope>();
			currentScope->valueExpectedInCurrentFunction = valueExpected;
			InterpretFunDef(function, arguments);
			currentScope = previousScopes.top();
			previousScopes.pop();
		}
		else
		{
			CallFunction(functionFromVariable, arguments, valueExpected);
		}
	}
	else
	{
		throw InterpreterException("Function definition not found.", currentPosition);
	}
}

void Interpreter::CallFunction(const Value::Function* const function, const std::vector<Value>& arguments, const bool valueExpected)
{
	previousScopes.push(currentScope);
	currentScope = std::make_shared<Scope>();
	currentScope->valueExpectedInCurrentFunction = valueExpected;
	InterpretFunction(function, arguments);
	currentScope = previousScopes.top();
	previousScopes.pop();
}

void Interpreter::InterpretWhileLoop(const WhileLoop* const whileLoop)
{
	currentPosition = whileLoop->startingPosition;
	auto conditionExpression = EvaluateExpression(whileLoop->condition.get());
	Print(L"While " + conditionExpression.ToPrintString());
	while (conditionExpression.ToBool())
	{
		InterpretBlock(whileLoop->block.get());

		conditionExpression = EvaluateExpression(whileLoop->condition.get());
	}
}

void Interpreter::InterpretReturn(const Return* const returnStatement)
{
	currentPosition = returnStatement->startingPosition;
	if (currentScope->valueExpectedInCurrentFunction)
	{
		if (returnStatement->expression)
		{
			lastReturnedValue = EvaluateExpression(returnStatement->expression.get());
			Print(L"Return " + lastReturnedValue->ToPrintString());
		}
		else
		{
			throw InterpreterException("Function was expected to return value but returns nothing.", currentPosition);
		}
	}
	else
	{
		lastReturnedValue = std::nullopt;
		Print(L"Return");
	}
}

void Interpreter::InterpretConditional(const Conditional* const conditional)
{
	currentPosition = conditional->startingPosition;
	auto conditionExpression = EvaluateExpression(conditional->condition.get());
	Print(L"Conditional " + conditionExpression.ToPrintString());
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
	currentPosition = declaration->startingPosition;
	if (currentScope->VariableAlreadyExists(declaration->identifier))
	{
		throw InterpreterException("Redefinition of variable is not allowed.", currentPosition);
	}
	if (FunctionAlreadyExists(declaration->identifier))
	{
		throw InterpreterException("Variable can not have the same name as function does.", currentPosition);
	}
	currentScope->variables.push_back(Variable(declaration->varMutable, declaration->identifier));
	if (declaration->expression)
	{
		auto value = EvaluateExpression(declaration->expression.get());
		currentScope->variables.back().value = value;
		Print(L"Declaration " + declaration->identifier + L" = " + value.ToPrintString());
	}
	else
	{
		Print(L"Declaration " + declaration->identifier);
	}
}

void Interpreter::InterpretAssignment(const Assignment* const assignment)
{
	currentPosition = assignment->startingPosition;
	auto variable = currentScope->GetVariable(assignment->identifier);
	if (!variable)
	{
		throw InterpreterException("Variable was not declared.", currentPosition);
	}
	if (!variable->isMutable)
	{
		throw InterpreterException("Cannot assign to immutable variable.", currentPosition);
	}
	variable->value = EvaluateExpression(assignment->expression.get());
	Print(L"Assignment " + assignment->identifier + L" = " + variable->value->ToPrintString());
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
	currentPosition = expression->startingPosition;
	return expression->EvaluateThis(*this);
}

Value Interpreter::EvaluateStandardExpression(const StandardExpression* const expression)
{
	currentPosition = expression->startingPosition;
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
	currentPosition = conjunction->startingPosition;
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
	currentPosition = relation->startingPosition;
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
			throw InterpreterException("Cannot handle such operator.", currentPosition);
			break;
		}
	}
	return first;
}

Value Interpreter::EvaluateAdditive(const Additive* const additive)
{
	currentPosition = additive->startingPosition;
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
			throw InterpreterException("Cannot handle such operator.", currentPosition);
			break;
		}
	}
	return (additive->negated) ? -currentValue : currentValue;
}

Value Interpreter::EvaluateMultiplicative(const Multiplicative* const multiplicative)
{
	currentPosition = multiplicative->startingPosition;
	auto first = EvaluateFactor(multiplicative->factors.front().get());
	auto currentValue = first;
	for (size_t i = 0; i < multiplicative->operators.size(); ++i)
	{
		switch (multiplicative->operators[i])
		{
		case MultiplicationOperator::Multiply:
			currentValue *= EvaluateFactor(multiplicative->factors[i + 1].get());
			break;
		case  MultiplicationOperator::Divide:
			currentValue /= EvaluateFactor(multiplicative->factors[i + 1].get());
			break;
		default:
			throw InterpreterException("Cannot handle such operator.", currentPosition);
			break;
		}
	}
	return currentValue;
}

Value Interpreter::EvaluateFactor(const Factor* const factor)
{
	currentPosition = factor->startingPosition;
	std::optional<Value> evaluatedVal = std::nullopt;
	if (std::holds_alternative<std::wstring>(factor->factor))
	{
		auto variable = currentScope->GetVariable(std::get<std::wstring>(factor->factor));
		if (!variable)
		{
			std::stringstream ss;
			ss << "Variable '" << StringConversion::ToNarrow(std::get<std::wstring>(factor->factor)) << "' was not declared.";
			throw InterpreterException(ss.str().c_str(), currentPosition);
		}
		if (variable->value)
		{
			return *variable->value;
		}
		std::stringstream ss;
		ss << "Variable '" << StringConversion::ToNarrow(std::get<std::wstring>(factor->factor)) << "' does not have value.";
		throw InterpreterException(ss.str().c_str(), currentPosition);
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
			throw InterpreterException("Function did not return any value", currentPosition);
		}
	}
	if (evaluatedVal)
	{
		return (factor->logicallyNegated) ? !(*evaluatedVal) : *evaluatedVal;
	}
	throw InterpreterException("Could not evaluate factor value", currentPosition);
}

Value Interpreter::EvaluateLiteral(const Literal& literal)
{
	currentPosition = literal.startingPosition;
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

	throw InterpreterException("Could not evaluate literal value", currentPosition);
}

Value Interpreter::EvaluateFuncExpression(const FuncExpression* funcExpression)
{
	currentPosition = funcExpression->startingPosition;
	Value currentValue;
	for (size_t i = 0; i < funcExpression->composables.size(); ++i)
	{
		if (i > 0)
		{
			currentValue = Value(currentValue >> EvaluateComposable(funcExpression->composables[i].get()));
		}
		else
		{
			currentValue = EvaluateComposable(funcExpression->composables[i].get());
		}
	}
	return currentValue;
}

Value Interpreter::EvaluateComposable(const Composable* const composable)
{
	currentPosition = composable->startingPosition;
	auto bindable = EvaluateBindable(composable->bindable.get());
	if (!composable->arguments.empty())
	{
		std::vector<Value> arguments;
		for (const auto& arg : composable->arguments)
		{
			arguments.push_back(EvaluateExpression(arg.get()));
		}

		return bindable << arguments;
	}
	return bindable;
}

Value Interpreter::EvaluateBindable(const Bindable* const bindable)
{
	currentPosition = bindable->startingPosition;
	if (auto funcLit = std::get_if<std::unique_ptr<FunctionLiteral>>(&bindable->bindable))
	{
		return EvaluateFunctionLiteral(funcLit->get());
	}
	if (auto funcExpr = std::get_if<std::unique_ptr<FuncExpression>>(&bindable->bindable))
	{
		return EvaluateFuncExpression(funcExpr->get());
	}
	if (auto funcCall = std::get_if<std::unique_ptr<FunctionCall>>(&bindable->bindable))
	{
		InterpretFunctionCall(funcCall->get(), true);
		if (!lastReturnedValue)
		{
			throw InterpreterException("Function did not return any value", currentPosition);
		}
		return *lastReturnedValue;
	}
	if (std::holds_alternative<std::wstring>(bindable->bindable))
	{
		const auto& identifier = std::get<std::wstring>(bindable->bindable);
		auto variable = currentScope->GetVariable(identifier);
		if (!variable)
		{
			auto function = GetFunction(identifier);
			if (function)
			{
				return Value(Value::Function(function->block.get(), function->parameters));
			}
			throw InterpreterException("Variable nor function with such name was not declared.", currentPosition);
		}
		if (variable->value)
		{
			return *variable->value;
		}
		throw InterpreterException("Variable does not have value.", currentPosition);
	}

	throw InterpreterException("Could not evaluate bindable value", currentPosition);
}

Value Interpreter::EvaluateFunctionLiteral(const FunctionLiteral* const functionLiteral)
{
	currentPosition = functionLiteral->startingPosition;
	if (!functionLiteral->block)
	{
		throw InterpreterException("Function literal does not have block.", currentPosition);
	}
	return Value(Value::Function(functionLiteral->block.get(), functionLiteral->parameters));
}

const FunctionDefiniton* Interpreter::GetFunction(const std::wstring& identifier) const noexcept
{
	for (auto& func : knownFunctions)
	{
		if (func->identifier == identifier)
		{
			return func;
		}
	}
	return nullptr;
}

bool Interpreter::FunctionAlreadyExists(const std::wstring& identifier) const noexcept
{
	return GetFunction(identifier) != nullptr;
}

void Interpreter::Print(const std::wstring& msg) const noexcept
{
	std::wstring tabulation(currentDepth, L'\t');
	std::wcout << tabulation << msg << std::endl;
}