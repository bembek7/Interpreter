#include <gtest/gtest.h>

#include <gtest/gtest.h>
#include <sstream>
#include "Lexer.h"

class LexerTest : public ::testing::Test
{
protected:
	Lexer lexer;
};

static void CompareTokens(const std::vector<Lexer::Token>& tokens, const std::vector<Lexer::Token>& expectedTokens)
{
	ASSERT_EQ(tokens.size(), expectedTokens.size());

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		EXPECT_EQ(tokens[i].type, expectedTokens[i].type);
		EXPECT_EQ(tokens[i].value, expectedTokens[i].value);
		EXPECT_EQ(tokens[i].line, expectedTokens[i].line);
		EXPECT_EQ(tokens[i].column, expectedTokens[i].column);
	}
}

static void CompareErrors(const std::vector<Lexer::LexicalError>& errors, const std::vector<Lexer::LexicalError>& expectedErrors)
{
	ASSERT_EQ(errors.size(), expectedErrors.size());

	for (size_t i = 0; i < errors.size(); ++i)
	{
		EXPECT_EQ(errors[i].type, expectedErrors[i].type);
		EXPECT_EQ(errors[i].message, expectedErrors[i].message);
		EXPECT_EQ(errors[i].line, expectedErrors[i].line);
		EXPECT_EQ(errors[i].column, expectedErrors[i].column);
		EXPECT_EQ(errors[i].terminating, expectedErrors[i].terminating);
	}
}

TEST_F(LexerTest, SingleCharOperatorRecognition)
{
	std::wstringstream input(L"= + - * / !");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Assign, 1, 1},
		{Lexer::TokenType::Plus, 1, 3},
		{Lexer::TokenType::Minus, 1, 5},
		{Lexer::TokenType::Asterisk, 1, 7},
		{Lexer::TokenType::Slash, 1, 9},
		{Lexer::TokenType::LogicalNot, 1, 11},
		{Lexer::TokenType::EndOfFile, 1, 12}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, TwoCharOperatorRecognition)
{
	std::wstringstream input(L"&& || == !=");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::LogicalAnd, 1, 1},
		{Lexer::TokenType::LogicalOr, 1, 4},
		{Lexer::TokenType::Equal, 1, 7},
		{Lexer::TokenType::NotEqual, 1, 10},
		{Lexer::TokenType::EndOfFile, 1, 12}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, StringLiteralRecognition)
{
	std::wstringstream input(L"\"Hello, World!\"");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, 1, 1, L"\"Hello, World!\""},
		{Lexer::TokenType::EndOfFile, 1, 18}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, KeywordAndIdentifierRecognition)
{
	std::wstringstream input(L"var myVariable while");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, 1, 1},
		{Lexer::TokenType::Identifier, 1, 5, L"myVariable"},
		{Lexer::TokenType::While, 1, 16},
		{Lexer::TokenType::EndOfFile, 1, 21}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, IntegerRecognition)
{
	std::wstringstream input(L"12345");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Integer, 1, 1, 12345},
		{Lexer::TokenType::EndOfFile, 1, 6}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, CommentRecognition)
{
	std::wstringstream input(L"# This is a comment\nvar");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Comment, 1, 1},
		{Lexer::TokenType::Var, 2, 1},
		{Lexer::TokenType::EndOfFile, 2, 4}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, UnrecognizedCharacterRecognition)
{
	std::wstringstream input(L"@");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, 1, 1, L"@"},
		{Lexer::TokenType::EndOfFile, 1, 2}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, MultipleTokensIncludingWhitespaceAndOperators)
{
	std::wstringstream input(L"var count = 123 + myVar * 4;");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, 1, 1},
		{Lexer::TokenType::Identifier, 1, 5, L"count"},
		{Lexer::TokenType::Assign, 1, 11},
		{Lexer::TokenType::Integer, 1, 13, 123},
		{Lexer::TokenType::Plus, 1, 17},
		{Lexer::TokenType::Identifier, 1, 19, L"myVar"},
		{Lexer::TokenType::Asterisk, 1, 25},
		{Lexer::TokenType::Integer, 1, 27, 4},
		{Lexer::TokenType::Semicolon, 1, 28},
		{Lexer::TokenType::EndOfFile, 1, 29}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, LongStringLiteralWithEscapedCharacters)
{
	std::wstringstream input(L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\"");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, 1, 1, L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\""},
		{Lexer::TokenType::EndOfFile, 1, 65}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, MixedSingleAndMultiCharacterOperators)
{
	std::wstringstream input(L">= <= != && || = !");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::GreaterEqual, 1, 1},
		{Lexer::TokenType::LessEqual, 1, 4},
		{Lexer::TokenType::NotEqual, 1, 7},
		{Lexer::TokenType::LogicalAnd, 1, 10},
		{Lexer::TokenType::LogicalOr, 1, 13},
		{Lexer::TokenType::Assign, 1, 16},
		{Lexer::TokenType::LogicalNot, 1, 18},
		{Lexer::TokenType::EndOfFile, 1, 19}
	};
	CompareTokens(tokens, expectedTokens);
}

/*
TEST_F(LexerTest, MalformedTokens)
{
	std::wstringstream input(L"00123 var$ %abc");
	const auto& tokens = lexer.Tokenize(input).first;
	// some error
	std::vector<Lexer::Token> expectedTokens = {
		{Lexer::TokenType::Unrecognized, L"00123", 1, 1},
		{Lexer::TokenType::Keyword, L"var", 1, 7},
		{Lexer::TokenType::Unrecognized, L"$", 1, 10},
		{Lexer::TokenType::Unrecognized, L"%", 1, 12},
		{Lexer::TokenType::Identifier, L"abc", 1, 13},
		{Lexer::TokenType::EndOfFile, L"", 1, 16}
	};
	CompareTokens(tokens, expectedTokens);
}*/

TEST_F(LexerTest, NestedCommentsAndOperators)
{
	std::wstringstream input(L"# This is a # comment\nvar x += 10 # Another comment");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Comment, 1, 1},
		{Lexer::TokenType::Var, 2, 1},
		{Lexer::TokenType::Identifier, 2, 5, L"x"},
		{Lexer::TokenType::PlusAssign, 2, 7},
		{Lexer::TokenType::Integer, 2, 10, 10},
		{Lexer::TokenType::Comment, 2, 13},
		{Lexer::TokenType::EndOfFile, 3, 1}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, KeytwordInIdentifier)
{
	std::wstringstream input(L"while123");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Identifier, 1, 1, L"while123"},
		{Lexer::TokenType::EndOfFile, 1, 9}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, EdgeCaseMultipleNewlinesAndTabs)
{
	std::wstringstream input(L"\n\n\t\tvar a = 5\nwhile (a < 10) { a += 1; }\n");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, 3, 3},
		{Lexer::TokenType::Identifier, 3, 7, L"a"},
		{Lexer::TokenType::Assign, 3, 9},
		{Lexer::TokenType::Integer, 3, 11, 5},
		{Lexer::TokenType::While, 4, 1},
		{Lexer::TokenType::LParenth, 4, 7},
		{Lexer::TokenType::Identifier, 4, 8, L"a"},
		{Lexer::TokenType::Less, 4, 10},
		{Lexer::TokenType::Integer, 4, 12, 10},
		{Lexer::TokenType::RParenth, 4, 14},
		{Lexer::TokenType::LBracket, 4, 16},
		{Lexer::TokenType::Identifier, 4, 18, L"a"},
		{Lexer::TokenType::PlusAssign, 4, 20},
		{Lexer::TokenType::Integer, 4, 23, 1},
		{Lexer::TokenType::Semicolon, 4, 24},
		{Lexer::TokenType::RBracket, 4, 26},
		{Lexer::TokenType::EndOfFile, 5, 1}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, VariableAssignmentAndComment)
{
	std::wstringstream input(L"var a = 10;\n\nvar b = a * a;\n\n # b automatically converted to string and printed");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, 1, 1},
		{Lexer::TokenType::Identifier, 1, 5, L"a"},
		{Lexer::TokenType::Assign, 1, 7},
		{Lexer::TokenType::Integer, 1, 9, 10},
		{Lexer::TokenType::Semicolon, 1, 11},
		{Lexer::TokenType::Var, 3, 1},
		{Lexer::TokenType::Identifier, 3, 5, L"b"},
		{Lexer::TokenType::Assign, 3, 7},
		{Lexer::TokenType::Identifier, 3, 9, L"a"},
		{Lexer::TokenType::Asterisk, 3, 11},
		{Lexer::TokenType::Identifier, 3, 13, L"a"},
		{Lexer::TokenType::Semicolon, 3, 14},
		{Lexer::TokenType::Comment, 5, 2},
		{Lexer::TokenType::EndOfFile, 6, 1}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, IfElseBlock)
{
	std::wstringstream input(L"var b = false;\n\nif(b)\n{\n\t# do sth\n}\nelse\n{\n\t# do sth else\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, 1, 1},
		{Lexer::TokenType::Identifier, 1, 5, L"b"},
		{Lexer::TokenType::Assign, 1, 7},
		{Lexer::TokenType::Boolean, 1, 9, false},
		{Lexer::TokenType::Semicolon, 1, 14},
		{Lexer::TokenType::If, 3, 1},
		{Lexer::TokenType::LParenth, 3, 3},
		{Lexer::TokenType::Identifier, 3, 4, L"b"},
		{Lexer::TokenType::RParenth, 3, 5},
		{Lexer::TokenType::LBracket, 4, 1},
		{Lexer::TokenType::Comment, 5, 2},
		{Lexer::TokenType::RBracket, 6, 1},
		{Lexer::TokenType::Else, 7, 1},
		{Lexer::TokenType::LBracket, 8, 1},
		{Lexer::TokenType::Comment, 9, 2},
		{Lexer::TokenType::RBracket, 10, 1},
		{Lexer::TokenType::EndOfFile, 10, 2}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, WhileLoop)
{
	std::wstringstream input(L"while(a < 10)\n{\n\t# do stuff\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::While, 1, 1},
		{Lexer::TokenType::LParenth, 1, 6},
		{Lexer::TokenType::Identifier, 1, 7, L"a"},
		{Lexer::TokenType::Less, 1, 9},
		{Lexer::TokenType::Integer, 1, 11, 10},
		{Lexer::TokenType::RParenth, 1, 13},
		{Lexer::TokenType::LBracket, 2, 1},
		{Lexer::TokenType::Comment, 3, 2},
		{Lexer::TokenType::RBracket, 4, 1},
		{Lexer::TokenType::EndOfFile, 4, 2}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, RecursiveFunction)
{
	std::wstringstream input(L"func Fizz(a, b)\n{\n\treturn Fizz(a - 1, b);\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, 1, 1},
		{Lexer::TokenType::Identifier, 1, 6, L"Fizz"},
		{Lexer::TokenType::LParenth, 1, 10},
		{Lexer::TokenType::Identifier, 1, 11, L"a"},
		{Lexer::TokenType::Comma, 1, 12},
		{Lexer::TokenType::Identifier, 1, 14, L"b"},
		{Lexer::TokenType::RParenth, 1, 15},
		{Lexer::TokenType::LBracket, 2, 1},
		{Lexer::TokenType::Return, 3, 2},
		{Lexer::TokenType::Identifier, 3, 9, L"Fizz"},
		{Lexer::TokenType::LParenth, 3, 13},
		{Lexer::TokenType::Identifier, 3, 14, L"a"},
		{Lexer::TokenType::Minus, 3, 16},
		{Lexer::TokenType::Integer, 3, 18, 1},
		{Lexer::TokenType::Comma, 3, 19},
		{Lexer::TokenType::Identifier, 3, 21, L"b"},
		{Lexer::TokenType::RParenth, 3, 22},
		{Lexer::TokenType::Semicolon, 3, 23},
		{Lexer::TokenType::RBracket, 4, 1},
		{Lexer::TokenType::EndOfFile, 4, 2}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, FunctionWithReturn)
{
	std::wstringstream input(L"func Add(a, b)\n{\n\treturn a + b;\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, 1, 1},
		{Lexer::TokenType::Identifier, 1, 6, L"Add"},
		{Lexer::TokenType::LParenth, 1, 9},
		{Lexer::TokenType::Identifier, 1, 10, L"a"},
		{Lexer::TokenType::Comma, 1, 11},
		{Lexer::TokenType::Identifier, 1, 13, L"b"},
		{Lexer::TokenType::RParenth, 1, 14},
		{Lexer::TokenType::LBracket, 2, 1},
		{Lexer::TokenType::Return, 3, 2},
		{Lexer::TokenType::Identifier, 3, 9, L"a"},
		{Lexer::TokenType::Plus, 3, 11},
		{Lexer::TokenType::Identifier, 3, 13, L"b"},
		{Lexer::TokenType::Semicolon, 3, 14},
		{Lexer::TokenType::RBracket, 4, 1},
		{Lexer::TokenType::EndOfFile, 4, 2}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, SimpleMainFunction)
{
	std::wstringstream input(L"func main()\n{\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, 1, 1},
		{Lexer::TokenType::Identifier, 1, 6, L"main"},
		{Lexer::TokenType::LParenth, 1, 10},
		{Lexer::TokenType::RParenth, 1, 11},
		{Lexer::TokenType::LBracket, 2, 1},
		{Lexer::TokenType::RBracket, 3, 1},
		{Lexer::TokenType::EndOfFile, 3, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, HigherOrderFunctionComposition)
{
	std::wstringstream input(L"func compose(f, g)\n{\n\treturn func(x) { return f(g(x)); }\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, 1, 1},
		{Lexer::TokenType::Identifier, 1, 6, L"compose"},
		{Lexer::TokenType::LParenth, 1, 13},
		{Lexer::TokenType::Identifier, 1, 14, L"f"},
		{Lexer::TokenType::Comma, 1, 15},
		{Lexer::TokenType::Identifier, 1, 17, L"g"},
		{Lexer::TokenType::RParenth, 1, 18},
		{Lexer::TokenType::LBracket, 2, 1},
		{Lexer::TokenType::Return, 3, 2},
		{Lexer::TokenType::Func, 3, 9},
		{Lexer::TokenType::LParenth, 3, 13},
		{Lexer::TokenType::Identifier, 3, 14, L"x"},
		{Lexer::TokenType::RParenth, 3, 15},
		{Lexer::TokenType::LBracket, 3, 17},
		{Lexer::TokenType::Return, 3, 19},
		{Lexer::TokenType::Identifier, 3, 26, L"f"},
		{Lexer::TokenType::LParenth, 3, 27},
		{Lexer::TokenType::Identifier, 3, 28, L"g"},
		{Lexer::TokenType::LParenth, 3, 29},
		{Lexer::TokenType::Identifier, 3, 30, L"x"},
		{Lexer::TokenType::RParenth, 3, 31},
		{Lexer::TokenType::RParenth, 3, 32},
		{Lexer::TokenType::Semicolon, 3, 33},
		{Lexer::TokenType::RBracket, 3, 35},
		{Lexer::TokenType::RBracket, 4, 1},
		{Lexer::TokenType::EndOfFile, 4, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, RecognizesMultipleFloatsAndIntegers) {
	std::wstringstream input(L"3.14 2.718 42 0.5");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Float, 1, 1, 3.14f},
		{Lexer::TokenType::Float, 1, 6, 2.718f},
		{Lexer::TokenType::Integer, 1, 12, 42},
		{Lexer::TokenType::Float, 1, 15, 0.5f},
		{Lexer::TokenType::EndOfFile, 1, 18}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, IntegerOverflow)
{
	std::wstringstream input(L"2147483648");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, 1, 1, L"2147483648"},
		{Lexer::TokenType::EndOfFile, 1, 11}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::IntegerOverflow, "Number would fall out of the range of the integer", 1, 1}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, FloatOverflow)
{
	std::wstringstream input(L"99999999999999999999999999999999999999999.1");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, 1, 1, L"99999999999999999999999999999999999999999.1"},
		{Lexer::TokenType::EndOfFile, 1, 44}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::FloatOverflow, "Number would fall out of the range of the float", 1, 1}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, LeadingZerosError)
{
	std::wstringstream input(L"00042");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, 1, 1, L"00042"},
		{Lexer::TokenType::EndOfFile, 1, 6}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::InvalidNumber, "Invalid number format - leading zeros.", 1, 1}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}