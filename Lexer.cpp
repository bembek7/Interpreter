#include "Lexer.h"
#include <istream>
#include <cwctype>

// TODO (not obvious stuff, well obvious too)
// warp building sections into functions returning optionals
// overflow checking
// minuses before numbers

// cannot use peek with wide chars

std::vector<Lexer::Token> Lexer::Tokenize(std::wistream& source) const
{
	std::vector<Token> resolvedTokens;

	unsigned int line = 1;
	unsigned int column = 1;

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

		resolvedTokens.push_back(BuildToken(currentChar, source, line, column));
	}

	resolvedTokens.push_back(Token(TokenType::EndOfFile, L"", line, column));

	return resolvedTokens;
}

std::optional<Lexer::Token> Lexer::TryBuildComment(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (currentChar == L'#')
	{
		unsigned int commentLength = 0;
		while (source.get(currentChar) && currentChar != L'\n')
		{
			commentLength++;
			if (commentLength > maxCommentLength)
			{
				// error: comment too long
			}
		};
		const auto token = Token(TokenType::Comment, std::wstring(), line, column);
		line++;
		column = 1;
		return token;
	}
	return std::nullopt;
}

std::optional< Lexer::Token> Lexer::TryBuildInteger(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
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
		// needs overflow check
		const auto token = Token(TokenType::Integer, std::stoi(builtNumber), line, column);
		column += (unsigned int)builtNumber.length();
		return token;
	}
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildKeywordOrIdentifier(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (std::iswalpha(currentChar) || currentChar == L'_')
	{
		std::wstring identifier{ currentChar };
		while (source.get(currentChar) && (std::iswalnum(currentChar) || currentChar == L'_'))
		{
			identifier += currentChar;
		}
		source.unget();

		const bool isKeyword = (std::find(keywords.begin(), keywords.end(), identifier) != keywords.end());
		TokenType type = isKeyword ? TokenType::Keyword : TokenType::Identifier;
		const auto token = Token(type, identifier, line, column);
		column += (unsigned int)identifier.length();
		return token;
	}
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildDelimiter(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (std::wcschr(L";", currentChar))
	{
		const auto token = Token(TokenType::Delimiter, std::wstring{ currentChar }, line, column);
		column++;
		return token;
	}
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildSingleCharOperator(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (std::find(singleCharOperators.begin(), singleCharOperators.end(), std::wstring(1, currentChar)) != singleCharOperators.end())
	{
		const auto token = Token(TokenType::Operator, std::wstring{ currentChar }, line, column);
		column++;
		return token;
	}
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildTwoCharsOperator(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	wchar_t nextChar;
	if (source.get(nextChar))
	{
		std::wstring opStr{ currentChar, nextChar };
		if (std::find(twoCharsOperators.begin(), twoCharsOperators.end(), opStr) != twoCharsOperators.end())
		{
			const auto token = Token(TokenType::Operator, opStr, line, column);
			column += 2;
			return token;
		}
	}
	source.unget();
	return std::nullopt;
}

Lexer::Token Lexer::BuildToken(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	std::optional<Token> token;
	// could use some function table ???
	if (token = TryBuildComment(currentChar, source, line, column))
	{
		return token.value();
	}
	if (token = TryBuildInteger(currentChar, source, line, column))
	{
		return token.value();
	}
	if (token = TryBuildKeywordOrIdentifier(currentChar, source, line, column))
	{
		return token.value();
	}
	if (token = TryBuildDelimiter(currentChar, source, line, column))
	{
		return token.value();
	}
	if (token = TryBuildSingleCharOperator(currentChar, source, line, column))
	{
		return token.value();
	}
	if (token = TryBuildTwoCharsOperator(currentChar, source, line, column))
	{
		return token.value();
	}

	token = Token(TokenType::Unrecognized, { currentChar }, line, column);
	column++;
	return token.value();
}