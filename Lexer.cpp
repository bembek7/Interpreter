#include "Lexer.h"
#include <istream>
#include <cwctype>

std::vector<Lexer::Token> Lexer::Tokenize(std::istream& source) const
std::vector<Lexer::Token> Lexer::Tokenize(std::wistream& source) const
{
	std::vector<Token> resolvedTokens;

	int line = 1;
	int column = 1;

	wchar_t currentChar;

	while (source.get(currentChar))
	{
		resolvedTokens.push_back(Token(TokenType::Test, std::string{ ch }, line, column));

		if (ch == '\n')
		{
			line++;
			column = 1;
		}
		else
		{
			column++;
		}
	}

	resolvedTokens.push_back(Token(TokenType::EndOfFile, "", line, column));

	return resolvedTokens;
}