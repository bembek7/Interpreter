#pragma once
#include "Position.h"
#include <variant>
#include <string>
#include <optional>
#include <unordered_map>

class LexToken
{
public:
	enum class TokenType
	{
		Identifier,
		Integer,
		Float,
		String,
		Boolean,
		Semicolon,
		LParenth,
		RParenth,
		LBracket,
		RBracket,
		Comma,
		Comment,
		EndOfFile,
		Unrecognized,
		Assign,
		Plus,
		Minus,
		Asterisk,
		Slash,
		LogicalNot,
		Less,
		Greater,
		LogicalAnd,
		LogicalOr,
		Equal,
		LessEqual,
		GreaterEqual,
		NotEqual,
		PlusAssign,
		MinusAssign,
		AsteriskAssign,
		SlashAssign,
		AndAssign,
		OrAssign,
		Mut,
		Var,
		While,
		If,
		Else,
		Return,
		Func,
		FunctionBind,
		FunctionCompose
	};

	LexToken(const TokenType type, const Position position, const std::monostate value = std::monostate{});
	LexToken(const TokenType type, const Position position, const std::wstring value);
	LexToken(const TokenType type, const Position position, const int value);
	LexToken(const TokenType type, const Position position, const float value);
	LexToken(const TokenType type, const Position position, const bool value);

	static std::optional<TokenType> FindTokenInMap(const std::wstring& key, const std::unordered_map<std::wstring, LexToken::TokenType>& map) noexcept;

	TokenType GetType() const noexcept;
	Position GetPosition() const noexcept;
	std::variant<std::monostate, std::wstring, int, float, bool> GetValue() const noexcept;

private:
	TokenType type;
	Position position;
	std::variant<std::monostate, std::wstring, int, float, bool> value;
};