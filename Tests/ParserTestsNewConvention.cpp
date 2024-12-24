#include <gtest/gtest.h>
#include "ParserImpl.h"

template <typename T>
using vecUni = std::vector<std::unique_ptr<T>>;

class ParserTest : public ParserImpl
{
public:
	ParserTest(Lexer* const lexer)
		: ParserImpl(lexer)
	{
	}
};

class ParserTestNewConvention : public ::testing::Test
{
public:
};

TEST_F(ParserTestNewConvention, ParseConditional_TrueCondition_NoElse)
{
	std::wstringstream input(L"if (true) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto conditional = parser.ParseConditional();
	ASSERT_NE(conditional, nullptr);
	ASSERT_NE(conditional->condition, nullptr);
	ASSERT_NE(conditional->ifBlock, nullptr);
	EXPECT_EQ(conditional->elseBlock, nullptr);

	auto* condition = dynamic_cast<StandardExpression*>(conditional->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), true);

	EXPECT_TRUE(conditional->ifBlock->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseConditional_FalseCondition_NoElse)
{
	std::wstringstream input(L"if (false) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto conditional = parser.ParseConditional();
	ASSERT_NE(conditional, nullptr);
	ASSERT_NE(conditional->condition, nullptr);
	ASSERT_NE(conditional->ifBlock, nullptr);
	EXPECT_EQ(conditional->elseBlock, nullptr);

	auto* condition = dynamic_cast<StandardExpression*>(conditional->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), false);

	EXPECT_TRUE(conditional->ifBlock->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseConditional_TrueCondition_WithElse)
{
	std::wstringstream input(L"if (true) {} else {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto conditional = parser.ParseConditional();
	ASSERT_NE(conditional, nullptr);
	ASSERT_NE(conditional->condition, nullptr);
	ASSERT_NE(conditional->ifBlock, nullptr);
	ASSERT_NE(conditional->elseBlock, nullptr);

	auto* condition = dynamic_cast<StandardExpression*>(conditional->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), true);

	EXPECT_TRUE(conditional->ifBlock->statements.empty());
	EXPECT_TRUE(conditional->elseBlock->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseConditional_FalseCondition_WithElse)
{
	std::wstringstream input(L"if (false) {} else {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto conditional = parser.ParseConditional();
	ASSERT_NE(conditional, nullptr);
	ASSERT_NE(conditional->condition, nullptr);
	ASSERT_NE(conditional->ifBlock, nullptr);
	ASSERT_NE(conditional->elseBlock, nullptr);

	auto* condition = dynamic_cast<StandardExpression*>(conditional->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), false);

	EXPECT_TRUE(conditional->ifBlock->statements.empty());
	EXPECT_TRUE(conditional->elseBlock->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseFunctionLit_ValidFunctionLit)
{
	std::wstringstream input(L"(a, b) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionLit = parser.ParseFunctionLit();
	ASSERT_NE(functionLit, nullptr);
	ASSERT_EQ(functionLit->parameters.size(), 2);
	EXPECT_EQ(functionLit->parameters[0].identifier, L"a");
	EXPECT_EQ(functionLit->parameters[1].identifier, L"b");
	ASSERT_NE(functionLit->block, nullptr);
	EXPECT_TRUE(functionLit->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseFunctionLit_ValidFunctionLitWithMutableParam)
{
	std::wstringstream input(L"(mut a, b) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionLit = parser.ParseFunctionLit();
	ASSERT_NE(functionLit, nullptr);
	ASSERT_EQ(functionLit->parameters.size(), 2);
	EXPECT_EQ(functionLit->parameters[0].identifier, L"a");
	EXPECT_TRUE(functionLit->parameters[0].paramMutable);
	EXPECT_EQ(functionLit->parameters[1].identifier, L"b");
	EXPECT_FALSE(functionLit->parameters[1].paramMutable);
	ASSERT_NE(functionLit->block, nullptr);
	EXPECT_TRUE(functionLit->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseFunctionLit_MissingParameters)
{
	std::wstringstream input(L"() {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionLit = parser.ParseFunctionLit();
	ASSERT_NE(functionLit, nullptr);
	EXPECT_TRUE(functionLit->parameters.empty());
	ASSERT_NE(functionLit->block, nullptr);
	EXPECT_TRUE(functionLit->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseFunctionLit_MissingBlock)
{
	std::wstringstream input(L"(a, b)");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseFunctionLit(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseFunctionLit_InvalidSyntax)
{
	std::wstringstream input(L"(a, b {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseFunctionLit(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseBindable_ValidFunctionLiteral)
{
	std::wstringstream input(L"() {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto bindable = parser.ParseBindable();
	ASSERT_NE(bindable, nullptr);
	auto* functionLit = std::get_if<std::unique_ptr<FunctionLiteral>>(&bindable->bindable);
	ASSERT_NE(functionLit, nullptr);
	EXPECT_TRUE((*functionLit)->parameters.empty());
	ASSERT_NE((*functionLit)->block, nullptr);
	EXPECT_TRUE((*functionLit)->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseBindable_ValidFunctionCall)
{
	std::wstringstream input(L"foo()");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto bindable = parser.ParseBindable();
	ASSERT_NE(bindable, nullptr);
	auto* functionCall = std::get_if<std::unique_ptr<FunctionCall>>(&bindable->bindable);
	ASSERT_NE(functionCall, nullptr);
	EXPECT_EQ((*functionCall)->identifier, L"foo");
	EXPECT_TRUE((*functionCall)->arguments.empty());
}

TEST_F(ParserTestNewConvention, ParseBindable_ValidIdentifier)
{
	std::wstringstream input(L"foo");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto bindable = parser.ParseBindable();
	ASSERT_NE(bindable, nullptr);
	auto* identifier = std::get_if<std::wstring>(&bindable->bindable);
	ASSERT_NE(identifier, nullptr);
	EXPECT_EQ(*identifier, L"foo");
}

TEST_F(ParserTestNewConvention, ParseBindable_InvalidSyntax)
{
	std::wstringstream input(L"(");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseBindable(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseComposable_ValidBindable)
{
	std::wstringstream input(L"foo");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto composable = parser.ParseComposable();
	ASSERT_NE(composable, nullptr);
	ASSERT_NE(composable->bindable, nullptr);
	auto* identifier = std::get_if<std::wstring>(&composable->bindable->bindable);
	ASSERT_NE(identifier, nullptr);
	EXPECT_EQ(*identifier, L"foo");
	EXPECT_TRUE(composable->arguments.empty());
}

TEST_F(ParserTestNewConvention, ParseComposable_ValidBindableWithArguments)
{
	std::wstringstream input(L"foo << (bar, 42)");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto composable = parser.ParseComposable();
	ASSERT_NE(composable, nullptr);
	ASSERT_NE(composable->bindable, nullptr);
	auto* identifier = std::get_if<std::wstring>(&composable->bindable->bindable);
	ASSERT_NE(identifier, nullptr);
	EXPECT_EQ(*identifier, L"foo");
	ASSERT_EQ(composable->arguments.size(), 2);
	auto* arg1 = dynamic_cast<StandardExpression*>(composable->arguments[0].get());
	ASSERT_NE(arg1, nullptr);
	auto* arg1Literal = std::get_if<std::wstring>(&arg1->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	EXPECT_EQ(*arg1Literal, L"bar");

	auto* arg2 = dynamic_cast<StandardExpression*>(composable->arguments[1].get());
	ASSERT_NE(arg2, nullptr);
	auto* arg2Literal = std::get_if<Literal>(&arg2->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(arg2Literal, nullptr);
	EXPECT_EQ(std::get<int>(arg2Literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseComposable_InvalidSyntax)
{
	std::wstringstream input(L"foo << (bar, 42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseComposable(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseComposable_EmptyInput)
{
	std::wstringstream input(L"");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto composable = parser.ParseComposable();
	EXPECT_EQ(composable, nullptr);
}

TEST_F(ParserTestNewConvention, ParseFuncExpression_ValidSingleComposable)
{
	std::wstringstream input(L"foo");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto funcExpression = parser.ParseFuncExpression();
	ASSERT_NE(funcExpression, nullptr);
	ASSERT_EQ(funcExpression->composables.size(), 1);
	auto* composable = funcExpression->composables[0].get();
	ASSERT_NE(composable, nullptr);
	auto* identifier = std::get_if<std::wstring>(&composable->bindable->bindable);
	ASSERT_NE(identifier, nullptr);
	EXPECT_EQ(*identifier, L"foo");
}

TEST_F(ParserTestNewConvention, ParseFuncExpression_ValidComposableChain)
{
	std::wstringstream input(L"foo >> bar");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto funcExpression = parser.ParseFuncExpression();
	ASSERT_NE(funcExpression, nullptr);
	ASSERT_EQ(funcExpression->composables.size(), 2);

	auto* composable1 = funcExpression->composables[0].get();
	ASSERT_NE(composable1, nullptr);
	auto* identifier1 = std::get_if<std::wstring>(&composable1->bindable->bindable);
	ASSERT_NE(identifier1, nullptr);
	EXPECT_EQ(*identifier1, L"foo");

	auto* composable2 = funcExpression->composables[1].get();
	ASSERT_NE(composable2, nullptr);
	auto* identifier2 = std::get_if<std::wstring>(&composable2->bindable->bindable);
	ASSERT_NE(identifier2, nullptr);
	EXPECT_EQ(*identifier2, L"bar");
}

TEST_F(ParserTestNewConvention, ParseFuncExpression_ValidComposableWithArguments)
{
	std::wstringstream input(L"foo << (bar, 42) >> baz");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto funcExpression = parser.ParseFuncExpression();
	ASSERT_NE(funcExpression, nullptr);
	ASSERT_EQ(funcExpression->composables.size(), 2);

	auto* composable1 = funcExpression->composables[0].get();
	ASSERT_NE(composable1, nullptr);
	auto* identifier1 = std::get_if<std::wstring>(&composable1->bindable->bindable);
	ASSERT_NE(identifier1, nullptr);
	EXPECT_EQ(*identifier1, L"foo");
	ASSERT_EQ(composable1->arguments.size(), 2);
	auto* arg1 = dynamic_cast<StandardExpression*>(composable1->arguments[0].get());
	ASSERT_NE(arg1, nullptr);
	auto* arg1Literal = std::get_if<std::wstring>(&arg1->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	EXPECT_EQ(*arg1Literal, L"bar");

	auto* arg2 = dynamic_cast<StandardExpression*>(composable1->arguments[1].get());
	ASSERT_NE(arg2, nullptr);
	auto* arg2Literal = std::get_if<Literal>(&arg2->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(arg2Literal, nullptr);
	EXPECT_EQ(std::get<int>(arg2Literal->value), 42);

	auto* composable2 = funcExpression->composables[1].get();
	ASSERT_NE(composable2, nullptr);
	auto* identifier2 = std::get_if<std::wstring>(&composable2->bindable->bindable);
	ASSERT_NE(identifier2, nullptr);
	EXPECT_EQ(*identifier2, L"baz");
}

TEST_F(ParserTestNewConvention, ParseFuncExpression_InvalidSyntax)
{
	std::wstringstream input(L"foo >>");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseFuncExpression(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseFuncExpression_EmptyInput)
{
	std::wstringstream input(L"");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto funcExpression = parser.ParseFuncExpression();
	EXPECT_EQ(funcExpression, nullptr);
}

TEST_F(ParserTestNewConvention, ParseAdditive_ValidSingleMultiplicative)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto additive = parser.ParseAdditive();
	ASSERT_NE(additive, nullptr);
	ASSERT_EQ(additive->multiplicatives.size(), 1);
	auto* multiplicative = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative, nullptr);
	ASSERT_EQ(multiplicative->factors.size(), 1);
	auto* factor = multiplicative->factors[0].get();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseAdditive_ValidAdditiveExpression)
{
	std::wstringstream input(L"42 + 58");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto additive = parser.ParseAdditive();
	ASSERT_NE(additive, nullptr);
	ASSERT_EQ(additive->multiplicatives.size(), 2);
	ASSERT_EQ(additive->operators.size(), 1);
	EXPECT_EQ(additive->operators[0], AdditionOperator::Plus);

	auto* multiplicative1 = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative1, nullptr);
	ASSERT_EQ(multiplicative1->factors.size(), 1);
	auto* factor1 = multiplicative1->factors[0].get();
	ASSERT_NE(factor1, nullptr);
	auto* literal1 = std::get_if<Literal>(&factor1->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<int>(literal1->value), 42);

	auto* multiplicative2 = additive->multiplicatives[1].get();
	ASSERT_NE(multiplicative2, nullptr);
	ASSERT_EQ(multiplicative2->factors.size(), 1);
	auto* factor2 = multiplicative2->factors[0].get();
	ASSERT_NE(factor2, nullptr);
	auto* literal2 = std::get_if<Literal>(&factor2->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<int>(literal2->value), 58);
}

TEST_F(ParserTestNewConvention, ParseAdditive_InvalidSyntax)
{
	std::wstringstream input(L"42 +");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseAdditive(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseFactor_ValidLiteral)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto factor = parser.ParseFactor();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseFactor_ValidNegatedLiteral)
{
	std::wstringstream input(L"!true");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto factor = parser.ParseFactor();
	ASSERT_NE(factor, nullptr);
	EXPECT_TRUE(factor->logicallyNegated);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), true);
}

TEST_F(ParserTestNewConvention, ParseFactor_ValidExpression)
{
	std::wstringstream input(L"(42)");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto factor = parser.ParseFactor();
	ASSERT_NE(factor, nullptr);
	auto* expression = std::get_if<std::unique_ptr<StandardExpression>>(&factor->factor);
	ASSERT_NE(expression, nullptr);
	auto* conjunction = (*expression)->conjunctions[0].get();
	ASSERT_NE(conjunction, nullptr);
	auto* relation = conjunction->relations[0].get();
	ASSERT_NE(relation, nullptr);
	auto* additive = relation->firstAdditive.get();
	ASSERT_NE(additive, nullptr);
	auto* multiplicative = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative, nullptr);
	auto* factorInner = multiplicative->factors[0].get();
	ASSERT_NE(factorInner, nullptr);
	auto* literal = std::get_if<Literal>(&factorInner->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseFactor_InvalidSyntax)
{
	std::wstringstream input(L"!");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseFactor(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseLiteral_ValidInteger)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto literal = parser.ParseLiteral();
	ASSERT_TRUE(literal.has_value());
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseLiteral_ValidFloat)
{
	std::wstringstream input(L"3.14");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto literal = parser.ParseLiteral();
	ASSERT_TRUE(literal.has_value());
	EXPECT_FLOAT_EQ(std::get<float>(literal->value), 3.14f);
}

TEST_F(ParserTestNewConvention, ParseLiteral_ValidString)
{
	std::wstringstream input(L"\"hello\"");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto literal = parser.ParseLiteral();
	ASSERT_TRUE(literal.has_value());
	EXPECT_EQ(std::get<std::wstring>(literal->value), L"hello");
}

TEST_F(ParserTestNewConvention, ParseLiteral_ValidBoolean)
{
	std::wstringstream input(L"true");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto literal = parser.ParseLiteral();
	ASSERT_TRUE(literal.has_value());
	EXPECT_EQ(std::get<bool>(literal->value), true);
}

TEST_F(ParserTestNewConvention, ParseLiteral_InvalidSyntax)
{
	std::wstringstream input(L"invalid");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto literal = parser.ParseLiteral();
	EXPECT_FALSE(literal.has_value());
}

TEST_F(ParserTestNewConvention, ParseExpression_ValidStandardExpression)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto expression = parser.ParseExpression();
	ASSERT_NE(expression, nullptr);
	auto* standardExpression = dynamic_cast<StandardExpression*>(expression.get());
	ASSERT_NE(standardExpression, nullptr);
	auto* conjunction = standardExpression->conjunctions[0].get();
	ASSERT_NE(conjunction, nullptr);
	auto* relation = conjunction->relations[0].get();
	ASSERT_NE(relation, nullptr);
	auto* additive = relation->firstAdditive.get();
	ASSERT_NE(additive, nullptr);
	auto* multiplicative = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative, nullptr);
	auto* factor = multiplicative->factors[0].get();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseExpression_ValidFuncExpression)
{
	std::wstringstream input(L"[foo >> bar]");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto expression = parser.ParseExpression();
	ASSERT_NE(expression, nullptr);
	auto* funcExpression = dynamic_cast<FuncExpression*>(expression.get());
	ASSERT_NE(funcExpression, nullptr);
	ASSERT_EQ(funcExpression->composables.size(), 2);

	auto* composable1 = funcExpression->composables[0].get();
	ASSERT_NE(composable1, nullptr);
	auto* identifier1 = std::get_if<std::wstring>(&composable1->bindable->bindable);
	ASSERT_NE(identifier1, nullptr);
	EXPECT_EQ(*identifier1, L"foo");

	auto* composable2 = funcExpression->composables[1].get();
	ASSERT_NE(composable2, nullptr);
	auto* identifier2 = std::get_if<std::wstring>(&composable2->bindable->bindable);
	ASSERT_NE(identifier2, nullptr);
	EXPECT_EQ(*identifier2, L"bar");
}

TEST_F(ParserTestNewConvention, ParseExpression_InvalidSyntax)
{
	std::wstringstream input(L"[foo");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseExpression(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseStandardExpression_ValidSingleConjunction)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto standardExpression = parser.ParseStandardExpression();
	ASSERT_NE(standardExpression, nullptr);
	ASSERT_EQ(standardExpression->conjunctions.size(), 1);
	auto* conjunction = standardExpression->conjunctions[0].get();
	ASSERT_NE(conjunction, nullptr);
	auto* relation = conjunction->relations[0].get();
	ASSERT_NE(relation, nullptr);
	auto* additive = relation->firstAdditive.get();
	ASSERT_NE(additive, nullptr);
	auto* multiplicative = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative, nullptr);
	auto* factor = multiplicative->factors[0].get();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseStandardExpression_ValidLogicalOr)
{
	std::wstringstream input(L"true || false");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto standardExpression = parser.ParseStandardExpression();
	ASSERT_NE(standardExpression, nullptr);
	ASSERT_EQ(standardExpression->conjunctions.size(), 2);

	auto* conjunction1 = standardExpression->conjunctions[0].get();
	ASSERT_NE(conjunction1, nullptr);
	auto* relation1 = conjunction1->relations[0].get();
	ASSERT_NE(relation1, nullptr);
	auto* additive1 = relation1->firstAdditive.get();
	ASSERT_NE(additive1, nullptr);
	auto* multiplicative1 = additive1->multiplicatives[0].get();
	ASSERT_NE(multiplicative1, nullptr);
	auto* factor1 = multiplicative1->factors[0].get();
	ASSERT_NE(factor1, nullptr);
	auto* literal1 = std::get_if<Literal>(&factor1->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<bool>(literal1->value), true);

	auto* conjunction2 = standardExpression->conjunctions[1].get();
	ASSERT_NE(conjunction2, nullptr);
	auto* relation2 = conjunction2->relations[0].get();
	ASSERT_NE(relation2, nullptr);
	auto* additive2 = relation2->firstAdditive.get();
	ASSERT_NE(additive2, nullptr);
	auto* multiplicative2 = additive2->multiplicatives[0].get();
	ASSERT_NE(multiplicative2, nullptr);
	auto* factor2 = multiplicative2->factors[0].get();
	ASSERT_NE(factor2, nullptr);
	auto* literal2 = std::get_if<Literal>(&factor2->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<bool>(literal2->value), false);
}

TEST_F(ParserTestNewConvention, ParseStandardExpression_InvalidSyntax)
{
	std::wstringstream input(L"true ||");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseStandardExpression(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseConjunction_ValidSingleRelation)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto conjunction = parser.ParseConjunction();
	ASSERT_NE(conjunction, nullptr);
	ASSERT_EQ(conjunction->relations.size(), 1);
	auto* relation = conjunction->relations[0].get();
	ASSERT_NE(relation, nullptr);
	auto* additive = relation->firstAdditive.get();
	ASSERT_NE(additive, nullptr);
	auto* multiplicative = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative, nullptr);
	auto* factor = multiplicative->factors[0].get();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseConjunction_ValidLogicalAnd)
{
	std::wstringstream input(L"true && false");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto conjunction = parser.ParseConjunction();
	ASSERT_NE(conjunction, nullptr);
	ASSERT_EQ(conjunction->relations.size(), 2);

	auto* relation1 = conjunction->relations[0].get();
	ASSERT_NE(relation1, nullptr);
	auto* additive1 = relation1->firstAdditive.get();
	ASSERT_NE(additive1, nullptr);
	auto* multiplicative1 = additive1->multiplicatives[0].get();
	ASSERT_NE(multiplicative1, nullptr);
	auto* factor1 = multiplicative1->factors[0].get();
	ASSERT_NE(factor1, nullptr);
	auto* literal1 = std::get_if<Literal>(&factor1->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<bool>(literal1->value), true);

	auto* relation2 = conjunction->relations[1].get();
	ASSERT_NE(relation2, nullptr);
	auto* additive2 = relation2->firstAdditive.get();
	ASSERT_NE(additive2, nullptr);
	auto* multiplicative2 = additive2->multiplicatives[0].get();
	ASSERT_NE(multiplicative2, nullptr);
	auto* factor2 = multiplicative2->factors[0].get();
	ASSERT_NE(factor2, nullptr);
	auto* literal2 = std::get_if<Literal>(&factor2->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<bool>(literal2->value), false);
}

TEST_F(ParserTestNewConvention, ParseConjunction_InvalidSyntax)
{
	std::wstringstream input(L"true &&");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseConjunction(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseRelation_ValidSingleAdditive)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto relation = parser.ParseRelation();
	ASSERT_NE(relation, nullptr);
	ASSERT_NE(relation->firstAdditive, nullptr);
	auto* additive = relation->firstAdditive.get();
	ASSERT_NE(additive, nullptr);
	auto* multiplicative = additive->multiplicatives[0].get();
	ASSERT_NE(multiplicative, nullptr);
	auto* factor = multiplicative->factors[0].get();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseRelation_ValidRelationOperator)
{
	std::wstringstream input(L"42 < 58");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto relation = parser.ParseRelation();
	ASSERT_NE(relation, nullptr);
	ASSERT_NE(relation->firstAdditive, nullptr);
	ASSERT_TRUE(relation->relationOperator.has_value());
	EXPECT_EQ(relation->relationOperator.value(), RelationOperator::Less);
	ASSERT_NE(relation->secondAdditive, nullptr);

	auto* additive1 = relation->firstAdditive.get();
	ASSERT_NE(additive1, nullptr);
	auto* multiplicative1 = additive1->multiplicatives[0].get();
	ASSERT_NE(multiplicative1, nullptr);
	auto* factor1 = multiplicative1->factors[0].get();
	ASSERT_NE(factor1, nullptr);
	auto* literal1 = std::get_if<Literal>(&factor1->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<int>(literal1->value), 42);

	auto* additive2 = relation->secondAdditive.get();
	ASSERT_NE(additive2, nullptr);
	auto* multiplicative2 = additive2->multiplicatives[0].get();
	ASSERT_NE(multiplicative2, nullptr);
	auto* factor2 = multiplicative2->factors[0].get();
	ASSERT_NE(factor2, nullptr);
	auto* literal2 = std::get_if<Literal>(&factor2->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<int>(literal2->value), 58);
}

TEST_F(ParserTestNewConvention, ParseRelation_InvalidSyntax)
{
	std::wstringstream input(L"42 <");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseRelation(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseMultiplicative_ValidSingleFactor)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto multiplicative = parser.ParseMultiplicative();
	ASSERT_NE(multiplicative, nullptr);
	ASSERT_EQ(multiplicative->factors.size(), 1);
	auto* factor = multiplicative->factors[0].get();
	ASSERT_NE(factor, nullptr);
	auto* literal = std::get_if<Literal>(&factor->factor);
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseMultiplicative_ValidMultiplicativeExpression)
{
	std::wstringstream input(L"42 * 2");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto multiplicative = parser.ParseMultiplicative();
	ASSERT_NE(multiplicative, nullptr);
	ASSERT_EQ(multiplicative->factors.size(), 2);
	ASSERT_EQ(multiplicative->operators.size(), 1);
	EXPECT_EQ(multiplicative->operators[0], MultiplicationOperator::Multiply);

	auto* factor1 = multiplicative->factors[0].get();
	ASSERT_NE(factor1, nullptr);
	auto* literal1 = std::get_if<Literal>(&factor1->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<int>(literal1->value), 42);

	auto* factor2 = multiplicative->factors[1].get();
	ASSERT_NE(factor2, nullptr);
	auto* literal2 = std::get_if<Literal>(&factor2->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<int>(literal2->value), 2);
}

TEST_F(ParserTestNewConvention, ParseMultiplicative_InvalidSyntax)
{
	std::wstringstream input(L"42 *");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseMultiplicative(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseRestOfFunctionCallStatement_ValidFunctionCall)
{
	std::wstringstream input(L"();");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionCallStatement = parser.ParseRestOfFunctionCallStatement(L"foo");
	ASSERT_NE(functionCallStatement, nullptr);
	ASSERT_NE(functionCallStatement->funcCall, nullptr);
	EXPECT_EQ(functionCallStatement->funcCall->identifier, L"foo");
	EXPECT_TRUE(functionCallStatement->funcCall->arguments.empty());
}

TEST_F(ParserTestNewConvention, ParseRestOfFunctionCallStatement_ValidFunctionCallWithArguments)
{
	std::wstringstream input(L"(42, \"bar\");");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionCallStatement = parser.ParseRestOfFunctionCallStatement(L"foo");
	ASSERT_NE(functionCallStatement, nullptr);
	ASSERT_NE(functionCallStatement->funcCall, nullptr);
	EXPECT_EQ(functionCallStatement->funcCall->identifier, L"foo");
	ASSERT_EQ(functionCallStatement->funcCall->arguments.size(), 2);

	auto* arg1 = dynamic_cast<StandardExpression*>(functionCallStatement->funcCall->arguments[0].get());
	ASSERT_NE(arg1, nullptr);
	auto* literal1 = std::get_if<Literal>(&arg1->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<int>(literal1->value), 42);

	auto* arg2 = dynamic_cast<StandardExpression*>(functionCallStatement->funcCall->arguments[1].get());
	ASSERT_NE(arg2, nullptr);
	auto* literal2 = std::get_if<Literal>(&arg2->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<std::wstring>(literal2->value), L"bar");
}

TEST_F(ParserTestNewConvention, ParseRestOfFunctionCallStatement_MissingSemicolon)
{
	std::wstringstream input(L"()");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseRestOfFunctionCallStatement(L"foo"), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseRestOfFunctionCall_ValidFunctionCall)
{
	std::wstringstream input(L"()");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionCall = parser.ParseRestOfFunctionCall(L"foo");
	ASSERT_NE(functionCall, nullptr);
	EXPECT_EQ(functionCall->identifier, L"foo");
	EXPECT_TRUE(functionCall->arguments.empty());
}

TEST_F(ParserTestNewConvention, ParseRestOfFunctionCall_ValidFunctionCallWithArguments)
{
	std::wstringstream input(L"(42, \"bar\")");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionCall = parser.ParseRestOfFunctionCall(L"foo");
	ASSERT_NE(functionCall, nullptr);
	EXPECT_EQ(functionCall->identifier, L"foo");
	ASSERT_EQ(functionCall->arguments.size(), 2);

	auto* arg1 = dynamic_cast<StandardExpression*>(functionCall->arguments[0].get());
	ASSERT_NE(arg1, nullptr);
	auto* literal1 = std::get_if<Literal>(&arg1->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<int>(literal1->value), 42);

	auto* arg2 = dynamic_cast<StandardExpression*>(functionCall->arguments[1].get());
	ASSERT_NE(arg2, nullptr);
	auto* literal2 = std::get_if<Literal>(&arg2->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<std::wstring>(literal2->value), L"bar");
}

TEST_F(ParserTestNewConvention, ParseRestOfFunctionCall_MissingClosingParenthesis)
{
	std::wstringstream input(L"(42, \"bar\"");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseRestOfFunctionCall(L"foo"), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseLoop_ValidWhileLoop)
{
	std::wstringstream input(L"while (true) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto whileLoop = parser.ParseLoop();
	ASSERT_NE(whileLoop, nullptr);
	ASSERT_NE(whileLoop->condition, nullptr);
	ASSERT_NE(whileLoop->block, nullptr);
	EXPECT_TRUE(whileLoop->block->statements.empty());

	auto* condition = dynamic_cast<StandardExpression*>(whileLoop->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), true);
}

TEST_F(ParserTestNewConvention, ParseLoop_InvalidSyntax)
{
	std::wstringstream input(L"while (true) {");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseLoop(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseReturn_ValidReturn)
{
	std::wstringstream input(L"return 42;");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto returnStatement = parser.ParseReturn();
	ASSERT_NE(returnStatement, nullptr);
	ASSERT_NE(returnStatement->expression, nullptr);

	auto* expression = dynamic_cast<StandardExpression*>(returnStatement->expression.get());
	ASSERT_NE(expression, nullptr);
	auto* literal = std::get_if<Literal>(&(expression->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseReturn_ValidReturnWithoutExpression)
{
	std::wstringstream input(L"return;");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto returnStatement = parser.ParseReturn();
	ASSERT_NE(returnStatement, nullptr);
	EXPECT_EQ(returnStatement->expression, nullptr);
}

TEST_F(ParserTestNewConvention, ParseReturn_InvalidSyntax)
{
	std::wstringstream input(L"return 42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseReturn(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseDeclaration_ValidDeclaration)
{
	std::wstringstream input(L"var foo = 42;");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto declaration = parser.ParseDeclaration();
	ASSERT_NE(declaration, nullptr);
	EXPECT_EQ(declaration->identifier, L"foo");
	ASSERT_NE(declaration->expression, nullptr);

	auto* expression = dynamic_cast<StandardExpression*>(declaration->expression.get());
	ASSERT_NE(expression, nullptr);
	auto* literal = std::get_if<Literal>(&(expression->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseDeclaration_ValidMutableDeclaration)
{
	std::wstringstream input(L"mut var foo = 42;");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto declaration = parser.ParseDeclaration();
	ASSERT_NE(declaration, nullptr);
	EXPECT_EQ(declaration->identifier, L"foo");
	EXPECT_TRUE(declaration->varMutable);
	ASSERT_NE(declaration->expression, nullptr);

	auto* expression = dynamic_cast<StandardExpression*>(declaration->expression.get());
	ASSERT_NE(expression, nullptr);
	auto* literal = std::get_if<Literal>(&(expression->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseDeclaration_InvalidSyntax)
{
	std::wstringstream input(L"var foo = 42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseDeclaration(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseRestOfAssignment_ValidAssignment)
{
	std::wstringstream input(L"= 42;");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto assignment = parser.ParseRestOfAssignment(L"foo");
	ASSERT_NE(assignment, nullptr);
	EXPECT_EQ(assignment->identifier, L"foo");
	ASSERT_NE(assignment->expression, nullptr);

	auto* expression = dynamic_cast<StandardExpression*>(assignment->expression.get());
	ASSERT_NE(expression, nullptr);
	auto* literal = std::get_if<Literal>(&(expression->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseRestOfAssignment_InvalidSyntax)
{
	std::wstringstream input(L"= 42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseRestOfAssignment(L"foo"), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseArguments_ValidArguments)
{
	std::wstringstream input(L"42, \"bar\"");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto arguments = parser.ParseArguments();
	ASSERT_EQ(arguments.size(), 2);

	auto* arg1 = dynamic_cast<StandardExpression*>(arguments[0].get());
	ASSERT_NE(arg1, nullptr);
	auto* literal1 = std::get_if<Literal>(&arg1->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(literal1, nullptr);
	EXPECT_EQ(std::get<int>(literal1->value), 42);

	auto* arg2 = dynamic_cast<StandardExpression*>(arguments[1].get());
	ASSERT_NE(arg2, nullptr);
	auto* literal2 = std::get_if<Literal>(&arg2->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor);
	ASSERT_NE(literal2, nullptr);
	EXPECT_EQ(std::get<std::wstring>(literal2->value), L"bar");
}

TEST_F(ParserTestNewConvention, ParseArguments_InvalidSyntax)
{
	std::wstringstream input(L"42,");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseArguments(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseFunctionDefinition_ValidFunctionDefinition)
{
	std::wstringstream input(L"func foo() {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionDefinition = parser.ParseFunctionDefinition();
	ASSERT_NE(functionDefinition, nullptr);
	EXPECT_EQ(functionDefinition->identifier, L"foo");
	ASSERT_NE(functionDefinition->block, nullptr);
	EXPECT_TRUE(functionDefinition->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseFunctionDefinition_ValidFunctionDefinitionWithParams)
{
	std::wstringstream input(L"func foo(a, b) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto functionDefinition = parser.ParseFunctionDefinition();
	ASSERT_NE(functionDefinition, nullptr);
	EXPECT_EQ(functionDefinition->identifier, L"foo");
	ASSERT_EQ(functionDefinition->parameters.size(), 2);
	EXPECT_EQ(functionDefinition->parameters[0].identifier, L"a");
	EXPECT_EQ(functionDefinition->parameters[1].identifier, L"b");
	ASSERT_NE(functionDefinition->block, nullptr);
	EXPECT_TRUE(functionDefinition->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseFunctionDefinition_InvalidSyntax)
{
	std::wstringstream input(L"func foo(a, b) {");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseFunctionDefinition(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseParams_ValidParams)
{
	std::wstringstream input(L"a, b");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto params = parser.ParseParams();
	ASSERT_EQ(params.size(), 2);
	EXPECT_EQ(params[0].identifier, L"a");
	EXPECT_EQ(params[1].identifier, L"b");
}

TEST_F(ParserTestNewConvention, ParseParams_ValidParamsWithMutable)
{
	std::wstringstream input(L"mut a, b");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto params = parser.ParseParams();
	ASSERT_EQ(params.size(), 2);
	EXPECT_EQ(params[0].identifier, L"a");
	EXPECT_TRUE(params[0].paramMutable);
	EXPECT_EQ(params[1].identifier, L"b");
	EXPECT_FALSE(params[1].paramMutable);
}

TEST_F(ParserTestNewConvention, ParseParams_InvalidSyntax)
{
	std::wstringstream input(L"a,");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseParams(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseParam_ValidParam)
{
	std::wstringstream input(L"a");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto param = parser.ParseParam();
	ASSERT_TRUE(param.has_value());
	EXPECT_EQ(param->identifier, L"a");
	EXPECT_FALSE(param->paramMutable);
}

TEST_F(ParserTestNewConvention, ParseParam_ValidMutableParam)
{
	std::wstringstream input(L"mut a");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto param = parser.ParseParam();
	ASSERT_TRUE(param.has_value());
	EXPECT_EQ(param->identifier, L"a");
	EXPECT_TRUE(param->paramMutable);
}

TEST_F(ParserTestNewConvention, ParseParam_InvalidSyntax)
{
	std::wstringstream input(L"mut");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseParam(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseBlock_ValidEmptyBlock)
{
	std::wstringstream input(L"{}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto block = parser.ParseBlock();
	ASSERT_NE(block, nullptr);
	EXPECT_TRUE(block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseBlock_ValidBlockWithStatements)
{
	std::wstringstream input(L"{ return 42; }");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto block = parser.ParseBlock();
	ASSERT_NE(block, nullptr);
	ASSERT_EQ(block->statements.size(), 1);

	auto* returnStatement = dynamic_cast<Return*>(block->statements[0].get());
	ASSERT_NE(returnStatement, nullptr);
	ASSERT_NE(returnStatement->expression, nullptr);

	auto* expression = dynamic_cast<StandardExpression*>(returnStatement->expression.get());
	ASSERT_NE(expression, nullptr);
	auto* literal = std::get_if<Literal>(&(expression->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseBlock_InvalidSyntax)
{
	std::wstringstream input(L"{ return 42; ");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseBlock(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseStatement_ValidConditional)
{
	std::wstringstream input(L"if (true) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto statement = parser.ParseStatement();
	ASSERT_NE(statement, nullptr);

	auto* conditional = dynamic_cast<Conditional*>(statement.get());
	ASSERT_NE(conditional, nullptr);
	ASSERT_NE(conditional->condition, nullptr);
	ASSERT_NE(conditional->ifBlock, nullptr);
	EXPECT_TRUE(conditional->ifBlock->statements.empty());

	auto* condition = dynamic_cast<StandardExpression*>(conditional->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), true);
}

TEST_F(ParserTestNewConvention, ParseStatement_ValidLoop)
{
	std::wstringstream input(L"while (true) {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto statement = parser.ParseStatement();
	ASSERT_NE(statement, nullptr);

	auto* loop = dynamic_cast<WhileLoop*>(statement.get());
	ASSERT_NE(loop, nullptr);
	ASSERT_NE(loop->condition, nullptr);
	ASSERT_NE(loop->block, nullptr);
	EXPECT_TRUE(loop->block->statements.empty());

	auto* condition = dynamic_cast<StandardExpression*>(loop->condition.get());
	ASSERT_NE(condition, nullptr);
	auto* literal = std::get_if<Literal>(&(condition->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<bool>(literal->value), true);
}

TEST_F(ParserTestNewConvention, ParseStatement_ValidReturn)
{
	std::wstringstream input(L"return 42;");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto statement = parser.ParseStatement();
	ASSERT_NE(statement, nullptr);

	auto* returnStatement = dynamic_cast<Return*>(statement.get());
	ASSERT_NE(returnStatement, nullptr);
	ASSERT_NE(returnStatement->expression, nullptr);

	auto* expression = dynamic_cast<StandardExpression*>(returnStatement->expression.get());
	ASSERT_NE(expression, nullptr);
	auto* literal = std::get_if<Literal>(&(expression->conjunctions[0]->relations[0]->firstAdditive->multiplicatives[0]->factors[0]->factor));
	ASSERT_NE(literal, nullptr);
	EXPECT_EQ(std::get<int>(literal->value), 42);
}

TEST_F(ParserTestNewConvention, ParseStatement_InvalidSyntax)
{
	std::wstringstream input(L"if (true) {");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_THROW(parser.ParseStatement(), ParserTest::ParserException);
}

TEST_F(ParserTestNewConvention, ParseProgram_ValidProgram)
{
	std::wstringstream input(L"func foo() {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto program = parser.ParseProgram();
	ASSERT_NE(program, nullptr);
	ASSERT_EQ(program->funDefs.size(), 1);
	EXPECT_EQ(program->funDefs[0]->identifier, L"foo");
	ASSERT_NE(program->funDefs[0]->block, nullptr);
	EXPECT_TRUE(program->funDefs[0]->block->statements.empty());
}

TEST_F(ParserTestNewConvention, ParseProgram_ValidProgramWithMultipleFunctions)
{
	std::wstringstream input(L"func foo() {} func bar() {}");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto program = parser.ParseProgram();
	ASSERT_NE(program, nullptr);
	ASSERT_EQ(program->funDefs.size(), 2);
	EXPECT_EQ(program->funDefs[0]->identifier, L"foo");
	EXPECT_EQ(program->funDefs[1]->identifier, L"bar");
	ASSERT_NE(program->funDefs[0]->block, nullptr);
	ASSERT_NE(program->funDefs[1]->block, nullptr);
	EXPECT_TRUE(program->funDefs[0]->block->statements.empty());
	EXPECT_TRUE(program->funDefs[1]->block->statements.empty());
}

TEST_F(ParserTestNewConvention, GetTokenFromLexer_ValidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto token = parser.GetTokenFromLexer();
	EXPECT_EQ(token.GetType(), LexToken::TokenType::Integer);
	EXPECT_EQ(std::get<int>(token.GetValue()), 42);
}

TEST_F(ParserTestNewConvention, GetTokenFromLexer_CommentToken)
{
	std::wstringstream input(L"# comment\n42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto token = parser.GetTokenFromLexer();
	EXPECT_EQ(token.GetType(), LexToken::TokenType::Integer);
	EXPECT_EQ(std::get<int>(token.GetValue()), 42);
}

TEST_F(ParserTestNewConvention, GetNextToken_ValidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto token = parser.GetNextToken();
	EXPECT_EQ(token.GetType(), LexToken::TokenType::Integer);
	EXPECT_EQ(std::get<int>(token.GetValue()), 42);
}

TEST_F(ParserTestNewConvention, GetNextToken_ReuseLastToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto token1 = parser.GetNextToken();
	parser.lastUnusedToken = token1;
	auto token2 = parser.GetNextToken();
	EXPECT_EQ(token2.GetType(), LexToken::TokenType::Integer);
	EXPECT_EQ(std::get<int>(token2.GetValue()), 42);
}

TEST_F(ParserTestNewConvention, GetExpectedToken_ValidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto token = parser.GetExpectedToken(LexToken::TokenType::Integer);
	ASSERT_TRUE(token.has_value());
	EXPECT_EQ(token->GetType(), LexToken::TokenType::Integer);
	EXPECT_EQ(std::get<int>(token->GetValue()), 42);
}

TEST_F(ParserTestNewConvention, GetExpectedToken_InvalidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	auto token = parser.GetExpectedToken(LexToken::TokenType::String);
	EXPECT_FALSE(token.has_value());
}

TEST_F(ParserTestNewConvention, ConsumeToken_ValidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_TRUE(parser.ConsumeToken(LexToken::TokenType::Integer));
}

TEST_F(ParserTestNewConvention, ConsumeToken_InvalidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_FALSE(parser.ConsumeToken(LexToken::TokenType::String));
}

TEST_F(ParserTestNewConvention, CheckToken_ValidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_TRUE(parser.CheckToken(LexToken::TokenType::Integer));
}

TEST_F(ParserTestNewConvention, CheckToken_InvalidToken)
{
	std::wstringstream input(L"42");
	auto lexer = Lexer(&input);
	ParserTest parser = ParserTest(&lexer);
	EXPECT_FALSE(parser.CheckToken(LexToken::TokenType::String));
}