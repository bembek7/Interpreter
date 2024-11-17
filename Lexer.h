#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

class Lexer // maybe make it a singleton???
{
	friend class LexerTest; // just for easier testing
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

	struct Token
	{
		Token(const TokenType type, const size_t line, const size_t column, const std::variant<std::wstring, int, float, bool>& value = false) noexcept :
			type(type), line(line), column(column), value(value) {}
		TokenType type;
		size_t line;
		size_t column;
		std::variant<std::wstring, int, float, bool> value;
	};

	enum class ErrorType
	{
		IntegerOverflow,
		FloatOverflow,
		FloatTooLong,
		integerTooLong,
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
		LexicalError(const ErrorType type, const std::string& message,
			const size_t line, const size_t column, bool terminating = false) noexcept :
			type(type), message(message), line(line), column(column), terminating(terminating) {}
		ErrorType type;
		std::string message;
		size_t line;
		size_t column;
		bool terminating = false;
	};

	std::pair<std::vector<Token>, std::vector<LexicalError>> Tokenize(std::wistream& source);

private:
	Token BuildToken(std::wistream& source);

	std::optional<Token> TryBuildComment(std::wistream& source);
	std::optional<Token> TryBuildNumber(std::wistream& source);
	std::optional<Token> TryBuildWord(std::wistream& source);
	std::optional<Token> TryBuildSymbol();
	std::optional<Token> TryBuildSingleCharOperator();
	std::optional<Token> TryBuildTwoCharsOperator(std::wistream& source);
	std::optional<Token> TryBuildStringLiteral(std::wistream& source);
	std::optional<Token> TryBuildOperator(std::wistream& source);

	static std::optional<TokenType> FindTokenInMap(const std::wstring& key, const std::unordered_map<std::wstring, TokenType>& map) noexcept;

private:
	// For code tidyness the tokenization needed variables are stored as class member values
	std::vector<LexicalError> foundErrors;
	wchar_t currentChar = {};
	size_t currentLine = 1;
	size_t currentColumn = 1;

	static constexpr unsigned int maxCommentLength = 500;
	static constexpr unsigned int maxStringLiteralLength = 300;
	static constexpr unsigned int maxNumberLength = 45;
	static constexpr unsigned int maxIdentifierLength = 45;

	static const std::unordered_map<std::wstring, TokenType> keywords;

	static const std::unordered_map<std::wstring, TokenType> symbols;

	static const std::unordered_map<std::wstring, TokenType> singleCharOperators;

	static const std::unordered_map<std::wstring, TokenType> twoCharsOperators;
};