#include <gtest/gtest.h>
#include "Interpreter.h"

class InterpreterTest : public Interpreter
{
public:
	void InterpretFunDef(const FunctionDefiniton* const funDef, const std::vector<Value>& arguments = {})
	{
		Interpreter::InterpretFunDef(funDef, arguments);
	}

	void InterpretFunction(const Value::Function* const function, const std::vector<Value>& arguments)
	{
		Interpreter::InterpretFunction(function, arguments);
	}

	void InterpretFunctionCall(const FunctionCall* const functionCall, const bool valueExpected)
	{
		Interpreter::InterpretFunctionCall(functionCall, valueExpected);
	}

	const FunctionDefiniton* GetFunctionDefintion(const std::wstring& identifier)const noexcept
	{
		return Interpreter::GetFunctionDefintion(identifier);
	}

	Value EvaluateExpression(const Expression* const expression)
	{
		return Interpreter::EvaluateExpression(expression);
	}

	Value EvaluateConjunction(const Conjunction* const conjunction)
	{
		return Interpreter::EvaluateConjunction(conjunction);
	}

	Value EvaluateRelation(const Relation* const relation)
	{
		return Interpreter::EvaluateRelation(relation);
	}

	Value EvaluateAdditive(const Additive* const additive)
	{
		return Interpreter::EvaluateAdditive(additive);
	}

	Value EvaluateMultiplicative(const Multiplicative* const multiplicative)
	{
		return Interpreter::EvaluateMultiplicative(multiplicative);
	}

	Value EvaluateFactor(const Factor* const factor)
	{
		return Interpreter::EvaluateFactor(factor);
	}

	Value EvaluateLiteral(const Literal& literal)
	{
		return Interpreter::EvaluateLiteral(literal);
	}

	Value EvaluateComposable(const Composable* const composable)
	{
		return Interpreter::EvaluateComposable(composable);
	}

	Value EvaluateBindable(const Bindable* const bindable)
	{
		return Interpreter::EvaluateBindable(bindable);
	}

	Value EvaluateFunctionLiteral(const FunctionLiteral* const functionLiteral)
	{
		return Interpreter::EvaluateFunctionLiteral(functionLiteral);
	}

	const FunctionDefiniton* GetFunction(const std::wstring& identifier) const noexcept
	{
		return Interpreter::GetFunction(identifier);
	}
};