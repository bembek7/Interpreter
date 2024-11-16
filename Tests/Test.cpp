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

TEST_F(LexerTest, SingleCharOperatorRecognition)
{
	std::wstringstream input(L"= + - * / !");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Assign, L"=", 1, 1},
		{Lexer::TokenType::Plus, L"+", 1, 3},
		{Lexer::TokenType::Minus, L"-", 1, 5},
		{Lexer::TokenType::Asterisk, L"*", 1, 7},
		{Lexer::TokenType::Slash, L"/", 1, 9},
		{Lexer::TokenType::LogicalNot, L"!", 1, 11},
		{Lexer::TokenType::EndOfFile, L"", 1, 12}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, TwoCharOperatorRecognition)
{
	std::wstringstream input(L"&& || == !=");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::LogicalAnd, L"&&", 1, 1},
		{Lexer::TokenType::LogicalOr, L"||", 1, 4},
		{Lexer::TokenType::Equal, L"==", 1, 7},
		{Lexer::TokenType::NotEqual, L"!=", 1, 10},
		{Lexer::TokenType::EndOfFile, L"", 1, 12}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, StringLiteralRecognition)
{
	std::wstringstream input(L"\"Hello, World!\"");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, L"\"Hello, World!\"", 1, 1},
		{Lexer::TokenType::EndOfFile, L"", 1, 18}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, KeywordAndIdentifierRecognition)
{
	std::wstringstream input(L"var myVariable while");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"var", 1, 1},
		{Lexer::TokenType::Identifier, L"myVariable", 1, 5},
		{Lexer::TokenType::Keyword, L"while", 1, 16},
		{Lexer::TokenType::EndOfFile, L"", 1, 21}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, IntegerRecognition)
{
	std::wstringstream input(L"12345");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Integer, 12345, 1, 1},
		{Lexer::TokenType::EndOfFile, L"", 1, 6}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, CommentRecognition)
{
	std::wstringstream input(L"# This is a comment\nvar");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Comment, L"", 1, 1},
		{Lexer::TokenType::Keyword, L"var", 2, 1},
		{Lexer::TokenType::EndOfFile, L"", 2, 4}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, UnrecognizedCharacterRecognition)
{
	std::wstringstream input(L"@");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, L"@", 1, 1},
		{Lexer::TokenType::EndOfFile, L"", 1, 2}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, MultipleTokensIncludingWhitespaceAndOperators)
{
	std::wstringstream input(L"var count = 123 + myVar * 4;");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"var", 1, 1},
		{Lexer::TokenType::Identifier, L"count", 1, 5},
		{Lexer::TokenType::Assign, L"=", 1, 11},
		{Lexer::TokenType::Integer, 123, 1, 13},
		{Lexer::TokenType::Plus, L"+", 1, 17},
		{Lexer::TokenType::Identifier, L"myVar", 1, 19},
		{Lexer::TokenType::Asterisk, L"*", 1, 25},
		{Lexer::TokenType::Integer, 4, 1, 27},
		{Lexer::TokenType::Semicolon, L";", 1, 28},
		{Lexer::TokenType::EndOfFile, L"", 1, 29}
	};

	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, LongStringLiteralWithEscapedCharacters)
{
	std::wstringstream input(L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\"");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\"", 1, 1},
		{Lexer::TokenType::EndOfFile, L"", 1, 65}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, MixedSingleAndMultiCharacterOperators)
{
	std::wstringstream input(L">= <= != && || = !");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::GreaterEqual, L">=", 1, 1},
		{Lexer::TokenType::LessEqual, L"<=", 1, 4},
		{Lexer::TokenType::NotEqual, L"!=", 1, 7},
		{Lexer::TokenType::LogicalAnd, L"&&", 1, 10},
		{Lexer::TokenType::LogicalOr, L"||", 1, 13},
		{Lexer::TokenType::Assign, L"=", 1, 16},
		{Lexer::TokenType::LogicalNot, L"!", 1, 18},
		{Lexer::TokenType::EndOfFile, L"", 1, 19}
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
		{Lexer::TokenType::Comment, L"", 1, 1},
		{Lexer::TokenType::Keyword, L"var", 2, 1},
		{Lexer::TokenType::Identifier, L"x", 2, 5},
		{Lexer::TokenType::PlusAssign, L"+=", 2, 7},
		{Lexer::TokenType::Integer, 10, 2, 10},
		{Lexer::TokenType::Comment, L"", 2, 13},
		{Lexer::TokenType::EndOfFile, L"", 3, 1}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, KeytwordInIdentifier)
{
	std::wstringstream input(L"while123");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Identifier, L"while123", 1, 1},
		{Lexer::TokenType::EndOfFile, L"", 1, 9}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, EdgeCaseMultipleNewlinesAndTabs)
{
	std::wstringstream input(L"\n\n\t\tvar a = 5\nwhile (a < 10) { a += 1; }\n");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"var", 3, 3},
		{Lexer::TokenType::Identifier, L"a", 3, 7},
		{Lexer::TokenType::Assign, L"=", 3, 9},
		{Lexer::TokenType::Integer, 5, 3, 11},
		{Lexer::TokenType::Keyword, L"while", 4, 1},
		{Lexer::TokenType::LParenth, L"(", 4, 7},
		{Lexer::TokenType::Identifier, L"a", 4, 8},
		{Lexer::TokenType::Less, L"<", 4, 10},
		{Lexer::TokenType::Integer, 10, 4, 12},
		{Lexer::TokenType::RParenth, L")", 4, 14},
		{Lexer::TokenType::LBracket, L"{", 4, 16},
		{Lexer::TokenType::Identifier, L"a", 4, 18},
		{Lexer::TokenType::PlusAssign, L"+=", 4, 20},
		{Lexer::TokenType::Integer, 1, 4, 23},
		{Lexer::TokenType::Semicolon, L";", 4, 24},
		{Lexer::TokenType::RBracket, L"}", 4, 26},
		{Lexer::TokenType::EndOfFile, L"", 5, 1}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, VariableAssignmentAndComment)
{
	std::wstringstream input(L"var a = 10;\n\nvar b = a * a;\n\n # b automatically converted to string and printed");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"var", 1, 1},
		{Lexer::TokenType::Identifier, L"a", 1, 5},
		{Lexer::TokenType::Assign, L"=", 1, 7},
		{Lexer::TokenType::Integer, 10, 1, 9},
		{Lexer::TokenType::Semicolon, L";", 1, 11},
		{Lexer::TokenType::Keyword, L"var", 3, 1},
		{Lexer::TokenType::Identifier, L"b", 3, 5},
		{Lexer::TokenType::Assign, L"=", 3, 7},
		{Lexer::TokenType::Identifier, L"a", 3, 9},
		{Lexer::TokenType::Asterisk, L"*", 3, 11},
		{Lexer::TokenType::Identifier, L"a", 3, 13},
		{Lexer::TokenType::Semicolon, L";", 3, 14},
		{Lexer::TokenType::Comment, L"", 5, 2},
		{Lexer::TokenType::EndOfFile, L"", 6, 1}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, IfElseBlock)
{
	std::wstringstream input(L"var b = false;\n\nif(b)\n{\n\t# do sth\n}\nelse\n{\n\t# do sth else\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"var", 1, 1},
		{Lexer::TokenType::Identifier, L"b", 1, 5},
		{Lexer::TokenType::Assign, L"=", 1, 7},
		{Lexer::TokenType::Boolean, false, 1, 9},
		{Lexer::TokenType::Semicolon, L";", 1, 14},
		{Lexer::TokenType::Keyword, L"if", 3, 1},
		{Lexer::TokenType::LParenth, L"(", 3, 3},
		{Lexer::TokenType::Identifier, L"b", 3, 4},
		{Lexer::TokenType::RParenth, L")", 3, 5},
		{Lexer::TokenType::LBracket, L"{", 4, 1},
		{Lexer::TokenType::Comment, L"", 5, 2},
		{Lexer::TokenType::RBracket, L"}", 6, 1},
		{Lexer::TokenType::Keyword, L"else", 7, 1},
		{Lexer::TokenType::LBracket, L"{", 8, 1},
		{Lexer::TokenType::Comment, L"", 9, 2},
		{Lexer::TokenType::RBracket, L"}", 10, 1},
		{Lexer::TokenType::EndOfFile, L"", 10, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, WhileLoop)
{
	std::wstringstream input(L"while(a < 10)\n{\n\t# do stuff\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"while", 1, 1},
		{Lexer::TokenType::LParenth, L"(", 1, 6},
		{Lexer::TokenType::Identifier, L"a", 1, 7},
		{Lexer::TokenType::Less, L"<", 1, 9},
		{Lexer::TokenType::Integer, 10, 1, 11},
		{Lexer::TokenType::RParenth, L")", 1, 13},
		{Lexer::TokenType::LBracket, L"{", 2, 1},
		{Lexer::TokenType::Comment, L"", 3, 2},
		{Lexer::TokenType::RBracket, L"}", 4, 1},
		{Lexer::TokenType::EndOfFile, L"", 4, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, RecursiveFunction)
{
	std::wstringstream input(L"func Fizz(a, b)\n{\n\treturn Fizz(a - 1, b);\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"func", 1, 1},
		{Lexer::TokenType::Identifier, L"Fizz", 1, 6},
		{Lexer::TokenType::LParenth, L"(", 1, 10},
		{Lexer::TokenType::Identifier, L"a", 1, 11},
		{Lexer::TokenType::Comma, L",", 1, 12},
		{Lexer::TokenType::Identifier, L"b", 1, 14},
		{Lexer::TokenType::RParenth, L")", 1, 15},
		{Lexer::TokenType::LBracket, L"{", 2, 1},
		{Lexer::TokenType::Keyword, L"return", 3, 2},
		{Lexer::TokenType::Identifier, L"Fizz", 3, 9},
		{Lexer::TokenType::LParenth, L"(", 3, 13},
		{Lexer::TokenType::Identifier, L"a", 3, 14},
		{Lexer::TokenType::Minus, L"-", 3, 16},
		{Lexer::TokenType::Integer, 1, 3, 18},
		{Lexer::TokenType::Comma, L",", 3, 19},
		{Lexer::TokenType::Identifier, L"b", 3, 21},
		{Lexer::TokenType::RParenth, L")", 3, 22},
		{Lexer::TokenType::Semicolon, L";", 3, 23},
		{Lexer::TokenType::RBracket, L"}", 4, 1},
		{Lexer::TokenType::EndOfFile, L"", 4, 2},
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, FunctionWithReturn)
{
	std::wstringstream input(L"func Add(a, b)\n{\n\treturn a + b;\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"func", 1, 1},
		{Lexer::TokenType::Identifier, L"Add", 1, 6},
		{Lexer::TokenType::LParenth, L"(", 1, 9},
		{Lexer::TokenType::Identifier, L"a", 1, 10},
		{Lexer::TokenType::Comma, L",", 1, 11},
		{Lexer::TokenType::Identifier, L"b", 1, 13},
		{Lexer::TokenType::RParenth, L")", 1, 14},
		{Lexer::TokenType::LBracket, L"{", 2, 1},
		{Lexer::TokenType::Keyword, L"return", 3, 2},
		{Lexer::TokenType::Identifier, L"a", 3, 9},
		{Lexer::TokenType::Plus, L"+", 3, 11},
		{Lexer::TokenType::Identifier, L"b", 3, 13},
		{Lexer::TokenType::Semicolon, L";", 3, 14},
		{Lexer::TokenType::RBracket, L"}", 4, 1},
		{Lexer::TokenType::EndOfFile, L"", 4, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, SimpleMainFunction)
{
	std::wstringstream input(L"func main()\n{\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"func", 1, 1},
		{Lexer::TokenType::Identifier, L"main", 1, 6},
		{Lexer::TokenType::LParenth, L"(", 1, 10},
		{Lexer::TokenType::RParenth, L")", 1, 11},
		{Lexer::TokenType::LBracket, L"{", 2, 1},
		{Lexer::TokenType::RBracket, L"}", 3, 1},
		{Lexer::TokenType::EndOfFile, L"", 3, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, HigherOrderFunctionComposition)
{
	std::wstringstream input(L"func compose(f, g)\n{\n\treturn func(x) { return f(g(x)); }\n}");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Keyword, L"func", 1, 1},
		{Lexer::TokenType::Identifier, L"compose", 1, 6},
		{Lexer::TokenType::LParenth, L"(", 1, 13},
		{Lexer::TokenType::Identifier, L"f", 1, 14},
		{Lexer::TokenType::Comma, L",", 1, 15},
		{Lexer::TokenType::Identifier, L"g", 1, 17},
		{Lexer::TokenType::RParenth, L")", 1, 18},
		{Lexer::TokenType::LBracket, L"{", 2, 1},
		{Lexer::TokenType::Keyword, L"return", 3, 2},
		{Lexer::TokenType::Keyword, L"func", 3, 9},            // Inner "func"
		{Lexer::TokenType::LParenth, L"(", 3, 13},               // "(" for inner "func"
		{Lexer::TokenType::Identifier, L"x", 3, 14},              // "x" in inner "func"
		{Lexer::TokenType::RParenth, L")", 3, 15},               // ")" for inner "func"
		{Lexer::TokenType::LBracket, L"{", 3, 17},               // "{" after "func"
		{Lexer::TokenType::Keyword, L"return", 3, 19},
		{Lexer::TokenType::Identifier, L"f", 3, 26},               // "f" in "f(g(x))"
		{Lexer::TokenType::LParenth, L"(", 3, 27},               // "(" in "f(g(x))"
		{Lexer::TokenType::Identifier, L"g", 3, 28},               // "g" in "f(g(x))"
		{Lexer::TokenType::LParenth, L"(", 3, 29},               // "(" in "f(g(x))"
		{Lexer::TokenType::Identifier, L"x", 3, 30},               // "x" in "f(g(x))"
		{Lexer::TokenType::RParenth, L")", 3, 31},               // ")" in "f(g(x))"
		{Lexer::TokenType::RParenth, L")", 3, 32},               // ")" in "f(g(x))"
		{Lexer::TokenType::Semicolon, L";", 3, 33},               // ";" after "f(g(x))"
		{Lexer::TokenType::RBracket, L"}", 3, 35},                // Closing "}"
		{Lexer::TokenType::RBracket, L"}", 4, 1},                // Closing outer "}"
		{Lexer::TokenType::EndOfFile, L"", 4, 2}
	};
	CompareTokens(tokens, expectedTokens);
}

TEST_F(LexerTest, RecognizesMultipleFloatsAndIntegers) {
	std::wstringstream input(L"3.14 2.718 42 0.5");
	const auto& tokens = lexer.Tokenize(input).first;

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Float, 3.14f, 1, 1},
		{Lexer::TokenType::Float, 2.718f, 1, 6},
		{Lexer::TokenType::Integer, 42, 1, 12},
		{Lexer::TokenType::Float, 0.5f, 1, 15},
		{Lexer::TokenType::EndOfFile, L"", 1, 18}
	};

	CompareTokens(tokens, expectedTokens);
}