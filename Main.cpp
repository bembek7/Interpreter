// Main.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <sstream>
#include "Lexer.h"
#include <unordered_map>

int main()
{
	std::unordered_map<Lexer::TokenType, std::string> tokenNames =
	{
		{Lexer::TokenType::Identifier, "Identifier"},
		{Lexer::TokenType::Keyword, "Keyword"},
		{Lexer::TokenType::Number, "Number"},
		{Lexer::TokenType::String, "String"},
		{Lexer::TokenType::Operator, "Operator"},
		{Lexer::TokenType::Delimiter, "Delimiter"},
		{Lexer::TokenType::Comment, "Comment"},
		{Lexer::TokenType::EndOfFile, "EndOfFile"},
		{Lexer::TokenType::Error, "Error"},
		{Lexer::TokenType::Test, "Test"}
	}; // just for testing purposes

	std::string codeExample = R"(
        mut var a;
        a = 10;

        var b = "123";

        var c = a + b;    # c == 133 prioritizing numbers over strings
    )";

	Lexer lexer;

	std::istringstream codeStream(codeExample);
	const auto tokens = lexer.Tokenize(codeStream);

	for (const auto& token : tokens)
	{
		std::cout << tokenNames[token.type] << ": ";

		std::visit([](const auto& value) { std::cout << value; }, token.value);

		std::cout << std::endl;
	}

	return 0;
}