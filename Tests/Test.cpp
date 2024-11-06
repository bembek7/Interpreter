#include <gtest/gtest.h>

#include <gtest/gtest.h>
#include <sstream>
#include "Lexer.h"

class LexerTest : public ::testing::Test {
protected:
    Lexer lexer;
};

TEST_F(LexerTest, SingleCharOperatorRecognition) {
    std::wstringstream input(L"= + - * / !");
    auto tokens = lexer.Tokenize(input);

    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::Operator);
    EXPECT_EQ(std::get<std::wstring>(tokens[0].value), L"=");
    EXPECT_EQ(tokens[1].type, Lexer::TokenType::Operator);
    EXPECT_EQ(std::get<std::wstring>(tokens[1].value), L"+");
}

TEST_F(LexerTest, TwoCharOperatorRecognition) {
    std::wstringstream input(L"&& || == !=");
    auto tokens = lexer.Tokenize(input);

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::Operator);
    EXPECT_EQ(std::get<std::wstring>(tokens[0].value), L"&&");
    EXPECT_EQ(tokens[1].type, Lexer::TokenType::Operator);
    EXPECT_EQ(std::get<std::wstring>(tokens[1].value), L"||");
}

TEST_F(LexerTest, StringLiteralRecognition) {
    std::wstringstream input(L"\"Hello, World!\"");
    auto tokens = lexer.Tokenize(input);

    const wchar_t quote = L'"';
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::String);
    EXPECT_EQ(std::get<std::wstring>(tokens[0].value), L"\"Hello, World!\"");
    EXPECT_EQ(std::get<std::wstring>(tokens[0].value)[0], quote);
}

TEST_F(LexerTest, KeywordAndIdentifierRecognition) {
    std::wstringstream input(L"var myVariable while");
    auto tokens = lexer.Tokenize(input);

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::Keyword);
    EXPECT_EQ(std::get<std::wstring>(tokens[0].value), L"var");
    EXPECT_EQ(tokens[1].type, Lexer::TokenType::Identifier);
    EXPECT_EQ(std::get<std::wstring>(tokens[1].value), L"myVariable");
    EXPECT_EQ(tokens[2].type, Lexer::TokenType::Keyword);
    EXPECT_EQ(std::get<std::wstring>(tokens[2].value), L"while");
}

TEST_F(LexerTest, IntegerRecognition) {
    std::wstringstream input(L"12345");
    auto tokens = lexer.Tokenize(input);

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::Integer);
    EXPECT_EQ(std::get<int>(tokens[0].value), 12345);
}

TEST_F(LexerTest, CommentRecognition) {
    std::wstringstream input(L"# This is a comment\nvar");
    auto tokens = lexer.Tokenize(input);

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::Comment);
    EXPECT_EQ(tokens[1].type, Lexer::TokenType::Keyword);
    EXPECT_EQ(std::get<std::wstring>(tokens[1].value), L"var");
}

TEST_F(LexerTest, UnrecognizedCharacterRecognition) {
    std::wstringstream input(L"@");
    auto tokens = lexer.Tokenize(input);

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, Lexer::TokenType::Unrecognized);
    EXPECT_EQ(std::get<std::wstring>(tokens[0].value), L"@");
}
