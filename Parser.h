#pragma once
#include <istream>
#include "Lexer.h"

class Parser
{
public:
	Parser(Lexer* const lexer) noexcept;
	void SetLexer(Lexer* const newLexer) noexcept;

	struct FunctionDefiniton { bool b = false; };
	struct Program { bool b = false; };
	struct Param { bool b = false; };
	struct Statement { bool b = false; };
	struct FunctionCall { bool b = false; };
	struct Conditional { bool b = false; };
	struct Loop { bool b = false; };
	struct Return : public Loop { bool b = false; };
	struct Declaration { bool b = false; };
	struct Assignment { bool b = false; };
	struct Expression { bool b = false; };
	struct Conjunction { bool b = false; };
	struct Relation { bool b = false; };
	struct Multiplicative { bool b = false; };
	struct Additive { bool b = false; };
	struct Factor { bool b = false; };
	struct Literal { bool b = false; };
	struct FuncExpression { bool b = false; };
	struct Composable { bool b = false; };
	struct Bindable { bool b = false; };
	struct FunctionLit { bool b = false; };

private:
	Program ParseProgram();
	LexToken GetNextToken();
	bool ConsumeToken(const LexToken::TokenType expectedToken) noexcept;
	std::optional<FunctionDefiniton> ParseFunctionDefinition();
	std::vector<Param> ParseParams();
	std::optional<Param> ParseParam();
	std::vector<Statement> ParseBlock();
	std::optional<Statement> ParseStatement();
	std::optional<FunctionCall> ParseFunctionCall();
	std::optional<Conditional> ParseConditional();
	std::optional<Loop> ParseLoop();
	std::optional<Return> ParseReturn();
	std::optional<Declaration> ParseDeclaration();
	std::optional<Assignment> ParseAssignment();
	std::optional<std::vector<Expression>> ParseArguments();
	std::optional<Expression> ParseExpression();
	std::optional<Conjunction> ParseConjunction();
	std::optional<Relation> ParseRelation();
	std::optional<Multiplicative> ParseMultiplicative();
	std::optional<Additive> ParseAdditive();
	std::optional<Factor> ParseFactor();
	std::optional<Literal> ParseLiteral();
	std::optional<FuncExpression> ParseFuncExpression();
	std::optional<Composable> ParseComposable();
	std::optional<Bindable> ParseBindable();
	std::optional<FunctionLit> ParseFunctionLit();

private:
	Lexer* lexer = nullptr;
};