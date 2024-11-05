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
	static std::unordered_map<Lexer::TokenType, std::wstring> tokenNames =
	{
		{Lexer::TokenType::Identifier, L"Identifier"},
		{Lexer::TokenType::Keyword, L"Keyword"},
		{Lexer::TokenType::Number, L"Number"},
		{Lexer::TokenType::String, L"String"},
		{Lexer::TokenType::Boolean, L"Boolean"},
		{Lexer::TokenType::Operator, L"Operator"},
		{Lexer::TokenType::Delimiter, L"Delimiter"},
		{Lexer::TokenType::Comment, L"Comment"},
		{Lexer::TokenType::EndOfFile, L"EndOfFile"},
		{Lexer::TokenType::Unrecognized, L"Unrecognized"},
	}; // just for testing purposes

	std::wcout << "Type: " << tokenNames[token.type] << " value: ";

	std::visit([](const auto& value) { std::wcout << value; }, token.value);

	std::wcout << " line: " << token.line << " column: " << token.column << std::endl;
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

	std::wifstream codeFile(CodesPath::exampleCodesPath + "TestCode.txt");
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