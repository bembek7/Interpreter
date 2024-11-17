#include "Lexer.h"
#include "Lexer.h"
#include <istream>
#include <cwctype>
#include <sstream>
#include <array>

// cannot use peek with wide chars

const std::unordered_map<std::wstring, Lexer::TokenType> Lexer::keywords =
{
	{ L"mut",		TokenType::Mut },
	{ L"var",		TokenType::Var },
	{ L"while",		TokenType::While },
	{ L"if",		TokenType::If },
	{ L"else",		TokenType::Else },
	{ L"return",	TokenType::Return },
	{ L"func",		TokenType::Func },
	{ L"true",		TokenType::Boolean },
	{ L"false",		TokenType::Boolean }
};

const std::unordered_map<std::wstring, Lexer::TokenType> Lexer::symbols =
{
	{ L";", TokenType::Semicolon },
	{ L",", TokenType::Comma },
	{ L"{", TokenType::LBracket },
	{ L"}", TokenType::RBracket },
	{ L"(", TokenType::LParenth },
	{ L")", TokenType::RParenth },
};

const std::unordered_map<std::wstring, Lexer::TokenType> Lexer::singleCharOperators =
{
	{ L"=", TokenType::Assign },
	{ L"+", TokenType::Plus },
	{ L"-", TokenType::Minus },
	{ L"*", TokenType::Asterisk },
	{ L"/", TokenType::Slash },
	{ L"!", TokenType::LogicalNot },
	{ L"<", TokenType::Less },
	{ L">", TokenType::Greater },
};

const std::unordered_map<std::wstring, Lexer::TokenType> Lexer::twoCharsOperators =
{
	{ L"&&", TokenType::LogicalAnd },
	{ L"||", TokenType::LogicalOr },
	{ L"==", TokenType::Equal },
	{ L"!=", TokenType::NotEqual },
	{ L"<=", TokenType::LessEqual },
	{ L">=", TokenType::GreaterEqual },
	{ L"+=", TokenType::PlusAssign },
	{ L"-=", TokenType::MinusAssign },
	{ L"*=", TokenType::AsteriskAssign },
	{ L"/=", TokenType::SlashAssign },
	{ L"&=", TokenType::AndAssign },
	{ L"|=", TokenType::OrAssign },
	{ L"<<", TokenType::FunctionBind },
	{ L">>", TokenType::FunctionCompose },
};

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
			foundErrors.push_back(LexicalError(ErrorType::CommentTooLong, message.str(), currentLine, currentColumn, true));
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
			if (numberStr.length() > maxNumberLength)
			{
				std::stringstream message{};
				message << "Number too long. Max number length: " << maxNumberLength << ".";
				foundErrors.push_back(LexicalError(ErrorType::FloatTooLong, message.str(), currentLine, currentColumn, true));
				const auto token = Token(TokenType::Unrecognized, currentLine, currentColumn, numberStr);
				currentColumn += numberStr.length();
				return token;
			}
		}
		else
		{
			if (currentChar == L'.' && !isFloat)
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
	}

	if (numberStr.length() > 1 && numberStr[0] == L'0' && numberStr[1] != L'.')
	{
		std::string message = "Invalid number format - leading zeros.";
		foundErrors.push_back(LexicalError(ErrorType::InvalidNumber, std::move(message), currentLine, currentColumn));
		const auto token = Token(TokenType::Unrecognized, currentLine, currentColumn, numberStr);
		currentColumn += numberStr.length();
		return token;
	}

	TokenType tokenType;
	std::variant<std::wstring, int, float, bool> tokenValue;
	if (isFloat)
	{
		tokenType = TokenType::Float;
		try // I don't think using exceptions here is the perfect solution, but seems to be the simplest one
		{
			tokenValue = std::stof(numberStr);
		}
		catch (std::out_of_range)
		{
			std::string message = "Number would fall out of the range of the float";
			foundErrors.push_back(LexicalError(ErrorType::FloatOverflow, std::move(message), currentLine, currentColumn));
			const auto token = Token(TokenType::Unrecognized, currentLine, currentColumn, numberStr);
			currentColumn += numberStr.length();
			return token;
		}
	}
	else
	{
		tokenType = TokenType::Integer;
		try
		{
			tokenValue = std::stoi(numberStr);
		}
		catch (std::out_of_range)
		{
			std::string message = "Number would fall out of the range of the integer";
			foundErrors.push_back(LexicalError(ErrorType::IntegerOverflow, std::move(message), currentLine, currentColumn));
			const auto token = Token(TokenType::Unrecognized, currentLine, currentColumn, numberStr);
			currentColumn += numberStr.length();
			return token;
		}
	}

	const auto token = Token(tokenType, currentLine, currentColumn, tokenValue);
	currentColumn += numberStr.length();
	return token;
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
		if (word.length() > maxIdentifierLength)
		{
			std::stringstream message{};
			message << "Identifier too long. Max identifier length: " << maxIdentifierLength << ".";
			foundErrors.push_back(LexicalError(ErrorType::IdentifierTooLong, message.str(), currentLine, currentColumn, true));
			return Token(TokenType::Unrecognized, currentLine, currentColumn);
		}
	}
	source.unget();

	TokenType tokenType;
	std::variant<std::wstring, int, float, bool> tokenValue = false;
	if (const auto tokenTypeOpt = FindTokenInMap(word, keywords))
	{
		tokenType = tokenTypeOpt.value();
		if (tokenType == TokenType::Boolean)
		{
			tokenValue = (word == L"true") ? true : false;
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
	if (const auto tokenType = FindTokenInMap({ currentChar }, symbols))
	{
		const auto token = Token(tokenType.value(), currentLine, currentColumn);
		currentColumn++;
		return token;
	}

	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildSingleCharOperator()
{
	if (const auto tokenType = FindTokenInMap({ currentChar }, singleCharOperators))
	{
		const auto token = Token(tokenType.value(), currentLine, currentColumn);
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
		if (const auto tokenType = FindTokenInMap({ currentChar, nextChar }, twoCharsOperators))
		{
			const auto token = Token(tokenType.value(), currentLine, currentColumn);
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
		builtString += nextChar;
		if (builtString.length() > maxStringLiteralLength)
		{
			std::stringstream message{};
			message << "String literal too long. Max string literal length: " << maxStringLiteralLength << ".";
			foundErrors.push_back(LexicalError(ErrorType::StringLiteralTooLong, message.str(), currentLine, currentColumn, true));
			return Token(TokenType::Unrecognized, currentLine, currentColumn);
		}
		if (nextChar == L'"')
		{
			const auto token = Token(TokenType::String, currentLine, currentColumn, builtString);
			currentColumn += builtString.length() + 2;
			return token;
		}
		if (nextChar == L'\\')
		{
			if (source.get(nextChar))
			{
				builtString += nextChar;

				static const std::array<wchar_t, 4> handledEscapedChars = { L'"',  L'\\', L'n', L't' };
				if (std::find(handledEscapedChars.begin(), handledEscapedChars.end(), nextChar) == handledEscapedChars.end())
				{
					std::string message = "Unrecognized character escape sequence.";
					foundErrors.push_back(LexicalError(ErrorType::InvalidEscapeSequence, std::move(message), currentLine, currentColumn + builtString.length()));
				}
			}
			else
			{
				break;
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
	foundErrors.push_back(LexicalError(ErrorType::UnrecognizedSymbol, "Unrecognized symbol.", currentLine, currentColumn));
	currentColumn++;
	return token.value();
}

std::optional<Lexer::TokenType> Lexer::FindTokenInMap(const std::wstring& key, const std::unordered_map<std::wstring, TokenType>& map) noexcept
{
	if (const auto it = map.find(key); it != map.end())
	{
		return it->second;
	}
	return std::nullopt;
}