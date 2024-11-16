#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

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
		OrAssign
	};

	struct Token
	{
		Token(const TokenType type, const std::variant<std::wstring, int, float, bool>& value, const size_t line, const size_t column) :
			type(type), value(value), line(line), column(column) {}
		TokenType type;
		std::variant<std::wstring, int, float, bool> value;
		size_t line;
		size_t column;
	};

	enum class ErrorType
	{
		Overflow,
		TooLong,
		InvalidNumber,
		InvalidEscapeSequence,
		IncompleteStringLiteral
	};

	struct LexicalError
	{
		LexicalError(const ErrorType type, const std::string& message,
			const size_t line, const size_t column, bool terminating = false) :
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
	std::optional<Token> TryBuildKeywordOrIdentifier(std::wistream& source); // violates the one purpose rule, but saves code repetition
	std::optional<Token> TryBuildSymbol();
	std::optional<Token> TryBuildSingleCharOperator(std::wistream& source);
	std::optional<Token> TryBuildTwoCharsOperator(std::wistream& source);
	std::optional<Token> TryBuildStringLiteral(std::wistream& source);
	std::optional<Token> TryBuildOperator(std::wistream& source);

private:
	// For code tidyness the tokenization needed variables are stored as class member values
	std::vector<LexicalError> foundErrors;
	wchar_t currentChar = {};
	size_t currentLine = 1;
	size_t currentColumn = 1;

	static constexpr unsigned int maxCommentLength = 500;
	static constexpr unsigned int maxStringLiteralLength = 300;
	static constexpr unsigned int maxIntegerLength = 10;

	const std::vector<std::wstring> keywords = { L"mut", L"var", L"while", L"if", L"else", L"return", L"func", L"true", L"false" };

	const std::unordered_map<std::wstring, TokenType> symbols =
	{
		{ L";", TokenType::Semicolon },
		{ L",", TokenType::Comma },
		{ L"{", TokenType::LBracket },
		{ L"}", TokenType::RBracket },
		{ L"(", TokenType::LParenth },
		{ L")", TokenType::RParenth },
	};

	const std::unordered_map<std::wstring, TokenType> singleCharOperators =
	{
		{ L"=", TokenType::Assign },
		{ L"+", TokenType::Plus },
		{ L"-", TokenType::Minus },
		{ L"*", TokenType::Asterisk },
		{ L"/", TokenType::Slash },
		{ L"!", TokenType::LogicalNot },
		{ L"<", TokenType::Less },
		{ L">", TokenType::Greater },
	};

	const std::unordered_map<std::wstring, TokenType> twoCharsOperators =
	{
		{ L"&&", TokenType::LogicalAnd },
		{ L"||", TokenType::LogicalOr },
		{ L"==", TokenType::Equal },
		{ L"!=", TokenType::NotEqual },
		{ L"<=", TokenType::LessEqual },
		{ L">=", TokenType::GreaterEqual },
		{ L"+=", TokenType::PlusAssign },
		{ L"-=", TokenType::MinusAssign },
		{ L"*=", TokenType::AsteriskAssign },
		{ L"/=", TokenType::SlashAssign },
		{ L"&=", TokenType::AndAssign },
		{ L"|=", TokenType::OrAssign },		
	};
};