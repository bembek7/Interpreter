#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>

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
		unsigned int line;
		unsigned int column;
	};

	enum class ErrorType
	{
		Overflow,
		InvalidConversion,
		TooLong,
		InvalidNumber,
	};

	struct LexicalError
	{
		ErrorType type;
		std::string message;
		unsigned int line;
		unsigned int column;
		bool terminating = false;
	};

	std::pair<std::vector<Token>, std::vector<LexicalError>> Tokenize(std::wistream& source) const;

private:
	Token BuildToken(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;

	std::optional<Token> TryBuildComment(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;
	std::optional<Token> TryBuildNumber(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;
	std::optional<Token> TryBuildKeywordOrIdentifier(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const; // violates the one purpose rule, but saves code repetition
	std::optional<Token> TryBuildDelimiter(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;
	std::optional<Token> TryBuildSingleCharOperator(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;
	std::optional<Token> TryBuildTwoCharsOperator(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;
	std::optional<Token> TryBuildStringLiteral(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;
	std::optional<Token> TryBuildOperator(std::vector<LexicalError>& errors, wchar_t currentChar, std::wistream& source, unsigned int& line, unsigned int& column) const;

private:
	static constexpr unsigned int maxCommentLength = 300;
	static constexpr unsigned int maxIntegerLength = 10;

	const std::vector<std::wstring> keywords = { L"mut", L"var", L"while", L"if", L"else", L"return", L"func", L"true", L"false" };
	const std::vector<std::wstring> singleCharOperators =
	{
		L"=",  L"+",  L"-",  L"*",  L"/",  L"!", L"<",  L">",
	};
	const std::vector<std::wstring> twoCharsOperators =
	{
		L"&&", L"||", L"==", L"+=", L"-=", L"*=", L"/=", L"!=", L"&=", L"|=", L"<=", L">="
	};
};