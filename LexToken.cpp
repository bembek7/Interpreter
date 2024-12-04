#include "LexToken.h"
#include <stdexcept>
#include <optional>

LexToken::LexToken(const TokenType type, const Position position, const std::monostate value) :
	type(type), position(position), value(value)
{
	switch (type)
	{
	case TokenType::String:
	case TokenType::Identifier:
	case TokenType::Float:
	case TokenType::Integer:
	case TokenType::Boolean:
		throw std::runtime_error("Invalid type passed to a variant for this type of token");
		break;
	default:
		break;
	}
}

LexToken::LexToken(const TokenType type, const Position position, const std::wstring value) :
	type(type), position(position), value(value)
{
	switch (type)
	{
	case TokenType::String:
	case TokenType::Identifier:
	case TokenType::Unrecognized:
		break;
	default:
		throw std::runtime_error("Invalid type passed to a variant for this type of token");
		break;
	}
}

LexToken::LexToken(const TokenType type, const Position position, const int value) :
	type(type), position(position), value(value)
{
	switch (type)
	{
	case TokenType::Integer:
		break;
	default:
		throw std::runtime_error("Invalid type passed to a variant for this type of token");
		break;
	}
}

LexToken::LexToken(const TokenType type, const Position position, const float value) :
	type(type), position(position), value(value)
{
	switch (type)
	{
	case TokenType::Float:
		break;
	default:
		throw std::runtime_error("Invalid type passed to a variant for this type of token");
		break;
	}
}

LexToken::LexToken(const TokenType type, const Position position, const bool value) :
	type(type), position(position), value(value)
{
	switch (type)
	{
	case TokenType::Boolean:
		break;
	default:
		throw std::runtime_error("Invalid type passed to a variant for this type of token");
		break;
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