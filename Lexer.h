#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include "Position.h"
#include "LexToken.h"

class Lexer
{
public:
	Lexer(std::wistream* const  source) noexcept; // can load and analyze only one source

	enum class ErrorType
	{
		IntegerOverflow,
		FloatOverflow,
		NumberTooLong,
		IdentifierTooLong,
		CommentTooLong,
		StringLiteralTooLong,
		InvalidNumber,
		InvalidEscapeSequence,
		IncompleteStringLiteral,
		UnrecognizedSymbol
	};

	struct LexicalError
	{
		LexicalError(const ErrorType type, const Position position, bool terminating = false) noexcept;
		ErrorType type;
		Position position;
		std::string message;
		bool terminating = false;
	};

	std::pair<std::vector<LexToken>, std::vector<LexicalError>> ResolveAllRemaining();
	std::pair<LexToken, std::vector<LexicalError>> ResolveNext();
private:
	LexToken BuildToken();

	std::optional<LexToken> TryBuildComment();
	std::optional<LexToken> TryBuildNumber();
	std::optional<LexToken> TryBuildWord();
	std::optional<LexToken> TryBuildSymbolsMix();
	std::optional<LexToken> TryBuildSingleSymbol();
	std::optional<LexToken> TryBuildTwoCharsOperator();
	std::optional<LexToken> TryBuildStringLiteral();

private:
	std::wistream* source = nullptr;
	std::vector<LexicalError> currentErrors;
	wchar_t currentChar = {};
	Position currentPosition = { 1, 1 };
};