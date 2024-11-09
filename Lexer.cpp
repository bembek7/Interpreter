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

std::optional<Lexer::Token> Lexer::TryBuildNumber(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (std::iswdigit(currentChar))
	{
		std::wstring numberStr{ currentChar };
		bool isFloat = false;

		// Read the following characters to build a full number (integer or float)
		while (source.get(currentChar))
		{
			if (std::iswdigit(currentChar))
			{
				numberStr += currentChar;
			}
			else if (currentChar == L'.' && !isFloat) // only allow one decimal point
			{
				isFloat = true;
				numberStr += currentChar;
			}
			else
			{
				source.unget();
				break;
			}
		}

		// Check for overflow or invalid leading zeros for integer
		if (!isFloat && numberStr[0] == L'0' && numberStr.length() > 1)
		{
			// Error: Invalid integer format (e.g., leading zeros)
			// Handle this error appropriately
			return std::nullopt;
		}

		if (isFloat)
		{
			float floatValue = std::stof(numberStr);
			const auto token = Token(TokenType::Float, floatValue, line, column);
			column += (unsigned int)numberStr.length();
			return token;
		}
		else
		{
			int intValue = std::stoi(numberStr);
			const auto token = Token(TokenType::Integer, intValue, line, column);
			column += (unsigned int)numberStr.length();
			return token;
		}
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
		std::variant<std::wstring, int, float, bool> value = identifier;
		if (isKeyword && (identifier == L"true" || identifier == L"false"))
		{
			type = TokenType::Boolean;
			value = (identifier == L"true") ? true : false;
		}
		const auto token = Token(type, value, line, column);
		column += (unsigned int)identifier.length();
		return token;
	}
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildDelimiter(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (std::wcschr(L";(){},", currentChar))
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

std::optional<Lexer::Token> Lexer::TryBuildOperator(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (std::optional<Token> token = TryBuildTwoCharsOperator(currentChar, source, line, column))
	{
		return token.value();
	}
	else
	{
		return TryBuildSingleCharOperator(currentChar, source, line, column);
	}
}

std::optional<Lexer::Token> Lexer::TryBuildStringLiteral(wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const
{
	if (currentChar == L'"')
	{
		std::wstring builtString;
		builtString += currentChar;

		wchar_t nextChar;
		while (source.get(nextChar))
		{
			if (nextChar == L'"')
			{
				builtString += nextChar;
				const auto token = Token(TokenType::String, builtString, line, column);
				column += (unsigned int)builtString.length() + 2;
				return token;
			}
			if (nextChar == L'\\')
			{
				builtString += nextChar;
				if (source.get(nextChar))
				{
					if (nextChar == L'"' || nextChar == L'\\' || nextChar == L'n' || nextChar == L't')
					{
						builtString += nextChar;
					}
					else
					{
						// Error: Invalid escape sequence
					}
				}
				else
				{
					// Error: incomplete escape sequence
				}
			}
			else
			{
				builtString += nextChar;
			}
		}
	}

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
	if (token = TryBuildNumber(currentChar, source, line, column))
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
	if (token = TryBuildOperator(currentChar, source, line, column))
	{
		return token.value();
	}
	if (token = TryBuildStringLiteral(currentChar, source, line, column))
	{
		return token.value();
	}

	token = Token(TokenType::Unrecognized, std::wstring{ currentChar }, line, column);
	column++;
	return token.value();
}