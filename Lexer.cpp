#include "Lexer.h"
#include "Lexer.h"
#include "Lexer.h"
#include "Lexer.h"
#include <istream>
#include <cwctype>
#include <sstream>
#include <array>

// cannot use peek with wide chars
// could do some better errors throwing to avoid code repetition

namespace
{
	static constexpr unsigned int maxCommentLength = 500;
	static constexpr unsigned int maxStringLiteralLength = 300;
	static constexpr unsigned int maxNumberLength = 45;
	static constexpr unsigned int maxIdentifierLength = 45;

	const std::unordered_map<Lexer::ErrorType, std::string> errorsMessages =
	{
		{Lexer::ErrorType::IntegerOverflow, "Number would fall out of the range of the integer"},
		{Lexer::ErrorType::FloatOverflow, "Number would fall out of the range of the float"},
		{Lexer::ErrorType::NumberTooLong, "Number too long.Max number length : " + std::to_string(maxNumberLength) + "."},
		{Lexer::ErrorType::IdentifierTooLong, "Identifier too long.Max identifier length : " + std::to_string(maxIdentifierLength) + "."},
		{Lexer::ErrorType::CommentTooLong, "Comment too long. Max comment length: " + std::to_string(maxCommentLength) + "."},
		{Lexer::ErrorType::StringLiteralTooLong, "String literal too long. Max string literal length: " + std::to_string(maxStringLiteralLength) + "."},
		{Lexer::ErrorType::InvalidNumber, "Invalid number format - leading zeros."},
		{Lexer::ErrorType::InvalidEscapeSequence, "Unrecognized character escape sequence."},
		{Lexer::ErrorType::IncompleteStringLiteral, "Incomplete string literal."},
		{Lexer::ErrorType::UnrecognizedSymbol, "Incomplete string literal."}
	};

	const std::unordered_map<std::wstring, Lexer::TokenType> keywords =
	{
		{ L"mut",		Lexer::TokenType::Mut },
		{ L"var",		Lexer::TokenType::Var },
		{ L"while",		Lexer::TokenType::While },
		{ L"if",		Lexer::TokenType::If },
		{ L"else",		Lexer::TokenType::Else },
		{ L"return",	Lexer::TokenType::Return },
		{ L"func",		Lexer::TokenType::Func },
		{ L"true",		Lexer::TokenType::Boolean },
		{ L"false",		Lexer::TokenType::Boolean }
	};

	const std::unordered_map<std::wstring, Lexer::TokenType> symbols =
	{
		{ L";", Lexer::TokenType::Semicolon },
		{ L",", Lexer::TokenType::Comma },
		{ L"{", Lexer::TokenType::LBracket },
		{ L"}", Lexer::TokenType::RBracket },
		{ L"(", Lexer::TokenType::LParenth },
		{ L")", Lexer::TokenType::RParenth },
		{ L"=", Lexer::TokenType::Assign },
		{ L"+", Lexer::TokenType::Plus },
		{ L"-", Lexer::TokenType::Minus },
		{ L"*", Lexer::TokenType::Asterisk },
		{ L"/", Lexer::TokenType::Slash },
		{ L"!", Lexer::TokenType::LogicalNot },
		{ L"<", Lexer::TokenType::Less },
		{ L">", Lexer::TokenType::Greater },
	};

	const std::unordered_map<std::wstring, Lexer::TokenType> twoCharsOperators =
	{
		{ L"&&", Lexer::TokenType::LogicalAnd },
		{ L"||", Lexer::TokenType::LogicalOr },
		{ L"==", Lexer::TokenType::Equal },
		{ L"!=", Lexer::TokenType::NotEqual },
		{ L"<=", Lexer::TokenType::LessEqual },
		{ L">=", Lexer::TokenType::GreaterEqual },
		{ L"+=", Lexer::TokenType::PlusAssign },
		{ L"-=", Lexer::TokenType::MinusAssign },
		{ L"*=", Lexer::TokenType::AsteriskAssign },
		{ L"/=", Lexer::TokenType::SlashAssign },
		{ L"&=", Lexer::TokenType::AndAssign },
		{ L"|=", Lexer::TokenType::OrAssign },
		{ L"<<", Lexer::TokenType::FunctionBind },
		{ L">>", Lexer::TokenType::FunctionCompose },
	};
}

Lexer::LexicalError::LexicalError(const Lexer::ErrorType type, const Lexer::Position position, bool terminating) noexcept :
	type(type), position(position), terminating(terminating)
{
	message = errorsMessages.at(type);
}

Lexer::Lexer(std::wistream* const source) noexcept :
	source(source)
{}


std::pair<std::vector<Lexer::Token>, std::vector<Lexer::LexicalError>> Lexer::ResolveAllRemaining()
{
	std::vector<Lexer::Token> resolvedTokens;
	std::vector<Lexer::LexicalError> foundErrors;
	while(resolvedTokens.empty() || resolvedTokens.back().type != TokenType::EndOfFile)
	{
		const auto next = ResolveNext();
		resolvedTokens.push_back(next.first);
		if (!next.second.empty())
		{
			foundErrors.insert(foundErrors.end(), next.second.begin(), next.second.end());
			if (foundErrors.back().terminating)
			{
				return { resolvedTokens, foundErrors };
			}
		}
	}

	return { resolvedTokens, foundErrors };
}

std::pair<Lexer::Token, std::vector<Lexer::LexicalError>> Lexer::ResolveNext()
{
	currentErrors.clear();

	while (source->get(currentChar))
	{
		if (std::isspace(currentChar))
		{
			if (currentChar == L'\n')
			{
				currentPosition.line++;
				currentPosition.column = 1;
			}
			else
			{
				currentPosition.column++;
			}
			continue;
		}

		return { BuildToken(), currentErrors };
	}

	return { Token(TokenType::EndOfFile, currentPosition), currentErrors };
}

std::optional<Lexer::Token> Lexer::TryBuildComment()
{
	if (currentChar != L'#')
	{
		return std::nullopt;
	}

	unsigned int commentLength = 0;
	while (source->get(currentChar) && currentChar != L'\n')
	{
		commentLength++;
		if (commentLength > maxCommentLength)
		{
			currentErrors.push_back(LexicalError(ErrorType::CommentTooLong, currentPosition, true));
			return Token(TokenType::Comment, currentPosition);
		}
	};
	const auto token = Token(TokenType::Comment, currentPosition);
	currentPosition.line++;
	currentPosition.column = 1;
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildNumber()
{
	if (!std::iswdigit(currentChar))
	{
		return std::nullopt;
	}

	std::wstring numberStr{ currentChar };
	bool isFloat = false;

	while (source->get(currentChar))
	{
		if (std::iswdigit(currentChar))
		{
			numberStr += currentChar;
			if (numberStr.length() > maxNumberLength)
			{
				currentErrors.push_back(LexicalError(ErrorType::NumberTooLong, currentPosition, true));
				const auto token = Token(TokenType::Unrecognized, currentPosition, numberStr);
				currentPosition.column += numberStr.length();
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
				source->unget();
				break;
			}
		}
	}

	if (numberStr.length() > 1 && numberStr[0] == L'0' && numberStr[1] != L'.')
	{
		currentErrors.push_back(LexicalError(ErrorType::InvalidNumber, currentPosition));
		const auto token = Token(TokenType::Unrecognized, currentPosition, numberStr);
		currentPosition.column += numberStr.length();
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
			currentErrors.push_back(LexicalError(ErrorType::FloatOverflow, currentPosition));
			const auto token = Token(TokenType::Unrecognized, currentPosition, numberStr);
			currentPosition.column += numberStr.length();
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
			currentErrors.push_back(LexicalError(ErrorType::IntegerOverflow, currentPosition));
			const auto token = Token(TokenType::Unrecognized, currentPosition, numberStr);
			currentPosition.column += numberStr.length();
			return token;
		}
	}

	const auto token = Token(tokenType, currentPosition, tokenValue);
	currentPosition.column += numberStr.length();
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildWord()
{
	if (!std::iswalpha(currentChar) && currentChar != L'_')
	{
		return std::nullopt;
	}

	std::wstring word{ currentChar };
	while (source->get(currentChar) && (std::iswalnum(currentChar) || currentChar == L'_'))
	{
		word += currentChar;
		if (word.length() > maxIdentifierLength)
		{
			currentErrors.push_back(LexicalError(ErrorType::IdentifierTooLong, currentPosition, true));
			return Token(TokenType::Unrecognized, currentPosition);
		}
	}
	source->unget();

	TokenType tokenType;
	std::variant<std::wstring, int, float, bool> tokenValue = false;
	if (const auto tokenTypeOpt = FindTokenInMap(word, keywords))
	{
		tokenType = tokenTypeOpt.value();
		if (tokenType == TokenType::Boolean)
		{
			tokenValue = (word == L"true");
		}
	}
	else
	{
		tokenType = TokenType::Identifier;
		tokenValue = word;
	}
	const auto token = Token(tokenType, currentPosition, tokenValue);
	currentPosition.column += word.length();
	return token;
}

std::optional<Lexer::Token> Lexer::TryBuildSingleSymbol()
{
	auto symbol = std::wstring{ currentChar };
	if (const auto tokenType = FindTokenInMap({ currentChar }, symbols))
	{
		const auto token = Token(tokenType.value(), currentPosition);
		currentPosition.column++;
		return token;
	}

	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildTwoCharsOperator()
{
	wchar_t nextChar;
	if (source->get(nextChar))
	{
		if (const auto tokenType = FindTokenInMap({ currentChar, nextChar }, twoCharsOperators))
		{
			const auto token = Token(tokenType.value(), currentPosition);
			currentPosition.column += 2;
			return token;
		}
	}
	source->unget();
	return std::nullopt;
}

std::optional<Lexer::Token> Lexer::TryBuildSymbolsMix()
{
	if (std::optional<Token> token = TryBuildTwoCharsOperator())
	{
		return token.value();
	}
	else
	{
		return TryBuildSingleSymbol();
	}
}

std::optional<Lexer::Token> Lexer::TryBuildStringLiteral()
{
	if (currentChar != L'"')
	{
		return std::nullopt;
	}

	std::wstring builtString;
	builtString += currentChar;

	wchar_t nextChar;
	while (source->get(nextChar))
	{
		builtString += nextChar;
		if (builtString.length() > maxStringLiteralLength)
		{
			currentErrors.push_back(LexicalError(ErrorType::StringLiteralTooLong, currentPosition, true));
			return Token(TokenType::Unrecognized, currentPosition);
		}
		if (nextChar == L'"')
		{
			const auto token = Token(TokenType::String, currentPosition, builtString);
			currentPosition.column += builtString.length() + 2;
			return token;
		}
		if (nextChar == L'\\')
		{
			if (source->get(nextChar))
			{
				builtString += nextChar;

				static constexpr std::array<wchar_t, 4> handledEscapedChars = { L'"',  L'\\', L'n', L't' };
				if (std::find(handledEscapedChars.begin(), handledEscapedChars.end(), nextChar) == handledEscapedChars.end())
				{
					currentErrors.push_back(LexicalError(ErrorType::InvalidEscapeSequence, Position(currentPosition.line, currentPosition.column + builtString.length())));
				}
			}
			else
			{
				break;
			}
		}
	}
	currentErrors.push_back(LexicalError(ErrorType::IncompleteStringLiteral, currentPosition));
	const auto token = Token(TokenType::Unrecognized, currentPosition, builtString);
	currentPosition.column += builtString.length() + 1;
	return token;
}

Lexer::Token Lexer::BuildToken()
{
	std::optional<Token> token;
	if (token = TryBuildComment())
	{
		return token.value();
	}
	if (token = TryBuildNumber())
	{
		return token.value();
	}
	if (token = TryBuildWord())
	{
		return token.value();
	}
	if (token = TryBuildSymbolsMix())
	{
		return token.value();
	}
	if (token = TryBuildStringLiteral())
	{
		return token.value();
	}

	currentErrors.push_back(LexicalError(ErrorType::UnrecognizedSymbol, currentPosition));
	token = Token(TokenType::Unrecognized, currentPosition, std::wstring{ currentChar });
	currentPosition.column++;
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