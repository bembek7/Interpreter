#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

class Lexer
{
public:
	Lexer(std::wistream* const  source) noexcept; // can load and analyze only one source

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

	struct Position
	{
		Position(const size_t line, const size_t column) noexcept :
			line(line), column(column) {}
		size_t line;
		size_t column;
	};

	struct Token
	{
		Token(const TokenType type, const Position position, const std::variant<std::wstring, int, float, bool>& value = false) noexcept :
			type(type), position(position), value(value) {}
		TokenType type;
		Position position;
		std::variant<std::wstring, int, float, bool> value;
	};

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
		std::string message;
		Position position;
		bool terminating = false;
	};

	std::pair<std::vector<Token>, std::vector<LexicalError>> ResolveAllRemaining();
	std::pair<Token, std::vector<LexicalError>> ResolveNext();
private:
	Token BuildToken();

	std::optional<Token> TryBuildComment();
	std::optional<Token> TryBuildNumber();
	std::optional<Token> TryBuildWord();
	std::optional<Token> TryBuildSymbolsMix();
	std::optional<Token> TryBuildSingleSymbol();
	std::optional<Token> TryBuildTwoCharsOperator();
	std::optional<Token> TryBuildStringLiteral();

	static std::optional<TokenType> FindTokenInMap(const std::wstring& key, const std::unordered_map<std::wstring, TokenType>& map) noexcept;

private:
	std::wistream* source = nullptr;
	std::vector<LexicalError> currentErrors;
	wchar_t currentChar = {};
	Position currentPosition = { 1, 1 };
};