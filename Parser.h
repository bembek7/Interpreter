#pragma once
#include <istream>
#include "Lexer.h"
#include "ParserObjects/ParserObjects.h"
#include <queue>
#include <sstream>
class Parser
{
	class ParserException : public std::exception {
	private:
		std::string message;
		Position position;

	public:
		ParserException(const char* msg, const Position pos)
			: message(msg), position(pos)
		{
		}

		const char* what() const throw()
		{
			std::stringstream ss;
			ss << "Parser Error [line: " << position.line << ", column : " << position.column << "] " << message << std::endl;
			return ss.str().c_str();
		}
	};
public:
	Parser(Lexer* const lexer);
	void SetLexer(Lexer* const newLexer);

	std::unique_ptr<Program> ParseProgram();

private:
	LexToken GetNextToken();
	std::optional<LexToken> GetExpectedToken(const LexToken::TokenType expectedToken);
	bool ConsumeToken(const LexToken::TokenType expectedToken, std::optional<LexToken> boundTokenToReset = std::nullopt);
	bool CheckToken(const LexToken::TokenType expectedToken);

	std::unique_ptr<FunctionDefiniton> ParseFunctionDefinition();
	std::vector<Param> ParseParams();
	std::optional<Param> ParseParam();

	std::unique_ptr<Block> ParseBlock();
	std::unique_ptr<Statement> ParseStatement();
	std::unique_ptr<FunctionCallStatement> ParseFunctionCallStatement();
	std::unique_ptr<Conditional> ParseConditional();
	std::unique_ptr<WhileLoop> ParseLoop();
	std::unique_ptr<Return> ParseReturn();
	std::unique_ptr<Declaration> ParseDeclaration();
	std::unique_ptr<Assignment> ParseAssignment();
	std::vector<std::unique_ptr<Expression>> ParseArguments();

	std::unique_ptr<FunctionCall> ParseFunctionCall();

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

private:
	std::queue<LexToken> unusedTokens = {};
	Lexer* lexer = nullptr;
	Position currentPosition = Position(0, 0);
};