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
		Number,
		String,
		Operator,
		Delimiter,
		Comment,
		EndOfFile,
		Error,
		Test
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
};