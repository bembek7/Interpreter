#pragma once
#include <istream>
#include "Lexer.h"
#include "ParserObjects/ParserObjects.h"
#include <queue>
#include <sstream>

class ParserImpl
{
public:
	class ParserException : public std::runtime_error {
	private:
		std::string message;
		Position position;

	public:
		ParserException(const char* msg, const Position pos)
			: std::runtime_error(msg), position(pos)
		{
			std::stringstream ss;
			ss << "Parser Error [line: " << position.line << ", column : " << position.column << "] " << msg << std::endl;
			message = ss.str();
		}

		const char* what() const noexcept override
		{
			return message.c_str();
		}
	};

	ParserImpl(Lexer* const lexer);
	void SetLexer(Lexer* const newLexer);

	std::unique_ptr<Program> ParseProgram();

	//private:
	LexToken GetTokenFromLexer();
	LexToken GetNextToken();
	std::optional<LexToken> GetExpectedToken(const LexToken::TokenType expectedToken);
	bool ConsumeToken(const LexToken::TokenType expectedToken);
	bool CheckToken(const LexToken::TokenType expectedToken);

	std::unique_ptr<FunctionDefiniton> ParseFunctionDefinition();
	std::vector<Param> ParseParams();
	std::optional<Param> ParseParam();

	std::unique_ptr<Block> ParseBlock();
	std::unique_ptr<Statement> ParseStatement();
	std::unique_ptr<Conditional> ParseConditional();
	std::unique_ptr<WhileLoop> ParseLoop();
	std::unique_ptr<Return> ParseReturn();
	std::unique_ptr<Declaration> ParseDeclaration();
	std::unique_ptr<Assignment> ParseRestOfAssignment(const std::wstring& identifier);
	std::vector<std::unique_ptr<Expression>> ParseArguments();
	std::unique_ptr<FunctionCallStatement> ParseRestOfFunctionCallStatement(const std::wstring& identifier);
	std::unique_ptr<FunctionCall> ParseRestOfFunctionCall(const std::wstring& identifier);

	std::unique_ptr<Expression> ParseExpression();
	std::unique_ptr<StandardExpression> ParseStandardExpression();
	std::unique_ptr<Conjunction> ParseConjunction();
	std::unique_ptr<Relation> ParseRelation();
	std::unique_ptr<Multiplicative> ParseMultiplicative();
	std::unique_ptr<Additive> ParseAdditive();
	std::unique_ptr<Factor> ParseFactor();
	std::optional<Literal> ParseLiteral();

	std::unique_ptr<FuncExpression> ParseFuncExpression();
	std::unique_ptr<Composable> ParseComposable();
	std::unique_ptr<Bindable> ParseBindable();
	std::unique_ptr<FunctionLiteral> ParseFunctionLit();

	//private:
	std::optional<LexToken> lastUnusedToken;
	Lexer* lexer = nullptr;
	Position currentPosition = Position(0, 0);
};