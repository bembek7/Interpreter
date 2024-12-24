#include <gtest/gtest.h>
#include "Interpreter.h"
#include <ParserImpl.h>

static std::unique_ptr<Program> ParseStringAsProgram(const std::wstring& input) {
	std::wstringstream inputStream(input);
	Lexer lexer(&inputStream);
	ParserImpl parser(&lexer);
	return parser.ParseProgram();
}

std::unique_ptr<StandardExpression> ParseStringAsStandardExpression(const std::wstring& input) {
	std::wstringstream inputStream(input);
	Lexer lexer(&inputStream);
	ParserImpl parser(&lexer);
	return parser.ParseStandardExpression();
}

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

	bool FunctionAlreadyExists(const std::wstring& identifier) const noexcept
	{
		return Interpreter::FunctionAlreadyExists(identifier);
	}
};

static std::unique_ptr<Return> CreateReturnFromLiteral(const Literal& literal) {
	auto factor = std::make_unique<Factor>(literal);
	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->factors.push_back(std::move(factor));

	auto additive = std::make_unique<Additive>();
	additive->multiplicatives.push_back(std::move(multiplicative));

	auto relation = std::make_unique<Relation>();
	relation->firstAdditive = std::move(additive);

	auto conjunction = std::make_unique<Conjunction>();
	conjunction->relations.push_back(std::move(relation));

	auto standardExpression = std::make_unique<StandardExpression>();
	standardExpression->conjunctions.push_back(std::move(conjunction));

	return std::make_unique<Return>(std::move(standardExpression));
}

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

TEST_F(InterpreterTests, EvaluateFunctionLiteral_ValidFunctionLiteral_ReturnsFunctionValue) {
	std::vector<Param> parameters = { Param(L"param1"), Param(L"param2") };
	auto block = std::make_unique<Block>();
	FunctionLiteral functionLiteral{ parameters, std::move(block) };

	Value result = interpreter.EvaluateFunctionLiteral(&functionLiteral);

	ASSERT_TRUE(std::holds_alternative<Value::Function>(result.value));
	const auto& function = std::get<Value::Function>(result.value);
	EXPECT_EQ(function.parameters.size(), 2);
	EXPECT_EQ(function.parameters[0].identifier, L"param1");
	EXPECT_EQ(function.parameters[1].identifier, L"param2");
	EXPECT_EQ(function.block, functionLiteral.block.get());
}

TEST_F(InterpreterTests, EvaluateFunctionLiteral_EmptyParameters_ReturnsFunctionValue) {
	std::vector<Param> parameters;
	auto block = std::make_unique<Block>();
	FunctionLiteral functionLiteral{ parameters, std::move(block) };

	Value result = interpreter.EvaluateFunctionLiteral(&functionLiteral);

	ASSERT_TRUE(std::holds_alternative<Value::Function>(result.value));
	const auto& function = std::get<Value::Function>(result.value);
	EXPECT_TRUE(function.parameters.empty());
	EXPECT_EQ(function.block, functionLiteral.block.get());
}

TEST_F(InterpreterTests, EvaluateFunctionLiteral_NullBlock_ThrowsException) {
	std::vector<Param> parameters = { Param(L"param1") };
	FunctionLiteral functionLiteral{ parameters, nullptr };

	EXPECT_THROW(interpreter.EvaluateFunctionLiteral(&functionLiteral), InterpreterException);
}

TEST_F(InterpreterTests, EvaluateFunctionLiteral_SingleParameter_ReturnsFunctionValue) {
	std::vector<Param> parameters = { Param(L"param1") };
	auto block = std::make_unique<Block>();
	FunctionLiteral functionLiteral{ parameters, std::move(block) };

	Value result = interpreter.EvaluateFunctionLiteral(&functionLiteral);

	ASSERT_TRUE(std::holds_alternative<Value::Function>(result.value));
	const auto& function = std::get<Value::Function>(result.value);
	EXPECT_EQ(function.parameters.size(), 1);
	EXPECT_EQ(function.parameters[0].identifier, L"param1");
	EXPECT_EQ(function.block, functionLiteral.block.get());
}

TEST_F(InterpreterTests, EvaluateFunctionLiteral_MultipleParameters_ReturnsFunctionValue) {
	std::vector<Param> parameters = { Param(L"param1"), Param(L"param2"), Param(L"param3") };
	auto block = std::make_unique<Block>();
	FunctionLiteral functionLiteral{ parameters, std::move(block) };

	Value result = interpreter.EvaluateFunctionLiteral(&functionLiteral);

	ASSERT_TRUE(std::holds_alternative<Value::Function>(result.value));
	const auto& function = std::get<Value::Function>(result.value);
	EXPECT_EQ(function.parameters.size(), 3);
	EXPECT_EQ(function.parameters[0].identifier, L"param1");
	EXPECT_EQ(function.parameters[1].identifier, L"param2");
	EXPECT_EQ(function.parameters[2].identifier, L"param3");
	EXPECT_EQ(function.block, functionLiteral.block.get());
}

TEST_F(InterpreterTests, EvaluateFunctionLiteral_EmptyBlock_ReturnsFunctionValue) {
	std::vector<Param> parameters = { Param(L"param1") };
	auto block = std::make_unique<Block>(std::vector<std::unique_ptr<Statement>>{});
	FunctionLiteral functionLiteral{ parameters, std::move(block) };

	Value result = interpreter.EvaluateFunctionLiteral(&functionLiteral);

	ASSERT_TRUE(std::holds_alternative<Value::Function>(result.value));
	const auto& function = std::get<Value::Function>(result.value);
	EXPECT_EQ(function.parameters.size(), 1);
	EXPECT_EQ(function.parameters[0].identifier, L"param1");
	EXPECT_EQ(function.block, functionLiteral.block.get());
}

TEST_F(InterpreterTests, EvaluateFunctionLiteral_NonEmptyBlock_ReturnsFunctionValue) {
	std::vector<Param> parameters = { Param(L"param1") };
	auto block = std::make_unique<Block>();
	block->statements.push_back(CreateReturnFromLiteral(Literal{ 42 }));
	FunctionLiteral functionLiteral{ parameters, std::move(block) };

	Value result = interpreter.EvaluateFunctionLiteral(&functionLiteral);

	ASSERT_TRUE(std::holds_alternative<Value::Function>(result.value));
	const auto& function = std::get<Value::Function>(result.value);
	EXPECT_EQ(function.parameters.size(), 1);
	EXPECT_EQ(function.parameters[0].identifier, L"param1");
	EXPECT_EQ(function.block, functionLiteral.block.get());
}

TEST_F(InterpreterTests, GetFunction_FunctionExists) {
	std::wstring programCode = L"func testFunction() { return 42; }";
	auto program = ParseStringAsProgram(programCode);
	interpreter.Interpret(program.get());

	const FunctionDefiniton* result = interpreter.GetFunction(L"testFunction");

	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->identifier, L"testFunction");
}

TEST_F(InterpreterTests, GetFunction_FunctionDoesNotExist) {
	std::wstring programCode = L"func testFunction() { return 42; }";
	auto program = ParseStringAsProgram(programCode);
	interpreter.Interpret(program.get());

	const FunctionDefiniton* result = interpreter.GetFunction(L"nonExistentFunction");

	EXPECT_EQ(result, nullptr);
}

TEST_F(InterpreterTests, FunctionAlreadyExists_FunctionExists) {
	std::wstring programCode = L"func testFunction() { return 42; }";
	auto program = ParseStringAsProgram(programCode);
	interpreter.Interpret(program.get());

	bool result = interpreter.FunctionAlreadyExists(L"testFunction");

	EXPECT_TRUE(result);
}

TEST_F(InterpreterTests, FunctionAlreadyExists_FunctionDoesNotExist) {
	std::wstring programCode = L"func testFunction() { return 42; }";
	auto program = ParseStringAsProgram(programCode);
	interpreter.Interpret(program.get());

	bool result = interpreter.FunctionAlreadyExists(L"nonExistentFunction");

	EXPECT_FALSE(result);
}

TEST_F(InterpreterTests, GetFunctionDefintion_FunctionExists) {
	std::wstring programCode = L"func testFunction() { return 42; }";
	auto program = ParseStringAsProgram(programCode);
	interpreter.Interpret(program.get());

	const FunctionDefiniton* result = interpreter.GetFunctionDefintion(L"testFunction");

	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->identifier, L"testFunction");
}

TEST_F(InterpreterTests, GetFunctionDefintion_FunctionDoesNotExist) {
	std::wstring programCode = L"func testFunction() { return 42; }";
	auto program = ParseStringAsProgram(programCode);
	interpreter.Interpret(program.get());

	const FunctionDefiniton* result = interpreter.GetFunctionDefintion(L"nonExistentFunction");

	EXPECT_EQ(result, nullptr);
}

TEST_F(InterpreterTests, EvaluateStandardExpression_WithLiteral) {
	std::wstring expressionCode = L"42";
	auto expression = ParseStringAsStandardExpression(expressionCode);

	Value result = interpreter.EvaluateStandardExpression(expression.get());

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateStandardExpression_WithAddition) {
	std::wstring expressionCode = L"21 + 21";
	auto expression = ParseStringAsStandardExpression(expressionCode);

	Value result = interpreter.EvaluateStandardExpression(expression.get());

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateStandardExpression_WithMultiplication) {
	std::wstring expressionCode = L"6 * 7";
	auto expression = ParseStringAsStandardExpression(expressionCode);

	Value result = interpreter.EvaluateStandardExpression(expression.get());

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateStandardExpression_WithDivision) {
	std::wstring expressionCode = L"84 / 2";
	auto expression = ParseStringAsStandardExpression(expressionCode);

	Value result = interpreter.EvaluateStandardExpression(expression.get());

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}

TEST_F(InterpreterTests, EvaluateStandardExpression_WithSubtraction) {
	std::wstring expressionCode = L"50 - 8";
	auto expression = ParseStringAsStandardExpression(expressionCode);

	Value result = interpreter.EvaluateStandardExpression(expression.get());

	ASSERT_TRUE(std::holds_alternative<int>(result.value));
	EXPECT_EQ(std::get<int>(result.value), 42);
}