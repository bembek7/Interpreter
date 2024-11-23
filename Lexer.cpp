#include "Lexer.h"
#include "LexToken.h"
#include <istream>
#include <cwctype>
#include <sstream>
#include <array>
#include "OverflowChecks.h"
// cannot use peek with wide chars
// could do some better errors throwing to avoid code repetition

#define THROW_NUMBER_ERROR(ERROR_TYPE, NUMBER_LENGTH, IS_FLOAT) \
	const auto errorPosition = currentPosition; \
	const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition); \
	currentPosition.column += NUMBER_LENGTH; \
	const bool skippedSuccessfully = SkipNumber(IS_FLOAT); \
	currentErrors.push_back(LexicalError(ERROR_TYPE, errorPosition, !skippedSuccessfully)); \
	return token; \

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
		++commentLength;
		if (commentLength > maxCommentLength)
		{
			const auto errorPosition = currentPosition;
			const auto token = LexToken(LexToken::TokenType::Comment, currentPosition);
			const bool skippedSuccessfully = SkipComment();
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::CommentTooLong, errorPosition, !skippedSuccessfully));
			return token;
		}
	};
	const auto token = LexToken(LexToken::TokenType::Comment, currentPosition);
	++currentPosition.line;
	currentPosition.column = 1;
	return token;
}

std::optional<LexToken> Lexer::TryBuildNumber()
{
	if (!std::iswdigit(currentChar))
	{
		return std::nullopt;
	}

	bool isFloat = false;
	unsigned int numberLength = 1;
	unsigned int decimalPlace = 0;

	wchar_t zeroChar = L'0';

	int builtValueInt = currentChar - zeroChar;
	bool integerOverflows = false;

	float builtValueFloat = 0.f;

	while (source->get(currentChar))
	{
		if (std::iswdigit(currentChar))
		{
			++numberLength;
			if (numberLength > maxNumberLength)
			{
				THROW_NUMBER_ERROR(LexicalError::ErrorType::NumberTooLong, numberLength, isFloat);
			}

			const int currentDigit = currentChar - zeroChar;

			if (!isFloat)
			{
				if (integerOverflows)
				{
					if (OfC::WillMultiplicationOverflow(builtValueFloat, 10.f))
					{
						THROW_NUMBER_ERROR(LexicalError::ErrorType::FloatOverflow, numberLength, isFloat);
					}
					builtValueFloat *= 10;
					if (OfC::WillAdditionOverflow(builtValueFloat, float(currentDigit)))
					{
						THROW_NUMBER_ERROR(LexicalError::ErrorType::FloatOverflow, numberLength, isFloat);
					}
					builtValueFloat += currentDigit;
				}
				else
				{
					if (OfC::WillMultiplicationOverflow(builtValueInt, 10))
					{
						integerOverflows = true;
						builtValueFloat = float(builtValueInt) * 10;
					}
					else
					{
						builtValueInt *= 10;
					}
					if (!integerOverflows)
					{
						if (OfC::WillAdditionOverflow(builtValueInt, currentDigit))
						{
							integerOverflows = true;
							builtValueFloat = float(builtValueInt) + currentDigit;
						}
						else
						{
							builtValueInt += currentDigit;
						}
					}
				}

				if (numberLength == 2 && builtValueInt == 0)
				{
					THROW_NUMBER_ERROR(LexicalError::ErrorType::InvalidNumber, numberLength, isFloat);
				}
			}
			else
			{
				const float toAdd = currentDigit / float(std::pow(10, decimalPlace));
				if (OfC::WillAdditionOverflow(builtValueFloat, toAdd))
				{
					THROW_NUMBER_ERROR(LexicalError::ErrorType::FloatOverflow, numberLength, isFloat);
				}

				builtValueFloat += toAdd;
				++decimalPlace;
			}
		}
		else
		{
			if (currentChar == L'.' && !isFloat)
			{
				++numberLength;
				isFloat = true;
				builtValueFloat = float(builtValueInt);
				decimalPlace = 1;
			}
			else
			{
				source->unget();
				break;
			}
		}
	}

	LexToken::TokenType tokenType;
	std::variant<std::monostate, std::wstring, int, float, bool> tokenValue;
	if (isFloat)
	{
		tokenType = LexToken::TokenType::Float;
		tokenValue = builtValueFloat;
	}
	else
	{
		if (!integerOverflows)
		{
			tokenValue = builtValueInt;
		}
		else
		{
			THROW_NUMBER_ERROR(LexicalError::ErrorType::IntegerOverflow, numberLength, isFloat);
		}
		tokenType = LexToken::TokenType::Integer;
	}

	const auto token = LexToken(tokenType, currentPosition, tokenValue);
	currentPosition.column += numberLength;
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
			const auto errorPosition = currentPosition;
			const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition);
			currentPosition.column += word.length();
			const bool skippedSuccessfully = SkipIdentifier();
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::IdentifierTooLong, errorPosition, !skippedSuccessfully));
			return token;
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
	unsigned int escapeSequencesAdditionalLength = 0;

	while (source->get(currentChar))
	{
		if (builtString.length() > maxStringLiteralLength)
		{
			const auto errorPosition = currentPosition;
			const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition);
			currentPosition.column += builtString.length() + escapeSequencesAdditionalLength + 2;
			const bool skippedSuccessfully = SkipStringLiteral();
			currentErrors.push_back(LexicalError(LexicalError::ErrorType::StringLiteralTooLong, errorPosition, !skippedSuccessfully));
			return token;
		}
		if (currentChar == L'"')
		{
			const auto token = LexToken(LexToken::TokenType::String, currentPosition, builtString);
			currentPosition.column += builtString.length() + escapeSequencesAdditionalLength + 2;
			return token;
		}
		if (currentChar == L'\\')
		{
			if (source->get(currentChar))
			{
				static constexpr std::array<wchar_t, 4> handledEscapedChars = { L'"',  L'\\', L'n', L't' };
				if (std::find(handledEscapedChars.begin(), handledEscapedChars.end(), currentChar) == handledEscapedChars.end())
				{
					currentErrors.push_back(LexicalError(LexicalError::ErrorType::InvalidEscapeSequence, Position(currentPosition.line, currentPosition.column + builtString.length())));
				}
				else
				{
					escapeSequencesAdditionalLength += 1;
				}
				switch (currentChar)
				{
				case L'n':
					builtString.push_back(L'\n');
					break;
				case L't':
					builtString.push_back(L'\t');
					break;
				default:
					builtString.push_back(currentChar);
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			builtString += currentChar;
		}
	}
	currentErrors.push_back(LexicalError(LexicalError::ErrorType::IncompleteStringLiteral, currentPosition));
	const auto token = LexToken(LexToken::TokenType::Unrecognized, currentPosition);
	currentPosition.column += builtString.length() + escapeSequencesAdditionalLength + 1;
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

bool Lexer::SkipNumber(bool dotOccured)
{
	bool dotOccurred = false;

	static constexpr int maxSafety = 200;
	unsigned int alreadySkipped = 0;

	while (source->get(currentChar))
	{
		++alreadySkipped;
		if (alreadySkipped > maxSafety)
		{
			return false; // Failure
		}
		++currentPosition.column;
		if (std::iswdigit(currentChar))
		{
			continue;
		}
		else
		{
			if (!dotOccured && currentChar == L'.')
			{
				dotOccured = true;
				continue;
			}
		}

		--currentPosition.column;
		source->unget();
		break;
	}

	return true; // Successfully skipped number
}

bool Lexer::SkipComment()
{
	static constexpr int maxSafety = 5000;
	source->ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
	const bool skipSuccessfull = !source->fail() || source->eof();
	if (skipSuccessfull)
	{
		currentPosition.column = 1;
		++currentPosition.line;
	}
	return skipSuccessfull;
}

bool Lexer::SkipStringLiteral()
{
	static constexpr int maxSafety = 5000;
	unsigned int alreadySkipped = 0;
	wchar_t prevChar = 0;

	while (source->get(currentChar))
	{
		++alreadySkipped;
		if (alreadySkipped > maxSafety)
		{
			return false;
		}

		++currentPosition.column;
		if (currentChar == L'"' && prevChar != L'\\')
		{
			return true;
		}

		prevChar = currentChar;
	}
}

bool Lexer::SkipIdentifier()
{
	static constexpr int maxSafety = 200;
	unsigned int alreadySkipped = 0;

	while (source->get(currentChar))
	{
		++alreadySkipped;
		if (alreadySkipped > maxSafety)
		{
			return false;
		}
		++currentPosition.column;

		if (!std::iswalnum(currentChar) && currentChar != L'_')
		{
			--currentPosition.column;
			source->unget();
			return true;
		}
	}
}