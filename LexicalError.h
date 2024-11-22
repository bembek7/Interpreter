#pragma once
#include "Position.h"
#include <string>

class LexicalError
{
public:
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

	LexicalError(const ErrorType type, const Position position, bool terminating = false) noexcept;

	ErrorType GetType() const noexcept;
	Position GetPosition() const noexcept;
	std::string GetMessage() const noexcept;
	bool IsTerminating() const noexcept;

private:
	ErrorType type;
	Position position;
	std::string message;
	bool terminating = false;
};