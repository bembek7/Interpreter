// Main.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <sstream>
#include "Lexer.h"
#include <unordered_map>
#include <fstream>
#include "PathConfig.h"

static void PrintToken(const LexToken& token) noexcept
{
	static std::unordered_map<LexToken::TokenType, std::wstring> tokenNames =
	{
		{LexToken::TokenType::Identifier, L"Identifier"},
		{LexToken::TokenType::Integer, L"Integer"},
		{LexToken::TokenType::Float, L"Float"},
		{LexToken::TokenType::String, L"String"},
		{LexToken::TokenType::Boolean, L"Boolean"},
		{LexToken::TokenType::Semicolon, L"Semicolon"},
		{LexToken::TokenType::Comment, L"Comment"},
		{LexToken::TokenType::EndOfFile, L"EndOfFile"},
		{LexToken::TokenType::Unrecognized, L"Unrecognized"},
		{LexToken::TokenType::Assign, L"Assign"},
		{LexToken::TokenType::Plus, L"Plus"},
		{LexToken::TokenType::Minus, L"Minus"},
		{LexToken::TokenType::Asterisk, L"Asterisk"},
		{LexToken::TokenType::Slash, L"Slash"},
		{LexToken::TokenType::LogicalNot, L"LogicalNot"},
		{LexToken::TokenType::Less, L"Less"},
		{LexToken::TokenType::Greater, L"Greater"},
		{LexToken::TokenType::LogicalAnd, L"LogicalAnd"},
		{LexToken::TokenType::LogicalOr, L"LogicalOr"},
		{LexToken::TokenType::Equal, L"Equal"},
		{LexToken::TokenType::LessEqual, L"LessEqual"},
		{LexToken::TokenType::GreaterEqual, L"GreaterEqual"},
		{LexToken::TokenType::NotEqual, L"NotEqual"},
		{LexToken::TokenType::PlusAssign, L"PlusAssign"},
		{LexToken::TokenType::MinusAssign, L"MinusAssign"},
		{LexToken::TokenType::AsteriskAssign, L"AsteriskAssign"},
		{LexToken::TokenType::SlashAssign, L"SlashAssign"},
		{LexToken::TokenType::AndAssign, L"AndAssign"},
		{LexToken::TokenType::OrAssign, L"OrAssign"},
		{LexToken::TokenType::Mut, L"Mut"},
		{LexToken::TokenType::Var, L"Var"},
		{LexToken::TokenType::While, L"While"},
		{LexToken::TokenType::If, L"If"},
		{LexToken::TokenType::Else, L"Else"},
		{LexToken::TokenType::Return, L"Return"},
		{LexToken::TokenType::Func, L"Func"}
	}; // just for testing purposes

	std::wcout << "Type: " << tokenNames[token.GetType()] << " line: " << token.GetPosition().line << " column: " << token.GetPosition().column << std::endl;
}

static void PrintError(const Lexer::LexicalError& error) noexcept
{
	std::cout << "Error [line: " << error.position.line << ", column : " << error.position.column << "] " << error.message << std::endl;
}

int main()
{
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

	Lexer lexer = Lexer(&codeFile);
	const auto lexerOut = lexer.ResolveAllRemaining();

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