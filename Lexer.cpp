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

	resolvedTokens.push_back(Token(TokenType::EndOfFile, currentLine, currentColumn));

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
			return Token(TokenType::Comment, currentLine, currentColumn);
		}
	};
	const auto token = Token(TokenType::Comment, currentLine, currentColumn);
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
		return Token(TokenType::Unrecognized, currentLine, currentColumn, numberStr);
	}

	if (isFloat)
	{
		float floatValue = std::stof(numberStr);
		const auto token = Token(TokenType::Float, currentLine, currentColumn, floatValue);
		currentColumn += numberStr.length();
		return token;
	}
	else
	{
		int intValue = std::stoi(numberStr);
		const auto token = Token(TokenType::Integer, currentLine, currentColumn, intValue);
		currentColumn += numberStr.length();
		return token;
	}

	// error too long number
}

std::optional<Lexer::Token> Lexer::TryBuildWord(std::wistream& source)
{
	if (!std::iswalpha(currentChar) && currentChar != L'_')
	{
		return std::nullopt;
	}

	std::wstring word{ currentChar };
	while (source.get(currentChar) && (std::iswalnum(currentChar) || currentChar == L'_'))
	{
		word += currentChar;
		// too long idntifer check
	}
	source.unget();

	TokenType tokenType;
	std::variant<std::wstring, int, float, bool> tokenValue = false;
	if (std::find(keywords.begin(), keywords.end(), word) != keywords.end())
	{
		if (word == L"true" || word == L"false")
		{
			tokenType = TokenType::Boolean;
			tokenValue = (word == L"true") ? true : false;
		}
		else
		{
			tokenType = TokenType::Keyword;
		}
	}
	else
	{
		tokenType = TokenType::Identifier;
		tokenValue = word;
	}
	const auto token = Token(tokenType, currentLine, currentColumn, tokenValue);
	currentColumn += word.length();
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildSymbol()
{
	auto symbol = std::wstring{ currentChar };
	if (const auto it = symbols.find(symbol); it != symbols.end())
	{
		const auto token = Token(it->second, currentLine, currentColumn);
		currentColumn++;
		return token;
	}

	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildSingleCharOperator()
{
	auto operatorSymbol = std::wstring{ currentChar };
	if (const auto it = singleCharOperators.find(operatorSymbol); it != singleCharOperators.end())
	{
		const auto token = Token(it->second, currentLine, currentColumn);
		currentColumn++;
		return token;
	}
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildTwoCharsOperator(std::wistream& source)
{
	wchar_t nextChar;
	if (source.get(nextChar))
	{
		std::wstring operatorStr{ currentChar, nextChar };
		if (const auto it = twoCharsOperators.find(operatorStr); it != twoCharsOperators.end())
		{
			const auto token = Token(it->second, currentLine, currentColumn);
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
		return TryBuildSingleCharOperator();
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
			const auto token = Token(TokenType::String, currentLine, currentColumn, builtString);
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
				return Token(TokenType::Unrecognized, currentLine, currentColumn, builtString);
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
				return Token(TokenType::Unrecognized, currentLine, currentColumn);
			}
		}
	}
	std::string message = "Incomplete string literal.";
	foundErrors.push_back(LexicalError(ErrorType::IncompleteStringLiteral, std::move(message), currentLine, currentColumn));
	const auto token = Token(TokenType::Unrecognized, currentLine, currentColumn, builtString);
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
	if (token = TryBuildWord(source))
	{
		return token.value();
	}
	if (token = TryBuildSymbol())
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

	token = Token(TokenType::Unrecognized, currentLine, currentColumn, std::wstring{ currentChar });
	currentColumn++;
	return token.value();
}