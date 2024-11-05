// Main.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <sstream>
#include "Lexer.h"
#include <unordered_map>
#include <fstream>
#include "PathConfig.h"

static void PrintToken(const Lexer::Token& token)
{
	static std::unordered_map<Lexer::TokenType, std::string> tokenNames =
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

	std::cout << "Type: " << tokenNames[token.type] << " value: ";

	std::visit([](const auto& value) { std::cout << value; }, token.value);

	std::cout << " line: " << token.line << " column: " << token.column << std::endl;
}

int main()
{
	Lexer lexer;

	/*std::string codeExample = R"(
		mut var a;
		a = 10;

		var b = "123";

		var c = a + b;    # c == 133 prioritizing numbers over strings
	)";

	std::istringstream codeStream(codeExample);
	const auto tokens = lexer.Tokenize(codeStream*/

	std::ifstream codeFile(CodesPath::exampleCodesPath + "TestCode.txt");
	if (!codeFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 1;
	}

	const auto tokens = lexer.Tokenize(codeFile);

	for (const auto& token : tokens)
	{
		PrintToken(token);
	}

	codeFile.close();

	return 0;
}