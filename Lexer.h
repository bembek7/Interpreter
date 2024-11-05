#pragma once
#include <variant>
#include <string>
#include <vector>

class Lexer
{
public:
	enum class TokenType
	{
		Identifier,
		Keyword,
		Integer,
		Float,
		String,
		Boolean,
		Operator,
		Delimiter,
		Comment,
		EndOfFile,
		Unrecognized,
	};

	struct Token
	{
		Token(const TokenType type, const std::variant<std::wstring, int, float, bool>& value, const int line, const int column) :
			type(type), value(value), line(line), column(column) {}
		TokenType type;
		std::variant<std::wstring, int, float, bool> value;
		int line;
		int column;
	};

	std::vector<Token> Tokenize(std::wistream& source) const;
private:
	static constexpr unsigned int maxCommentLength = 300;
	static constexpr unsigned int maxIntegerLength = 10;
};