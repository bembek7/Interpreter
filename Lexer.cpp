#include "Lexer.h"
#include <istream>
#include <cwctype>

// TODO (not obvious stuff, well obvo=ious too)
// warp building sections into functions returning optionals
// overflow checking
// minuses before numbers

// cannot use peek with wide chars

std::vector<Lexer::Token> Lexer::Tokenize(std::wistream& source) const
{
	std::vector<Token> resolvedTokens;

	int line = 1;
	int column = 1;

	wchar_t currentChar;

	while (source.get(currentChar))
	{
		if (std::isspace(currentChar))
		{
			if (currentChar == L'\n')
			{
				line++;
				column = 1;
			}
			else
			{
				column++;
			}
			continue;
		}
		std::pair<int, int> tokenStartingPosition = { line, column };
		if (currentChar == L'#') // build comment
		{
			unsigned int commentLength = 0;
			while (source.get(currentChar) && currentChar != L'\n')
			{
				commentLength++;
				if (commentLength > maxCommentLength)
				{
					// ereor: comment too long
				}
			};
			line++;
			column = 1;
			resolvedTokens.push_back(Token(TokenType::Comment, std::wstring(), tokenStartingPosition.first, tokenStartingPosition.second));
			continue;
		}
		if (std::iswdigit(currentChar)) // build integer
		{
			std::wstring builtNumber{ currentChar };
			while (source.get(currentChar) && std::iswdigit(currentChar))
			{
				if (*builtNumber.begin() == L'0')
				{
					// error invalid integer def, cannot be '0123' or '0000123', has to be '123'
				}
				builtNumber += currentChar;
				if (builtNumber.length() > maxIntegerLength)
				{
					// error too long integer def
				}
			}
			source.unget();
			// overflow check
			resolvedTokens.push_back(Token(TokenType::Integer, std::stoi(builtNumber), line, column));
			column += builtNumber.length();
			continue;
		}
	}

	resolvedTokens.push_back(Token(TokenType::EndOfFile, L"", line, column));

	return resolvedTokens;
}