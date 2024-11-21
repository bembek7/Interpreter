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
		EXPECT_EQ(tokens[i].position.line, expectedTokens[i].position.line);
		EXPECT_EQ(tokens[i].position.column, expectedTokens[i].position.column);
	}
}

static void CompareErrors(const std::vector<Lexer::LexicalError>& errors, const std::vector<Lexer::LexicalError>& expectedErrors)
{
	ASSERT_EQ(errors.size(), expectedErrors.size());

	for (size_t i = 0; i < errors.size(); ++i)
	{
		EXPECT_EQ(errors[i].type, expectedErrors[i].type);
		EXPECT_EQ(errors[i].message, expectedErrors[i].message);
		EXPECT_EQ(errors[i].position.line, expectedErrors[i].position.line);
		EXPECT_EQ(errors[i].position.column, expectedErrors[i].position.column);
		EXPECT_EQ(errors[i].terminating, expectedErrors[i].terminating);
	}
}

TEST_F(LexerTest, SingleCharOperatorRecognition)
{
	std::wstringstream input(L"= + - * / !");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Assign, Lexer::Position(1, 1)},
		{Lexer::TokenType::Plus, Lexer::Position(1, 3)},
		{Lexer::TokenType::Minus, Lexer::Position(1, 5)},
		{Lexer::TokenType::Asterisk, Lexer::Position(1, 7)},
		{Lexer::TokenType::Slash, Lexer::Position(1, 9)},
		{Lexer::TokenType::LogicalNot, Lexer::Position(1, 11)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 12)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, TwoCharOperatorRecognition)
{
	std::wstringstream input(L"&& || == !=");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::LogicalAnd, Lexer::Position(1, 1)},
		{Lexer::TokenType::LogicalOr, Lexer::Position(1, 4)},
		{Lexer::TokenType::Equal, Lexer::Position(1, 7)},
		{Lexer::TokenType::NotEqual, Lexer::Position(1, 10)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 12)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralRecognition)
{
	std::wstringstream input(L"\"Hello, World!\"");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, Lexer::Position(1, 1), L"\"Hello, World!\""},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 18)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, KeywordAndIdentifierRecognition)
{
	std::wstringstream input(L"var myVariable while");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 5), L"myVariable"},
		{Lexer::TokenType::While, Lexer::Position(1, 16)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 21)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IntegerRecognition)
{
	std::wstringstream input(L"12345");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Integer, Lexer::Position(1, 1), 12345},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 6)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, CommentRecognition)
{
	std::wstringstream input(L"# This is a comment\nvar");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Comment, Lexer::Position(1, 1)},
		{Lexer::TokenType::Var, Lexer::Position(2, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(2, 4)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, UnrecognizedCharacterRecognition)
{
	std::wstringstream input(L"@");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1), L"@"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::UnrecognizedSymbol, Lexer::Position(1, 1)},
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, MultipleTokensIncludingWhitespaceAndOperators)
{
	std::wstringstream input(L"var count = 123 + myVar * 4;");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 5), L"count"},
		{Lexer::TokenType::Assign, Lexer::Position(1, 11)},
		{Lexer::TokenType::Integer, Lexer::Position(1, 13), 123},
		{Lexer::TokenType::Plus, Lexer::Position(1, 17)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 19), L"myVar"},
		{Lexer::TokenType::Asterisk, Lexer::Position(1, 25)},
		{Lexer::TokenType::Integer, Lexer::Position(1, 27), 4},
		{Lexer::TokenType::Semicolon, Lexer::Position(1, 28)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 29)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, LongStringLiteralWithEscapedCharacters)
{
	std::wstringstream input(L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\"");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, Lexer::Position(1, 1), L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\""},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 65)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, MixedSingleAndMultiCharacterOperators)
{
	std::wstringstream input(L">= <= != && || = !");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::GreaterEqual, Lexer::Position(1, 1)},
		{Lexer::TokenType::LessEqual, Lexer::Position(1, 4)},
		{Lexer::TokenType::NotEqual, Lexer::Position(1, 7)},
		{Lexer::TokenType::LogicalAnd, Lexer::Position(1, 10)},
		{Lexer::TokenType::LogicalOr, Lexer::Position(1, 13)},
		{Lexer::TokenType::Assign, Lexer::Position(1, 16)},
		{Lexer::TokenType::LogicalNot, Lexer::Position(1, 18)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 19)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, MalformedTokens)
{
	std::wstringstream input(L"00123 var$ %abc");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1), L"00123"},
		{Lexer::TokenType::Var, Lexer::Position(1, 7)},
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 10), L"$"},
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 12), L"%"},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 13), L"abc"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 16)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::InvalidNumber, Lexer::Position(1, 1)},
		{Lexer::ErrorType::UnrecognizedSymbol, Lexer::Position(1, 10)},
		{Lexer::ErrorType::UnrecognizedSymbol, Lexer::Position(1, 12)},
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, NestedCommentsAndOperators)
{
	std::wstringstream input(L"# This is a # comment\nvar x += 10 # Another comment");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Comment, Lexer::Position(1, 1)},
		{Lexer::TokenType::Var, Lexer::Position(2, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(2, 5), L"x"},
		{Lexer::TokenType::PlusAssign, Lexer::Position(2, 7)},
		{Lexer::TokenType::Integer, Lexer::Position(2, 10), 10},
		{Lexer::TokenType::Comment, Lexer::Position(2, 13)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(3, 1)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, KeytwordInIdentifier)
{
	std::wstringstream input(L"while123");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Identifier, Lexer::Position(1, 1), L"while123"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 9)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, EdgeCaseMultipleNewlinesAndTabs)
{
	std::wstringstream input(L"\n\n\t\tvar a = 5\nwhile (a < 10) { a += 1; }\n");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, Lexer::Position(3, 3)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 7), L"a"},
		{Lexer::TokenType::Assign, Lexer::Position(3, 9)},
		{Lexer::TokenType::Integer, Lexer::Position(3, 11), 5},
		{Lexer::TokenType::While, Lexer::Position(4, 1)},
		{Lexer::TokenType::LParenth, Lexer::Position(4, 7)},
		{Lexer::TokenType::Identifier, Lexer::Position(4, 8), L"a"},
		{Lexer::TokenType::Less, Lexer::Position(4, 10)},
		{Lexer::TokenType::Integer, Lexer::Position(4, 12), 10},
		{Lexer::TokenType::RParenth, Lexer::Position(4, 14)},
		{Lexer::TokenType::LBracket, Lexer::Position(4, 16)},
		{Lexer::TokenType::Identifier, Lexer::Position(4, 18), L"a"},
		{Lexer::TokenType::PlusAssign, Lexer::Position(4, 20)},
		{Lexer::TokenType::Integer, Lexer::Position(4, 23), 1},
		{Lexer::TokenType::Semicolon, Lexer::Position(4, 24)},
		{Lexer::TokenType::RBracket, Lexer::Position(4, 26)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(5, 1)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, VariableAssignmentAndComment)
{
	std::wstringstream input(L"var a = 10;\n\nvar b = a * a;\n\n # b automatically converted to string and printed");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 5), L"a"},
		{Lexer::TokenType::Assign, Lexer::Position(1, 7)},
		{Lexer::TokenType::Integer, Lexer::Position(1, 9), 10},
		{Lexer::TokenType::Semicolon, Lexer::Position(1, 11)},
		{Lexer::TokenType::Var, Lexer::Position(3, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 5), L"b"},
		{Lexer::TokenType::Assign, Lexer::Position(3, 7)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 9), L"a"},
		{Lexer::TokenType::Asterisk, Lexer::Position(3, 11)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 13), L"a"},
		{Lexer::TokenType::Semicolon, Lexer::Position(3, 14)},
		{Lexer::TokenType::Comment, Lexer::Position(5, 2)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(6, 1)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IfElseBlock)
{
	std::wstringstream input(L"var b = false;\n\nif(b)\n{\n\t# do sth\n}\nelse\n{\n\t# do sth else\n}");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Var, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 5), L"b"},
		{Lexer::TokenType::Assign, Lexer::Position(1, 7)},
		{Lexer::TokenType::Boolean, Lexer::Position(1, 9), false},
		{Lexer::TokenType::Semicolon, Lexer::Position(1, 14)},
		{Lexer::TokenType::If, Lexer::Position(3, 1)},
		{Lexer::TokenType::LParenth, Lexer::Position(3, 3)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 4), L"b"},
		{Lexer::TokenType::RParenth, Lexer::Position(3, 5)},
		{Lexer::TokenType::LBracket, Lexer::Position(4, 1)},
		{Lexer::TokenType::Comment, Lexer::Position(5, 2)},
		{Lexer::TokenType::RBracket, Lexer::Position(6, 1)},
		{Lexer::TokenType::Else, Lexer::Position(7, 1)},
		{Lexer::TokenType::LBracket, Lexer::Position(8, 1)},
		{Lexer::TokenType::Comment, Lexer::Position(9, 2)},
		{Lexer::TokenType::RBracket, Lexer::Position(10, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(10, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, WhileLoop)
{
	std::wstringstream input(L"while(a < 10)\n{\n\t# do stuff\n}");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::While, Lexer::Position(1, 1)},
		{Lexer::TokenType::LParenth, Lexer::Position(1, 6)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 7), L"a"},
		{Lexer::TokenType::Less, Lexer::Position(1, 9)},
		{Lexer::TokenType::Integer, Lexer::Position(1, 11), 10},
		{Lexer::TokenType::RParenth, Lexer::Position(1, 13)},
		{Lexer::TokenType::LBracket, Lexer::Position(2, 1)},
		{Lexer::TokenType::Comment, Lexer::Position(3, 2)},
		{Lexer::TokenType::RBracket, Lexer::Position(4, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(4, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, RecursiveFunction)
{
	std::wstringstream input(L"func Fizz(a, b)\n{\n\treturn Fizz(a - 1, b);\n}");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 6), L"Fizz"},
		{Lexer::TokenType::LParenth, Lexer::Position(1, 10)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 11), L"a"},
		{Lexer::TokenType::Comma, Lexer::Position(1, 12)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 14), L"b"},
		{Lexer::TokenType::RParenth, Lexer::Position(1, 15)},
		{Lexer::TokenType::LBracket, Lexer::Position(2, 1)},
		{Lexer::TokenType::Return, Lexer::Position(3, 2)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 9), L"Fizz"},
		{Lexer::TokenType::LParenth, Lexer::Position(3, 13)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 14), L"a"},
		{Lexer::TokenType::Minus, Lexer::Position(3, 16)},
		{Lexer::TokenType::Integer, Lexer::Position(3, 18), 1},
		{Lexer::TokenType::Comma, Lexer::Position(3, 19)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 21), L"b"},
		{Lexer::TokenType::RParenth, Lexer::Position(3, 22)},
		{Lexer::TokenType::Semicolon, Lexer::Position(3, 23)},
		{Lexer::TokenType::RBracket, Lexer::Position(4, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(4, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, FunctionWithReturn)
{
	std::wstringstream input(L"func Add(a, b)\n{\n\treturn a + b;\n}");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 6), L"Add"},
		{Lexer::TokenType::LParenth, Lexer::Position(1, 9)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 10), L"a"},
		{Lexer::TokenType::Comma, Lexer::Position(1, 11)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 13), L"b"},
		{Lexer::TokenType::RParenth, Lexer::Position(1, 14)},
		{Lexer::TokenType::LBracket, Lexer::Position(2, 1)},
		{Lexer::TokenType::Return, Lexer::Position(3, 2)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 9), L"a"},
		{Lexer::TokenType::Plus, Lexer::Position(3, 11)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 13), L"b"},
		{Lexer::TokenType::Semicolon, Lexer::Position(3, 14)},
		{Lexer::TokenType::RBracket, Lexer::Position(4, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(4, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, SimpleMainFunction)
{
	std::wstringstream input(L"func main()\n{\n}");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 6), L"main"},
		{Lexer::TokenType::LParenth, Lexer::Position(1, 10)},
		{Lexer::TokenType::RParenth, Lexer::Position(1, 11)},
		{Lexer::TokenType::LBracket, Lexer::Position(2, 1)},
		{Lexer::TokenType::RBracket, Lexer::Position(3, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(3, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, HigherOrderFunctionComposition)
{
	std::wstringstream input(L"func compose(f, g)\n{\n\treturn func(x) { return f(g(x)); }\n}");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Func, Lexer::Position(1, 1)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 6), L"compose"},
		{Lexer::TokenType::LParenth, Lexer::Position(1, 13)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 14), L"f"},
		{Lexer::TokenType::Comma, Lexer::Position(1, 15)},
		{Lexer::TokenType::Identifier, Lexer::Position(1, 17), L"g"},
		{Lexer::TokenType::RParenth, Lexer::Position(1, 18)},
		{Lexer::TokenType::LBracket, Lexer::Position(2, 1)},
		{Lexer::TokenType::Return, Lexer::Position(3, 2)},
		{Lexer::TokenType::Func, Lexer::Position(3, 9)},
		{Lexer::TokenType::LParenth, Lexer::Position(3, 13)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 14), L"x"},
		{Lexer::TokenType::RParenth, Lexer::Position(3, 15)},
		{Lexer::TokenType::LBracket, Lexer::Position(3, 17)},
		{Lexer::TokenType::Return, Lexer::Position(3, 19)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 26), L"f"},
		{Lexer::TokenType::LParenth, Lexer::Position(3, 27)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 28), L"g"},
		{Lexer::TokenType::LParenth, Lexer::Position(3, 29)},
		{Lexer::TokenType::Identifier, Lexer::Position(3, 30), L"x"},
		{Lexer::TokenType::RParenth, Lexer::Position(3, 31)},
		{Lexer::TokenType::RParenth, Lexer::Position(3, 32)},
		{Lexer::TokenType::Semicolon, Lexer::Position(3, 33)},
		{Lexer::TokenType::RBracket, Lexer::Position(3, 35)},
		{Lexer::TokenType::RBracket, Lexer::Position(4, 1)},
		{Lexer::TokenType::EndOfFile, Lexer::Position(4, 2)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, RecognizesMultipleFloatsAndIntegers) {
	std::wstringstream input(L"3.14 2.718 42 0.5");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Float, Lexer::Position(1, 1), 3.14f},
		{Lexer::TokenType::Float, Lexer::Position(1, 6), 2.718f},
		{Lexer::TokenType::Integer, Lexer::Position(1, 12), 42},
		{Lexer::TokenType::Float, Lexer::Position(1, 15), 0.5f},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 18)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IntegerOverflow)
{
	std::wstringstream input(L"2147483648");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1), L"2147483648"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 11)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::IntegerOverflow, Lexer::Position(1, 1)}
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
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1), L"99999999999999999999999999999999999999999.1"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 44)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::FloatOverflow, Lexer::Position(1, 1)}
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
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1), L"00042"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 6)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::InvalidNumber, Lexer::Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IncompleteStringLiteral)
{
	std::wstringstream input(L"\"Incomplete string");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1), L"\"Incomplete string"},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 20)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::IncompleteStringLiteral, Lexer::Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, CommentTooLong)
{
	std::wstring inputStr(550, '.');

	std::wstringstream input(L"#This is too long comment" + inputStr);
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Comment, Lexer::Position(1, 1)},
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::CommentTooLong, Lexer::Position(1, 1), true}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralWithValidEscapes)
{
	std::wstringstream input(L"\"Line1\\nLine2\\tTabbed\\\"Quote\\\"\"");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, Lexer::Position(1, 1), L"\"Line1\\nLine2\\tTabbed\\\"Quote\\\"\""},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 34)}
	};

	std::vector<Lexer::LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralTooLong)
{
	std::wstring longString(1000, L'a');
	std::wstringstream input(L"\"" + longString + L"\"");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 1)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::StringLiteralTooLong, Lexer::Position(1, 1), true}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralInvalidEscapeSequence)
{
	std::wstringstream input(L"\"Invalid\\xEscape\"");
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, Lexer::Position(1, 1), L"\"Invalid\\xEscape\""},
		{Lexer::TokenType::EndOfFile, Lexer::Position(1, 20)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::InvalidEscapeSequence, Lexer::Position(1, 11)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralsComplexScenarios)
{
	std::wstringstream input(
		L"\"Valid string\" \"Too long string" + std::wstring(1000 + 1, L'a') +
		L"\" \"Unclosed string \"Invalid\\xEscape\""
	);
	const auto& lexerOut = lexer.Tokenize(input);

	std::vector<Lexer::Token> expectedTokens =
	{
		{Lexer::TokenType::String, Lexer::Position(1, 1), L"\"Valid string\""},
		{Lexer::TokenType::Unrecognized, Lexer::Position(1, 18)}
	};

	std::vector<Lexer::LexicalError> expectedErrors =
	{
		{Lexer::ErrorType::StringLiteralTooLong, Lexer::Position(1, 18), true},
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}