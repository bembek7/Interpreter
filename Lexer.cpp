#include "Lexer.h"
#include <istream>
#include <cwctype>
#include <sstream>

// TODO (not obvious stuff, well obvious too)
// overflow checking
// minuses before numbers
// too long number, too long identifier
// mroe tokens
// cannot use peek with wide chars

std::pair<std::vector<Lexer::Token>, std::vector<Lexer::LexicalError>> Lexer::Tokenize(std::wistream& source)
{
	std::vector<Token> resolvedTokens;
	foundErrors = {};
	currentLine = 1;
	currentColumn = 1;

	while (source.get(currentChar))
	{
		if (std::isspace(currentChar))
		{
			if (currentChar == L'\n')
			{
				currentLine++;
				currentColumn = 1;
			}
			else
			{
				currentColumn++;
			}
			continue;
		}

		resolvedTokens.push_back(BuildToken(source));
		if (const auto& last = foundErrors.end(); last != foundErrors.end())
		{
			if (last->terminating)
			{
				return { resolvedTokens, foundErrors };
			}
		}
	}

	resolvedTokens.push_back(Token(TokenType::EndOfFile, L"", currentLine, currentColumn));

	return { resolvedTokens, foundErrors };
}

std::optional<Lexer::Token> Lexer::TryBuildComment(std::wistream& source)
{
	if (currentChar != L'#')
	{
		return std::nullopt;
	}

	unsigned int commentLength = 0;
	while (source.get(currentChar) && currentChar != L'\n')
	{
		commentLength++;
		if (commentLength > maxCommentLength)
		{
			std::stringstream message{};
			message << "Comment too long. Max comment length: " << maxCommentLength << ".";
			foundErrors.push_back(LexicalError(ErrorType::TooLong, message.str(), currentLine, currentColumn, true));
			return Token(TokenType::Comment, std::wstring{}, currentLine, currentColumn);
		}
	};
	const auto token = Token(TokenType::Comment, std::wstring(), currentLine, currentColumn);
	currentLine++;
	currentColumn = 1;
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildNumber(std::wistream& source)
{
	if (!std::iswdigit(currentChar))
	{
		return std::nullopt;
	}

	std::wstring numberStr{ currentChar };
	bool isFloat = false;

	while (source.get(currentChar))
	{
		if (std::iswdigit(currentChar))
		{
			numberStr += currentChar;
		}
		else if (currentChar == L'.' && !isFloat)
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

	if (!isFloat && numberStr[0] == L'0' && numberStr.length() > 1)
	{
		std::string message = "Invalid integer format - leading zeros.";
		foundErrors.push_back(LexicalError(ErrorType::InvalidNumber, std::move(message), currentLine, currentColumn, true));
		return Token(TokenType::Unrecognized, numberStr, currentLine, currentColumn);
	}

	if (isFloat)
	{
		float floatValue = std::stof(numberStr);
		const auto token = Token(TokenType::Float, floatValue, currentLine, currentColumn);
		currentColumn += numberStr.length();
		return token;
	}
	else
	{
		int intValue = std::stoi(numberStr);
		const auto token = Token(TokenType::Integer, intValue, currentLine, currentColumn);
		currentColumn += numberStr.length();
		return token;
	}

	// error too long number
}

std::optional<Lexer::Token> Lexer::TryBuildKeywordOrIdentifier(std::wistream& source)
{
	if (!std::iswalpha(currentChar) && currentChar != L'_')
	{
		return std::nullopt;
	}

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
	const auto token = Token(type, value, currentLine, currentColumn);
	currentColumn += identifier.length();
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildDelimiter(std::wistream& source)
{
	if (!std::wcschr(L";(){},", currentChar))
	{
		return std::nullopt;
	}

	const auto token = Token(TokenType::Delimiter, std::wstring{ currentChar }, currentLine, currentColumn);
	currentColumn++;
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildSingleCharOperator(std::wistream& source)
{
	if (std::find(singleCharOperators.begin(), singleCharOperators.end(), std::wstring(1, currentChar)) == singleCharOperators.end())
	{
		return std::nullopt;
	}

	const auto token = Token(TokenType::Operator, std::wstring{ currentChar }, currentLine, currentColumn);
	currentColumn++;
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildTwoCharsOperator(std::wistream& source)
{
	wchar_t nextChar;
	if (source.get(nextChar))
	{
		std::wstring opStr{ currentChar, nextChar };
		if (std::find(twoCharsOperators.begin(), twoCharsOperators.end(), opStr) != twoCharsOperators.end())
		{
			const auto token = Token(TokenType::Operator, opStr, currentLine, currentColumn);
			currentColumn += 2;
			return token;
		}
	}
	source.unget();
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildOperator(std::wistream& source)
{
	if (std::optional<Token> token = TryBuildTwoCharsOperator(source))
	{
		return token.value();
	}
	else
	{
		return TryBuildSingleCharOperator(source);
	}
}

std::optional<Lexer::Token> Lexer::TryBuildStringLiteral(std::wistream& source)
{
	if (currentChar != L'"')
	{
		return std::nullopt;
	}

	std::wstring builtString;
	builtString += currentChar;

	wchar_t nextChar;
	while (source.get(nextChar))
	{
		if (nextChar == L'"')
		{
			builtString += nextChar;
			const auto token = Token(TokenType::String, builtString, currentLine, currentColumn);
			currentColumn += builtString.length() + 2;
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
					std::string message = "Unrecognized character escape sequence.";
					foundErrors.push_back(LexicalError(ErrorType::InvalidEscapeSequence, std::move(message), currentLine, currentColumn + builtString.length()));
				}
			}
			else
			{
				std::string message = "Incomplete escape sequence.";
				foundErrors.push_back(LexicalError(ErrorType::InvalidEscapeSequence, std::move(message), currentLine, currentColumn + builtString.length()));
				return Token(TokenType::Unrecognized, builtString, currentLine, currentColumn);
			}
		}
		else
		{
			builtString += nextChar;
			if (builtString.length() > maxStringLiteralLength)
			{
				std::stringstream message{};
				message << "String literal too long. Max string literal length: " << maxStringLiteralLength << ".";
				foundErrors.push_back(LexicalError(ErrorType::TooLong, message.str(), currentLine, currentColumn, true));
				return Token(TokenType::Unrecognized, std::wstring{}, currentLine, currentColumn);
			}
		}
	}
	std::string message = "Incomplete string literal.";
	foundErrors.push_back(LexicalError(ErrorType::IncompleteStringLiteral, std::move(message), currentLine, currentColumn));
	const auto token = Token(TokenType::Unrecognized, builtString, currentLine, currentColumn);
	currentColumn += builtString.length() + 1;
	return token;
}

Lexer::Token Lexer::BuildToken(std::wistream& source)
{
	std::optional<Token> token;
	// could use some function table ???
	if (token = TryBuildComment(source))
	{
		return token.value();
	}
	if (token = TryBuildNumber(source))
	{
		return token.value();
	}
	if (token = TryBuildKeywordOrIdentifier(source))
	{
		return token.value();
	}
	if (token = TryBuildDelimiter(source))
	{
		return token.value();
	}
	if (token = TryBuildOperator(source))
	{
		return token.value();
	}
	if (token = TryBuildStringLiteral(source))
	{
		return token.value();
	}

	token = Token(TokenType::Unrecognized, std::wstring{ currentChar }, currentLine, currentColumn);
	currentColumn++;
	return token.value();
}