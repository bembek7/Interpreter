#include "LexicalError.h"
#include <unordered_map>

namespace
{
	const std::unordered_map<LexicalError::ErrorType, std::string> errorsMessages =
	{
		{LexicalError::ErrorType::IntegerOverflow, "Number would fall out of the range of the integer"},
		{LexicalError::ErrorType::FloatOverflow, "Number would fall out of the range of the float"},
		{LexicalError::ErrorType::NumberTooLong, "Number too long."},
		{LexicalError::ErrorType::IdentifierTooLong, "Identifier too long.."},
		{LexicalError::ErrorType::CommentTooLong, "Comment too long."},
		{LexicalError::ErrorType::StringLiteralTooLong, "String literal too long. Max string literal length."},
		{LexicalError::ErrorType::InvalidNumber, "Invalid number format - leading zeros."},
		{LexicalError::ErrorType::InvalidEscapeSequence, "Unrecognized character escape sequence."},
		{LexicalError::ErrorType::IncompleteStringLiteral, "Incomplete string literal."},
		{LexicalError::ErrorType::UnrecognizedSymbol, "Incomplete string literal."}
	};
}

LexicalError::LexicalError(const LexicalError::ErrorType type, const Position position, bool terminating) noexcept :
	type(type), position(position), terminating(terminating)
{
	message = errorsMessages.at(type);
}

LexicalError::ErrorType LexicalError::GetType() const noexcept
{
	return type;
}

Position LexicalError::GetPosition() const noexcept
{
	return position;
}

std::string LexicalError::GetMessage() const noexcept
{
	return message;
}

bool LexicalError::IsTerminating() const noexcept
{
	return terminating;
}