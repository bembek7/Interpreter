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

	const FunctionDefiniton* GetFunctionDefintion(const std::wstring& identifier) const noexcept
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

class InterpreterTests : public ::testing::Test
{
protected:
	InterpreterTest interpreter;
};

TEST_F(InterpreterTests, EvaluateLiteralInt)
{
	Literal literal;
	literal.value = 42;
	Value result = interpreter.EvaluateLiteral(literal);
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateLiteralBool)
{
	Literal literal;
	literal.value = true;
	Value result = interpreter.EvaluateLiteral(literal);
	EXPECT_EQ(std::get<bool>(result.value), true);
}

TEST_F(InterpreterTests, EvaluateLiteralString)
{
	Literal literal;
	literal.value = std::wstring(L"test");
	Value result = interpreter.EvaluateLiteral(literal);
	EXPECT_EQ(std::get<std::wstring>(result.value), L"test");
}

TEST_F(InterpreterTests, EvaluateLiteralFloat)
{
	Literal literal;
	literal.value = 3.14f;
	Value result = interpreter.EvaluateLiteral(literal);
	EXPECT_FLOAT_EQ(std::get<float>(result.value), 3.14f);
}

TEST_F(InterpreterTests, EvaluateFactor_WithLiteral) {
	Literal literal;
	literal.value = 42;
	Factor factor(literal);

	Value result = interpreter.EvaluateFactor(&factor);

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateFactor_WithLogicallyNegatedLiteral) {
	Literal literal;
	literal.value = true;
	Factor factor(literal, true);

	Value result = interpreter.EvaluateFactor(&factor);

	ASSERT_TRUE(std::holds_alternative<bool>(result.value));
	EXPECT_EQ(std::get<bool>(result.value), false);
}

TEST_F(InterpreterTests, EvaluateFactor_WithStringLiteral) {
	Literal literal;
	literal.value = std::wstring(L"test");
	Factor factor(literal);

	Value result = interpreter.EvaluateFactor(&factor);

	ASSERT_TRUE(std::holds_alternative<std::wstring>(result.value));
	EXPECT_EQ(std::get<std::wstring>(result.value), L"test");
}

TEST_F(InterpreterTests, EvaluateMultiplicative_SingleFactor) {
	Literal literal;
	literal.value = 42;
	auto factor = std::make_unique<Factor>(literal);
	Multiplicative multiplicative;
	multiplicative.factors.push_back(std::move(factor));

	Value result = interpreter.EvaluateMultiplicative(&multiplicative);

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateMultiplicative_MultipleFactors) {
	Literal literal1;
	literal1.value = 2;
	auto factor1 = std::make_unique<Factor>(literal1);

	Literal literal2;
	literal2.value = 3;
	auto factor2 = std::make_unique<Factor>(literal2);

	Multiplicative multiplicative;
	multiplicative.factors.push_back(std::move(factor1));
	multiplicative.factors.push_back(std::move(factor2));
	multiplicative.operators.push_back(MultiplicationOperator::Multiply);

	Value result = interpreter.EvaluateMultiplicative(&multiplicative);

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 6);
}

TEST_F(InterpreterTests, EvaluateMultiplicative_Division) {
	Literal literal1;
	literal1.value = 6;
	auto factor1 = std::make_unique<Factor>(literal1);

	Literal literal2;
	literal2.value = 2;
	auto factor2 = std::make_unique<Factor>(literal2);

	Multiplicative multiplicative;
	multiplicative.factors.push_back(std::move(factor1));
	multiplicative.factors.push_back(std::move(factor2));
	multiplicative.operators.push_back(MultiplicationOperator::Divide);

	Value result = interpreter.EvaluateMultiplicative(&multiplicative);

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 3);
}

TEST_F(InterpreterTests, EvaluateAdditive_SingleMultiplicative) {
	Literal literal;
	literal.value = 42;
	auto factor = std::make_unique<Factor>(literal);
	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->factors.push_back(std::move(factor));
	Additive additive;
	additive.multiplicatives.push_back(std::move(multiplicative));

	Value result = interpreter.EvaluateAdditive(&additive);

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateAdditive_MultipleMultiplicatives) {
	Literal literal1;
	literal1.value = 21;
	auto factor1 = std::make_unique<Factor>(literal1);
	auto multiplicative1 = std::make_unique<Multiplicative>();
	multiplicative1->factors.push_back(std::move(factor1));

	Literal literal2;
	literal2.value = 21;
	auto factor2 = std::make_unique<Factor>(literal2);
	auto multiplicative2 = std::make_unique<Multiplicative>();
	multiplicative2->factors.push_back(std::move(factor2));

	Additive additive;
	additive.multiplicatives.push_back(std::move(multiplicative1));
	additive.multiplicatives.push_back(std::move(multiplicative2));
	additive.operators.push_back(AdditionOperator::Plus);

	Value result = interpreter.EvaluateAdditive(&additive);

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateRelation_SingleAdditive) {
	Literal literal;
	literal.value = true;
	auto factor = std::make_unique<Factor>(literal);
	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->factors.push_back(std::move(factor));
	auto additive = std::make_unique<Additive>();
	additive->multiplicatives.push_back(std::move(multiplicative));
	Relation relation;
	relation.firstAdditive = std::move(additive);

	Value result = interpreter.EvaluateRelation(&relation);

	ASSERT_TRUE(std::holds_alternative<bool>(result.value));
	EXPECT_EQ(std::get<bool>(result.value), true);
}

TEST_F(InterpreterTests, EvaluateRelation_MultipleAdditives) {
	Literal literal1;
	literal1.value = 42;
	auto factor1 = std::make_unique<Factor>(literal1);
	auto multiplicative1 = std::make_unique<Multiplicative>();
	multiplicative1->factors.push_back(std::move(factor1));
	auto additive1 = std::make_unique<Additive>();
	additive1->multiplicatives.push_back(std::move(multiplicative1));

	Literal literal2;
	literal2.value = 42;
	auto factor2 = std::make_unique<Factor>(literal2);
	auto multiplicative2 = std::make_unique<Multiplicative>();
	multiplicative2->factors.push_back(std::move(factor2));
	auto additive2 = std::make_unique<Additive>();
	additive2->multiplicatives.push_back(std::move(multiplicative2));

	Relation relation;
	relation.firstAdditive = std::move(additive1);
	relation.secondAdditive = std::move(additive2);
	relation.relationOperator = RelationOperator::Equal;

	Value result = interpreter.EvaluateRelation(&relation);

	ASSERT_TRUE(std::holds_alternative<bool>(result.value));
	EXPECT_EQ(std::get<bool>(result.value), true);
}

TEST_F(InterpreterTests, EvaluateConjunction_SingleRelation) {
	Literal literal;
	literal.value = true;
	auto factor = std::make_unique<Factor>(literal);
	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->factors.push_back(std::move(factor));
	auto additive = std::make_unique<Additive>();
	additive->multiplicatives.push_back(std::move(multiplicative));
	auto relation = std::make_unique<Relation>();
	relation->firstAdditive = std::move(additive);
	Conjunction conjunction;
	conjunction.relations.push_back(std::move(relation));

	Value result = interpreter.EvaluateConjunction(&conjunction);

	ASSERT_TRUE(std::holds_alternative<bool>(result.value));
	EXPECT_EQ(std::get<bool>(result.value), true);
}

TEST_F(InterpreterTests, EvaluateConjunction_MultipleRelations) {
	Literal literal;
	literal.value = true;
	auto factor1 = std::make_unique<Factor>(literal);
	auto multiplicative1 = std::make_unique<Multiplicative>();
	multiplicative1->factors.push_back(std::move(factor1));
	auto additive1 = std::make_unique<Additive>();
	additive1->multiplicatives.push_back(std::move(multiplicative1));
	auto relation1 = std::make_unique<Relation>();
	relation1->firstAdditive = std::move(additive1);

	auto factor2 = std::make_unique<Factor>(literal);
	auto multiplicative2 = std::make_unique<Multiplicative>();
	multiplicative2->factors.push_back(std::move(factor2));
	auto additive2 = std::make_unique<Additive>();
	additive2->multiplicatives.push_back(std::move(multiplicative2));
	auto relation2 = std::make_unique<Relation>();
	relation2->firstAdditive = std::move(additive2);

	Conjunction conjunction;
	conjunction.relations.push_back(std::move(relation1));
	conjunction.relations.push_back(std::move(relation2));

	Value result = interpreter.EvaluateConjunction(&conjunction);

	ASSERT_TRUE(std::holds_alternative<bool>(result.value));
	EXPECT_EQ(std::get<bool>(result.value), true);
}
