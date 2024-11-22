#include "LexToken.h"
#include <stdexcept>
#include <optional>

LexToken::LexToken(const TokenType type, const Position position, const std::variant<std::monostate, std::wstring, int, float, bool>& value) :
	type(type), position(position), value(value)
{
	bool invalid = false;
	switch (type)
	{
	case TokenType::String:
	case TokenType::Identifier:
		if (!std::holds_alternative<std::wstring>(value))
		{
			invalid = true;
		}
		break;
	case TokenType::Float:
		if (!std::holds_alternative<float>(value))
		{
			invalid = true;
		}
		break;
	case TokenType::Integer:
		if (!std::holds_alternative<int>(value))
		{
			invalid = true;
		}
		break;
	case TokenType::Boolean:
		if (!std::holds_alternative<bool>(value))
		{
			invalid = true;
		}
		break;
	case TokenType::Unrecognized:
		if (!std::holds_alternative<std::wstring>(value) && !std::holds_alternative<std::monostate>(value))
		{
			invalid = true;
		}
		break;
	default:
		if (!std::holds_alternative<std::monostate>(value))
		{
			invalid = true;
		}
		break;
	}

	if (invalid)
	{
		throw std::runtime_error("Invalid type passed to a variant for this type of token");
	}
}

std::optional<LexToken::TokenType> LexToken::FindTokenInMap(const std::wstring& key, const std::unordered_map<std::wstring, LexToken::TokenType>& map) noexcept
{
	if (const auto it = map.find(key); it != map.end())
	{
		return it->second;
	}
	return std::nullopt;
}

LexToken::TokenType LexToken::GetType() const noexcept
{
	return type;
}

Position LexToken::GetPosition() const noexcept
{
	return position;
}

std::variant<std::monostate, std::wstring, int, float, bool> LexToken::GetValue() const noexcept
{
	return value;
}