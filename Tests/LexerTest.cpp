#include <gtest/gtest.h>

#include <sstream>
#include "Lexer.h"

class LexerTest : public ::testing::Test
{
protected:
	//Lexer lexer;
};

static void CompareTokens(const std::vector<LexToken>& tokens, const std::vector<LexToken>& expectedTokens)
{
	ASSERT_EQ(tokens.size(), expectedTokens.size());

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		EXPECT_EQ(tokens[i].GetType(), expectedTokens[i].GetType());
		EXPECT_EQ(tokens[i].GetPosition().line, expectedTokens[i].GetPosition().line);
		EXPECT_EQ(tokens[i].GetPosition().column, expectedTokens[i].GetPosition().column);

		if (tokens[i].GetValue().index() == expectedTokens[i].GetValue().index())
		{
			if (std::holds_alternative<float>(tokens[i].GetValue()))
			{
				float actual = std::get<float>(tokens[i].GetValue());
				float expected = std::get<float>(expectedTokens[i].GetValue());
				EXPECT_NEAR(actual, expected, 0.01f);
			}
			else
			{
				EXPECT_EQ(tokens[i].GetValue(), expectedTokens[i].GetValue());
			}
		}
		else
		{
			FAIL() << "Mismatched variant types in token values.";
		}
	}
}

static void CompareErrors(const std::vector<LexicalError>& errors, const std::vector<LexicalError>& expectedErrors)
{
	ASSERT_EQ(errors.size(), expectedErrors.size());

	for (size_t i = 0; i < errors.size(); ++i)
	{
		EXPECT_EQ(errors[i].GetType(), expectedErrors[i].GetType());
		EXPECT_EQ(errors[i].GetMessage(), expectedErrors[i].GetMessage());
		EXPECT_EQ(errors[i].GetPosition().line, expectedErrors[i].GetPosition().line);
		EXPECT_EQ(errors[i].GetPosition().column, expectedErrors[i].GetPosition().column);
		EXPECT_EQ(errors[i].IsTerminating(), expectedErrors[i].IsTerminating());
	}
}

TEST_F(LexerTest, SingleCharOperatorRecognition)
{
	std::wstringstream input(L"= + - * / !");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Assign, Position(1, 1)},
		{LexToken::TokenType::Plus, Position(1, 3)},
		{LexToken::TokenType::Minus, Position(1, 5)},
		{LexToken::TokenType::Asterisk, Position(1, 7)},
		{LexToken::TokenType::Slash, Position(1, 9)},
		{LexToken::TokenType::LogicalNot, Position(1, 11)},
		{LexToken::TokenType::EndOfFile, Position(1, 12)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, TwoCharOperatorRecognition)
{
	std::wstringstream input(L"&& || == !=");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::LogicalAnd, Position(1, 1)},
		{LexToken::TokenType::LogicalOr, Position(1, 4)},
		{LexToken::TokenType::Equal, Position(1, 7)},
		{LexToken::TokenType::NotEqual, Position(1, 10)},
		{LexToken::TokenType::EndOfFile, Position(1, 12)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralRecognition)
{
	std::wstringstream input(L"\"Hello, World!\"");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::String, Position(1, 1), L"Hello, World!"},
		{LexToken::TokenType::EndOfFile, Position(1, 16)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, KeywordAndIdentifierRecognition)
{
	std::wstringstream input(L"var myVariable while");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Var, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 5), L"myVariable"},
		{LexToken::TokenType::While, Position(1, 16)},
		{LexToken::TokenType::EndOfFile, Position(1, 21)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IntegerRecognition)
{
	std::wstringstream input(L"12345");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Integer, Position(1, 1), 12345},
		{LexToken::TokenType::EndOfFile, Position(1, 6)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, CommentRecognition)
{
	std::wstringstream input(L"# This is a comment\nvar");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Comment, Position(1, 1)},
		{LexToken::TokenType::Var, Position(2, 1)},
		{LexToken::TokenType::EndOfFile, Position(2, 4)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, UnrecognizedCharacterRecognition)
{
	std::wstringstream input(L"@");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1), L"@"},
		{LexToken::TokenType::EndOfFile, Position(1, 2)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::UnrecognizedSymbol, Position(1, 1)},
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, MultipleTokensIncludingWhitespaceAndOperators)
{
	std::wstringstream input(L"var count = 123 + myVar * 4;");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Var, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 5), L"count"},
		{LexToken::TokenType::Assign, Position(1, 11)},
		{LexToken::TokenType::Integer, Position(1, 13), 123},
		{LexToken::TokenType::Plus, Position(1, 17)},
		{LexToken::TokenType::Identifier, Position(1, 19), L"myVar"},
		{LexToken::TokenType::Asterisk, Position(1, 25)},
		{LexToken::TokenType::Integer, Position(1, 27), 4},
		{LexToken::TokenType::Semicolon, Position(1, 28)},
		{LexToken::TokenType::EndOfFile, Position(1, 29)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, LongStringLiteralWithEscapedCharacters)
{
	std::wstringstream input(L"\"This is a long string with \\\"escaped quotes\\\" and new\\nlines\"");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::String, Position(1, 1), L"This is a long string with \"escaped quotes\" and new\nlines"},
		{LexToken::TokenType::EndOfFile, Position(1, 63)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, MixedSingleAndMultiCharacterOperators)
{
	std::wstringstream input(L">= <= != && || = !");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::GreaterEqual, Position(1, 1)},
		{LexToken::TokenType::LessEqual, Position(1, 4)},
		{LexToken::TokenType::NotEqual, Position(1, 7)},
		{LexToken::TokenType::LogicalAnd, Position(1, 10)},
		{LexToken::TokenType::LogicalOr, Position(1, 13)},
		{LexToken::TokenType::Assign, Position(1, 16)},
		{LexToken::TokenType::LogicalNot, Position(1, 18)},
		{LexToken::TokenType::EndOfFile, Position(1, 19)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, MalformedTokens)
{
	std::wstringstream input(L"00123 var$ %abc");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1)},
		{LexToken::TokenType::Var, Position(1, 7)},
		{LexToken::TokenType::Unrecognized, Position(1, 10), L"$"},
		{LexToken::TokenType::Unrecognized, Position(1, 12), L"%"},
		{LexToken::TokenType::Identifier, Position(1, 13), L"abc"},
		{LexToken::TokenType::EndOfFile, Position(1, 16)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::InvalidNumber, Position(1, 1)},
		{LexicalError::ErrorType::UnrecognizedSymbol, Position(1, 10)},
		{LexicalError::ErrorType::UnrecognizedSymbol, Position(1, 12)},
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, NestedCommentsAndOperators)
{
	std::wstringstream input(L"# This is a # comment\nvar x += 10 # Another comment");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Comment, Position(1, 1)},
		{LexToken::TokenType::Var, Position(2, 1)},
		{LexToken::TokenType::Identifier, Position(2, 5), L"x"},
		{LexToken::TokenType::PlusAssign, Position(2, 7)},
		{LexToken::TokenType::Integer, Position(2, 10), 10},
		{LexToken::TokenType::Comment, Position(2, 13)},
		{LexToken::TokenType::EndOfFile, Position(3, 1)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, KeytwordInIdentifier)
{
	std::wstringstream input(L"while123");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Identifier, Position(1, 1), L"while123"},
		{LexToken::TokenType::EndOfFile, Position(1, 9)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, EdgeCaseMultipleNewlinesAndTabs)
{
	std::wstringstream input(L"\n\n\t\tvar a = 5\nwhile (a < 10) { a += 1; }\n");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Var, Position(3, 3)},
		{LexToken::TokenType::Identifier, Position(3, 7), L"a"},
		{LexToken::TokenType::Assign, Position(3, 9)},
		{LexToken::TokenType::Integer, Position(3, 11), 5},
		{LexToken::TokenType::While, Position(4, 1)},
		{LexToken::TokenType::LParenth, Position(4, 7)},
		{LexToken::TokenType::Identifier, Position(4, 8), L"a"},
		{LexToken::TokenType::Less, Position(4, 10)},
		{LexToken::TokenType::Integer, Position(4, 12), 10},
		{LexToken::TokenType::RParenth, Position(4, 14)},
		{LexToken::TokenType::LBracket, Position(4, 16)},
		{LexToken::TokenType::Identifier, Position(4, 18), L"a"},
		{LexToken::TokenType::PlusAssign, Position(4, 20)},
		{LexToken::TokenType::Integer, Position(4, 23), 1},
		{LexToken::TokenType::Semicolon, Position(4, 24)},
		{LexToken::TokenType::RBracket, Position(4, 26)},
		{LexToken::TokenType::EndOfFile, Position(5, 1)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, VariableAssignmentAndComment)
{
	std::wstringstream input(L"var a = 10;\n\nvar b = a * a;\n\n # b automatically converted to string and printed");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Var, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 5), L"a"},
		{LexToken::TokenType::Assign, Position(1, 7)},
		{LexToken::TokenType::Integer, Position(1, 9), 10},
		{LexToken::TokenType::Semicolon, Position(1, 11)},
		{LexToken::TokenType::Var, Position(3, 1)},
		{LexToken::TokenType::Identifier, Position(3, 5), L"b"},
		{LexToken::TokenType::Assign, Position(3, 7)},
		{LexToken::TokenType::Identifier, Position(3, 9), L"a"},
		{LexToken::TokenType::Asterisk, Position(3, 11)},
		{LexToken::TokenType::Identifier, Position(3, 13), L"a"},
		{LexToken::TokenType::Semicolon, Position(3, 14)},
		{LexToken::TokenType::Comment, Position(5, 2)},
		{LexToken::TokenType::EndOfFile, Position(6, 1)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IfElseBlock)
{
	std::wstringstream input(L"var b = false;\n\nif(b)\n{\n\t# do sth\n}\nelse\n{\n\t# do sth else\n}");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Var, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 5), L"b"},
		{LexToken::TokenType::Assign, Position(1, 7)},
		{LexToken::TokenType::Boolean, Position(1, 9), false},
		{LexToken::TokenType::Semicolon, Position(1, 14)},
		{LexToken::TokenType::If, Position(3, 1)},
		{LexToken::TokenType::LParenth, Position(3, 3)},
		{LexToken::TokenType::Identifier, Position(3, 4), L"b"},
		{LexToken::TokenType::RParenth, Position(3, 5)},
		{LexToken::TokenType::LBracket, Position(4, 1)},
		{LexToken::TokenType::Comment, Position(5, 2)},
		{LexToken::TokenType::RBracket, Position(6, 1)},
		{LexToken::TokenType::Else, Position(7, 1)},
		{LexToken::TokenType::LBracket, Position(8, 1)},
		{LexToken::TokenType::Comment, Position(9, 2)},
		{LexToken::TokenType::RBracket, Position(10, 1)},
		{LexToken::TokenType::EndOfFile, Position(10, 2)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, WhileLoop)
{
	std::wstringstream input(L"while(a < 10)\n{\n\t# do stuff\n}");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::While, Position(1, 1)},
		{LexToken::TokenType::LParenth, Position(1, 6)},
		{LexToken::TokenType::Identifier, Position(1, 7), L"a"},
		{LexToken::TokenType::Less, Position(1, 9)},
		{LexToken::TokenType::Integer, Position(1, 11), 10},
		{LexToken::TokenType::RParenth, Position(1, 13)},
		{LexToken::TokenType::LBracket, Position(2, 1)},
		{LexToken::TokenType::Comment, Position(3, 2)},
		{LexToken::TokenType::RBracket, Position(4, 1)},
		{LexToken::TokenType::EndOfFile, Position(4, 2)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, RecursiveFunction)
{
	std::wstringstream input(L"func Fizz(a, b)\n{\n\treturn Fizz(a - 1, b);\n}");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Func, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 6), L"Fizz"},
		{LexToken::TokenType::LParenth, Position(1, 10)},
		{LexToken::TokenType::Identifier, Position(1, 11), L"a"},
		{LexToken::TokenType::Comma, Position(1, 12)},
		{LexToken::TokenType::Identifier, Position(1, 14), L"b"},
		{LexToken::TokenType::RParenth, Position(1, 15)},
		{LexToken::TokenType::LBracket, Position(2, 1)},
		{LexToken::TokenType::Return, Position(3, 2)},
		{LexToken::TokenType::Identifier, Position(3, 9), L"Fizz"},
		{LexToken::TokenType::LParenth, Position(3, 13)},
		{LexToken::TokenType::Identifier, Position(3, 14), L"a"},
		{LexToken::TokenType::Minus, Position(3, 16)},
		{LexToken::TokenType::Integer, Position(3, 18), 1},
		{LexToken::TokenType::Comma, Position(3, 19)},
		{LexToken::TokenType::Identifier, Position(3, 21), L"b"},
		{LexToken::TokenType::RParenth, Position(3, 22)},
		{LexToken::TokenType::Semicolon, Position(3, 23)},
		{LexToken::TokenType::RBracket, Position(4, 1)},
		{LexToken::TokenType::EndOfFile, Position(4, 2)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, FunctionWithReturn)
{
	std::wstringstream input(L"func Add(a, b)\n{\n\treturn a + b;\n}");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Func, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 6), L"Add"},
		{LexToken::TokenType::LParenth, Position(1, 9)},
		{LexToken::TokenType::Identifier, Position(1, 10), L"a"},
		{LexToken::TokenType::Comma, Position(1, 11)},
		{LexToken::TokenType::Identifier, Position(1, 13), L"b"},
		{LexToken::TokenType::RParenth, Position(1, 14)},
		{LexToken::TokenType::LBracket, Position(2, 1)},
		{LexToken::TokenType::Return, Position(3, 2)},
		{LexToken::TokenType::Identifier, Position(3, 9), L"a"},
		{LexToken::TokenType::Plus, Position(3, 11)},
		{LexToken::TokenType::Identifier, Position(3, 13), L"b"},
		{LexToken::TokenType::Semicolon, Position(3, 14)},
		{LexToken::TokenType::RBracket, Position(4, 1)},
		{LexToken::TokenType::EndOfFile, Position(4, 2)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, SimpleMainFunction)
{
	std::wstringstream input(L"func main()\n{\n}");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Func, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 6), L"main"},
		{LexToken::TokenType::LParenth, Position(1, 10)},
		{LexToken::TokenType::RParenth, Position(1, 11)},
		{LexToken::TokenType::LBracket, Position(2, 1)},
		{LexToken::TokenType::RBracket, Position(3, 1)},
		{LexToken::TokenType::EndOfFile, Position(3, 2)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, HigherOrderFunctionComposition)
{
	std::wstringstream input(L"func compose(f, g)\n{\n\treturn func(x) { return f(g(x)); }\n}");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Func, Position(1, 1)},
		{LexToken::TokenType::Identifier, Position(1, 6), L"compose"},
		{LexToken::TokenType::LParenth, Position(1, 13)},
		{LexToken::TokenType::Identifier, Position(1, 14), L"f"},
		{LexToken::TokenType::Comma, Position(1, 15)},
		{LexToken::TokenType::Identifier, Position(1, 17), L"g"},
		{LexToken::TokenType::RParenth, Position(1, 18)},
		{LexToken::TokenType::LBracket, Position(2, 1)},
		{LexToken::TokenType::Return, Position(3, 2)},
		{LexToken::TokenType::Func, Position(3, 9)},
		{LexToken::TokenType::LParenth, Position(3, 13)},
		{LexToken::TokenType::Identifier, Position(3, 14), L"x"},
		{LexToken::TokenType::RParenth, Position(3, 15)},
		{LexToken::TokenType::LBracket, Position(3, 17)},
		{LexToken::TokenType::Return, Position(3, 19)},
		{LexToken::TokenType::Identifier, Position(3, 26), L"f"},
		{LexToken::TokenType::LParenth, Position(3, 27)},
		{LexToken::TokenType::Identifier, Position(3, 28), L"g"},
		{LexToken::TokenType::LParenth, Position(3, 29)},
		{LexToken::TokenType::Identifier, Position(3, 30), L"x"},
		{LexToken::TokenType::RParenth, Position(3, 31)},
		{LexToken::TokenType::RParenth, Position(3, 32)},
		{LexToken::TokenType::Semicolon, Position(3, 33)},
		{LexToken::TokenType::RBracket, Position(3, 35)},
		{LexToken::TokenType::RBracket, Position(4, 1)},
		{LexToken::TokenType::EndOfFile, Position(4, 2)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, RecognizesMultipleFloatsAndIntegers) {
	std::wstringstream input(L"3.14 2.718 42 0.5");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Float, Position(1, 1), 3.14f},
		{LexToken::TokenType::Float, Position(1, 6), 2.718f},
		{LexToken::TokenType::Integer, Position(1, 12), 42},
		{LexToken::TokenType::Float, Position(1, 15), 0.5f},
		{LexToken::TokenType::EndOfFile, Position(1, 18)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IntegerOverflow)
{
	std::wstringstream input(L"2147483648");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1)},
		{LexToken::TokenType::EndOfFile, Position(1, 11)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::IntegerOverflow, Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, FloatOverflow)
{
	std::wstringstream input(L"99999999999999999999999999999999999999999.1");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1)},
		{LexToken::TokenType::EndOfFile, Position(1, 44)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::FloatOverflow, Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, LeadingZerosError)
{
	std::wstringstream input(L"00042");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1)},
		{LexToken::TokenType::EndOfFile, Position(1, 6)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::InvalidNumber, Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, IncompleteStringLiteral)
{
	std::wstringstream input(L"\"Incomplete string");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1)},
		{LexToken::TokenType::EndOfFile, Position(1, 19)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::IncompleteStringLiteral, Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, CommentTooLong)
{
	std::wstring inputStr(550, '.');

	std::wstringstream input(L"#This is too long comment" + inputStr);
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Comment, Position(1, 1)},
		{LexToken::TokenType::EndOfFile, Position(2, 1)},
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::CommentTooLong, Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralWithValidEscapes)
{
	std::wstringstream input(L"\"Line1\\nLine2\\tTabbed\"");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::String, Position(1, 1), L"Line1\nLine2\tTabbed"},
		{LexToken::TokenType::EndOfFile, Position(1, 23)}
	};

	std::vector<LexicalError> expectedErrors = {};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralTooLong)
{
	std::wstring longString(1000, L'a');
	std::wstringstream input(L"\"" + longString + L"\"");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::Unrecognized, Position(1, 1)},
		{LexToken::TokenType::EndOfFile, Position(1, 1003)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::StringLiteralTooLong, Position(1, 1)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralInvalidEscapeSequence)
{
	std::wstringstream input(L"\"Invalid\\xEscape\"");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::String, Position(1, 1), L"InvalidxEscape"},
		{LexToken::TokenType::EndOfFile, Position(1, 17)}
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::InvalidEscapeSequence, Position(1, 8)}
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}

TEST_F(LexerTest, StringLiteralsComplexScenarios)
{
	std::wstringstream input(L"\"Valid string\" \"Too long string" + std::wstring(1000 + 1, L'a') + L"\"");
	auto lexer = Lexer(&input);
	const auto& lexerOut = lexer.ResolveAllRemaining();

	std::vector<LexToken> expectedTokens =
	{
		{LexToken::TokenType::String, Position(1, 1), L"Valid string"},
		{LexToken::TokenType::Unrecognized, Position(1, 16)},
		{LexToken::TokenType::EndOfFile, Position(1, 1034)},
	};

	std::vector<LexicalError> expectedErrors =
	{
		{LexicalError::ErrorType::StringLiteralTooLong, Position(1, 16)},
	};

	CompareTokens(lexerOut.first, expectedTokens);
	CompareErrors(lexerOut.second, expectedErrors);
}