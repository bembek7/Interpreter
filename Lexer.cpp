#include "Lexer.h"
#include "Lexer.h"
#include "LexToken.h"
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

	const std::unordered_map<std::wstring, LexToken::TokenType> keywords =
	{
		{ L"mut",		LexToken::TokenType::Mut },
		{ L"var",		LexToken::TokenType::Var },
		{ L"while",		LexToken::TokenType::While },
		{ L"if",		LexToken::TokenType::If },
		{ L"else",		LexToken::TokenType::Else },
		{ L"return",	LexToken::TokenType::Return },
		{ L"func",		LexToken::TokenType::Func },
		{ L"true",		LexToken::TokenType::Boolean },
		{ L"false",		LexToken::TokenType::Boolean }
	};

	const std::unordered_map<std::wstring, LexToken::TokenType> symbols =
	{
		{ L";", LexToken::TokenType::Semicolon },
		{ L",", LexToken::TokenType::Comma },
		{ L"{", LexToken::TokenType::LBracket },
		{ L"}", LexToken::TokenType::RBracket },
		{ L"(", LexToken::TokenType::LParenth },
		{ L")", LexToken::TokenType::RParenth },
		{ L"=", LexToken::TokenType::Assign },
		{ L"+", LexToken::TokenType::Plus },
		{ L"-", LexToken::TokenType::Minus },
		{ L"*", LexToken::TokenType::Asterisk },
		{ L"/", LexToken::TokenType::Slash },
		{ L"!", LexToken::TokenType::LogicalNot },
		{ L"<", LexToken::TokenType::Less },
		{ L">", LexToken::TokenType::Greater },
	};

	const std::unordered_map<std::wstring, LexToken::TokenType> twoCharsOperators =
	{
		{ L"&&", LexToken::TokenType::LogicalAnd },
		{ L"||", LexToken::TokenType::LogicalOr },
		{ L"==", LexToken::TokenType::Equal },
		{ L"!=", LexToken::TokenType::NotEqual },
		{ L"<=", LexToken::TokenType::LessEqual },
		{ L">=", LexToken::TokenType::GreaterEqual },
		{ L"+=", LexToken::TokenType::PlusAssign },
		{ L"-=", LexToken::TokenType::MinusAssign },
		{ L"*=", LexToken::TokenType::AsteriskAssign },
		{ L"/=", LexToken::TokenType::SlashAssign },
		{ L"&=", LexToken::TokenType::AndAssign },
		{ L"|=", LexToken::TokenType::OrAssign },
		{ L"<<", LexToken::TokenType::FunctionBind },
		{ L">>", LexToken::TokenType::FunctionCompose },
	};
}

Lexer::Lexer(std::wistream* const source) noexcept :
	source(source)
{}

void Lexer::SetNewSource(std::wistream* const newSource) noexcept
{
	source = newSource;
	currentPosition.line = 1;
	currentPosition.column = 1;
}

std::pair<std::vector<LexToken>, std::vector<LexicalError>> Lexer::ResolveAllRemaining()
{
	std::vector<LexToken> resolvedTokens;
	std::vector<LexicalError> foundErrors;
	while (resolvedTokens.empty() || resolvedTokens.back().GetType() != LexToken::TokenType::EndOfFile)
	{
		const auto next = ResolveNext();
		resolvedTokens.push_back(next.first);
		if (!next.second.empty())
		{
			foundErrors.insert(foundErrors.end(), next.second.begin(), next.second.end());
			if (foundErrors.back().IsTerminating())
			{
				return { resolvedTokens, foundErrors };
			}
		}
	}

	return { resolvedTokens, foundErrors };
}

std::pair<LexToken, std::vector<LexicalError>> Lexer::ResolveNext()
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

	return { LexToken(LexToken::TokenType::EndOfFile, currentPosition), currentErrors };
}

std::optional<LexToken> Lexer::TryBuildComment()
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
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::CommentTooLong, currentPosition, true));
			return LexToken(LexToken::TokenType::Comment, currentPosition);
		}
	};
	const auto token = LexToken(LexToken::TokenType::Comment, currentPosition);
	currentPosition.line++;
	currentPosition.column = 1;
	return token;
}

std::optional<LexToken> Lexer::TryBuildNumber()
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
				currentErrors.push_back(LexicalError(LexicalError::ErrorType::NumberTooLong, currentPosition, true));
				const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition, numberStr);
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
		currentErrors.push_back(LexicalError(LexicalError::ErrorType::InvalidNumber, currentPosition));
		const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition, numberStr);
		currentPosition.column += numberStr.length();
		return token;
	}

	LexToken::TokenType tokenType;
	std::variant<std::monostate, std::wstring, int, float, bool> tokenValue;
	if (isFloat)
	{
		tokenType = LexToken::TokenType::Float;
		try // I don't think using exceptions here is the perfect solution, but seems to be the simplest one
		{
			tokenValue = std::stof(numberStr);
		}
		catch (std::out_of_range)
		{
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::FloatOverflow, currentPosition));
			const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition, numberStr);
			currentPosition.column += numberStr.length();
			return token;
		}
	}
	else
	{
		tokenType = LexToken::TokenType::Integer;
		try
		{
			tokenValue = std::stoi(numberStr);
		}
		catch (std::out_of_range)
		{
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::IntegerOverflow, currentPosition));
			const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition, numberStr);
			currentPosition.column += numberStr.length();
			return token;
		}
	}

	const auto token = LexToken(tokenType, currentPosition, tokenValue);
	currentPosition.column += numberStr.length();
	return token;
}

std::optional<LexToken> Lexer::TryBuildWord()
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
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::IdentifierTooLong, currentPosition, true));
			return LexToken(LexToken::TokenType::Unrecognized, currentPosition);
		}
	}
	source->unget();

	LexToken::TokenType tokenType;
	std::variant<std::monostate, std::wstring, int, float, bool> tokenValue;
	if (const auto tokenTypeOpt = LexToken::FindTokenInMap(word, keywords))
	{
		tokenType = tokenTypeOpt.value();
		if (tokenType == LexToken::TokenType::Boolean)
		{
			tokenValue = (word == L"true");
		}
	}
	else
	{
		tokenType = LexToken::TokenType::Identifier;
		tokenValue = word;
	}
	const auto token = LexToken(tokenType, currentPosition, tokenValue);
	currentPosition.column += word.length();
	return token;
}

std::optional<LexToken> Lexer::TryBuildSingleSymbol()
{
	auto symbol = std::wstring{ currentChar };
	if (const auto tokenType = LexToken::FindTokenInMap({ currentChar }, symbols))
	{
		const auto token = LexToken(tokenType.value(), currentPosition);
		currentPosition.column++;
		return token;
	}

	return std::nullopt;
}

std::optional<LexToken> Lexer::TryBuildTwoCharsOperator()
{
	wchar_t nextChar;
	if (source->get(nextChar))
	{
		if (const auto tokenType = LexToken::FindTokenInMap({ currentChar, nextChar }, twoCharsOperators))
		{
			const auto token = LexToken(tokenType.value(), currentPosition);
			currentPosition.column += 2;
			return token;
		}
	}
	source->unget();
	return std::nullopt;
}

std::optional<LexToken> Lexer::TryBuildSymbolsMix()
{
	if (std::optional<LexToken> token = TryBuildTwoCharsOperator())
	{
		return token.value();
	}
	else
	{
		return TryBuildSingleSymbol();
	}
}

std::optional<LexToken> Lexer::TryBuildStringLiteral()
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
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::StringLiteralTooLong, currentPosition, true));
			return LexToken(LexToken::TokenType::Unrecognized, currentPosition);
		}
		if (nextChar == L'"')
		{
			const auto token = LexToken(LexToken::TokenType::String, currentPosition, builtString);
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
					currentErrors.push_back(LexicalError(LexicalError::ErrorType::InvalidEscapeSequence, Position(currentPosition.line, currentPosition.column + builtString.length())));
				}
			}
			else
			{
				break;
			}
		}
	}
	currentErrors.push_back(LexicalError(LexicalError::ErrorType::IncompleteStringLiteral, currentPosition));
	const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition, builtString);
	currentPosition.column += builtString.length() + 1;
	return token;
}

LexToken Lexer::BuildToken()
{
	std::optional<LexToken> token;
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

	currentErrors.push_back(LexicalError(LexicalError::ErrorType::UnrecognizedSymbol, currentPosition));
	token = LexToken(LexToken::TokenType::Unrecognized, currentPosition, std::wstring{ currentChar });
	currentPosition.column++;
	return token.value();
}