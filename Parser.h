#pragma once
#include <istream>
#include "Lexer.h"
#include "ParserObjects.h"

class Parser
{
	class ParserException : public std::exception {
	private:
		std::string message;

	public:
		ParserException(const char* msg)
			: message(msg)
		{}

		const char* what() const throw()
		{
			return message.c_str();
		}
	};
public:
	Parser(Lexer* const lexer) noexcept;
	void SetLexer(Lexer* const newLexer) noexcept;

	Program ParseProgram();

private:
	LexToken GetNextToken();
	std::optional<LexToken> GetExpectedToken(const LexToken::TokenType expectedToken);
	bool ConsumeToken(const LexToken::TokenType expectedToken);
	std::unique_ptr<FunctionDefiniton> ParseFunctionDefinition();
	std::vector<std::unique_ptr<Param>> ParseParams();
	std::unique_ptr<Param> ParseParam();

	std::unique_ptr<Block> ParseBlock();
	std::unique_ptr<Statement> ParseStatement();
	std::unique_ptr<FunctionCall> ParseFunctionCall();
	std::unique_ptr<Conditional> ParseConditional();
	std::unique_ptr<WhileLoop> ParseLoop();
	std::unique_ptr<Return> ParseReturn();
	std::unique_ptr<Declaration> ParseDeclaration();
	std::unique_ptr<Assignment> ParseAssignment();
	std::vector<std::unique_ptr<Expression>> ParseArguments();

	std::unique_ptr<Expression> ParseExpression();
	std::unique_ptr<Conjunction> ParseConjunction();
	std::unique_ptr<Relation> ParseRelation();
	std::unique_ptr<Multiplicative> ParseMultiplicative();
	std::unique_ptr<Additive> ParseAdditive();
	std::unique_ptr<Factor> ParseFactor();
	std::unique_ptr<Literal> ParseLiteral();

	std::optional<FuncExpression> ParseFuncExpression();
	std::optional<Composable> ParseComposable();
	std::optional<Bindable> ParseBindable();
	std::optional<FunctionLit> ParseFunctionLit();

private:
	std::optional<LexToken> lastUnusedToken = std::nullopt;
	Lexer* lexer = nullptr;
};