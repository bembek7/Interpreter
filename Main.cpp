// Main.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <sstream>
#include "Lexer.h"
#include <unordered_map>
#include <fstream>
#include "PathConfig.h"

static void PrintToken(const Lexer::Token& token) noexcept
{
	static std::unordered_map<Lexer::TokenType, std::wstring> tokenNames =
	{
		{Lexer::TokenType::Identifier, L"Identifier"},
		{Lexer::TokenType::Integer, L"Integer"},
		{Lexer::TokenType::Float, L"Float"},
		{Lexer::TokenType::String, L"String"},
		{Lexer::TokenType::Boolean, L"Boolean"},
		{Lexer::TokenType::Semicolon, L"Semicolon"},
		{Lexer::TokenType::Comment, L"Comment"},
		{Lexer::TokenType::EndOfFile, L"EndOfFile"},
		{Lexer::TokenType::Unrecognized, L"Unrecognized"},
		{Lexer::TokenType::Assign, L"Assign"},
		{Lexer::TokenType::Plus, L"Plus"},
		{Lexer::TokenType::Minus, L"Minus"},
		{Lexer::TokenType::Asterisk, L"Asterisk"},
		{Lexer::TokenType::Slash, L"Slash"},
		{Lexer::TokenType::LogicalNot, L"LogicalNot"},
		{Lexer::TokenType::Less, L"Less"},
		{Lexer::TokenType::Greater, L"Greater"},
		{Lexer::TokenType::LogicalAnd, L"LogicalAnd"},
		{Lexer::TokenType::LogicalOr, L"LogicalOr"},
		{Lexer::TokenType::Equal, L"Equal"},
		{Lexer::TokenType::LessEqual, L"LessEqual"},
		{Lexer::TokenType::GreaterEqual, L"GreaterEqual"},
		{Lexer::TokenType::NotEqual, L"NotEqual"},
		{Lexer::TokenType::PlusAssign, L"PlusAssign"},
		{Lexer::TokenType::MinusAssign, L"MinusAssign"},
		{Lexer::TokenType::AsteriskAssign, L"AsteriskAssign"},
		{Lexer::TokenType::SlashAssign, L"SlashAssign"},
		{Lexer::TokenType::AndAssign, L"AndAssign"},
		{Lexer::TokenType::OrAssign, L"OrAssign"},
		{Lexer::TokenType::Mut, L"Mut"},
		{Lexer::TokenType::Var, L"Var"},
		{Lexer::TokenType::While, L"While"},
		{Lexer::TokenType::If, L"If"},
		{Lexer::TokenType::Else, L"Else"},
		{Lexer::TokenType::Return, L"Return"},
		{Lexer::TokenType::Func, L"Func"}
	}; // just for testing purposes

	std::wcout << "Type: " << tokenNames[token.type] << " value: ";

	std::visit([](const auto& value) { std::wcout << value; }, token.value);

	std::wcout << " line: " << token.line << " column: " << token.column << std::endl;
}

static void PrintError(const Lexer::LexicalError& error) noexcept
{
	std::cout << "Error [line: " << error.line << ", column : " << error.column << "] " << error.message << std::endl;
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

	const auto lexerOut = lexer.Tokenize(codeFile);

	for (const auto& error : lexerOut.second)
	{
		PrintError(error);
	}

	for (const auto& token : lexerOut.first)
	{
		PrintToken(token);
	}

	codeFile.close();

	return 0;
}