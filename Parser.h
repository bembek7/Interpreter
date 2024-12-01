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

	struct Block : Statement
	{
		Block(std::vector<std::unique_ptr<Statement>> statements) noexcept :
			statements(std::move(statements)) {}
		std::vector<std::unique_ptr<Statement>> statements;
	};

	struct FunctionCall : Statement
	{
		FunctionCall(const std::wstring& identifier, const std::vector<Expression>& arguments) noexcept :
			identifier(identifier), arguments(arguments) {}
		std::wstring identifier;
		std::vector<Expression> arguments;
	};

	struct Conditional : Statement
	{
		Expression condition;
		std::unique_ptr<Block> ifBlock;
		std::optional<std::unique_ptr<Block>> elseBlock;
	};

	struct WhileLoop : Statement
	{
		Expression condition;
		std::unique_ptr<Block> block;
	};

	struct Return : Statement
	{
		Return(const std::optional<Expression>& expression) noexcept :
			expression(expression) {}
		std::optional<Expression> expression;
	};

	struct Declaration : Statement
	{
		bool varMutable = false;
		std::wstring identifier;
		std::optional<Expression> expression;
	};

	struct Assignment : Statement
	{
		Assignment(const std::wstring& identifier, const Expression& expression) noexcept :
			identifier(identifier), expression(expression) {}

		std::wstring identifier;
		Expression expression;
	};

	Program ParseProgram();

private:
	LexToken GetNextToken();
	bool ConsumeToken(const LexToken::TokenType expectedToken) noexcept;
	std::optional<FunctionDefiniton> ParseFunctionDefinition();
	std::vector<Param> ParseParams();
	std::optional<Param> ParseParam();

	std::unique_ptr<Block> ParseBlock();
	std::unique_ptr<Statement> ParseStatement();
	std::unique_ptr<FunctionCall> ParseFunctionCall();
	std::unique_ptr<Conditional> ParseConditional();
	std::unique_ptr<WhileLoop> ParseLoop();
	std::unique_ptr<Return> ParseReturn();
	std::unique_ptr<Declaration> ParseDeclaration();
	std::unique_ptr<Assignment> ParseAssignment();

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