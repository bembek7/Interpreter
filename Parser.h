#pragma once
#include <istream>
#include "Lexer.h"

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

	struct Statement
	{
		virtual ~Statement() = default;
	};

	struct Literal { bool b = false; };
	struct FuncExpression { bool b = false; };
	struct Composable { bool b = false; };
	struct Bindable { bool b = false; };
	struct FunctionLit { bool b = false; };

	struct Factor
	{
		bool logicallyNegated = false;
		std::variant<std::unique_ptr<Literal>, std::wstring> factor;
	};

	enum class MultiplicationOperator
	{
		Multiple,
		Divide
	};

	struct Multiplicative
	{
		std::vector<std::unique_ptr<Factor>> factors;
		std::vector<MultiplicationOperator> operators;
	};

	enum class AdditionOperator
	{
		Plus,
		Minus
	};

	struct Additive
	{
		bool negated = false;
		std::vector<std::unique_ptr<Multiplicative>> multiplicatives;
		std::vector<AdditionOperator> operators;
	};

	enum class RelationOperator
	{
		Equal,
	};

	struct Relation
	{
		std::unique_ptr<Additive> firstAdditive;
		std::optional<RelationOperator> relationOperator;
		std::unique_ptr<Additive> secondAdditive;
	};

	struct Conjunction
	{
		std::vector<std::unique_ptr<Relation>> relations;
	};

	struct Expression
	{
		std::vector<std::unique_ptr<Conjunction>> conjunctions;
	};

	struct Param
	{
		bool paramMutable = false;
		std::wstring identifier;
	};

	struct Block : Statement
	{
		Block(std::vector<std::unique_ptr<Statement>> statements) noexcept :
			statements(std::move(statements)) {}
		std::vector<std::unique_ptr<Statement>> statements;
	};

	struct FunctionCall : Statement
	{
		FunctionCall(const std::wstring& identifier, std::vector<std::unique_ptr<Expression>> arguments) noexcept :
			identifier(identifier), arguments(std::move(arguments)) {}
		std::wstring identifier;
		std::vector<std::unique_ptr<Expression>> arguments;
	};

	struct Conditional : Statement
	{
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Block> ifBlock;
		std::unique_ptr<Block> elseBlock;
	};

	struct WhileLoop : Statement
	{
		std::unique_ptr<Expression> condition;
		std::unique_ptr<Block> block;
	};

	struct Return : Statement
	{
		Return(std::unique_ptr<Expression> expression) noexcept :
			expression(std::move(expression)) {}
		std::unique_ptr<Expression> expression;
	};

	struct Declaration : Statement
	{
		bool varMutable = false;
		std::wstring identifier;
		std::unique_ptr<Expression> expression;
	};

	struct Assignment : Statement
	{
		Assignment(const std::wstring& identifier, std::unique_ptr<Expression> expression) noexcept :
			identifier(identifier), expression(std::move(expression)) {}

		std::wstring identifier;
		std::unique_ptr<Expression> expression;
	};

	struct FunctionDefiniton
	{
		std::wstring identifier;
		std::vector<std::unique_ptr<Param>> parameters;
		std::unique_ptr<Block> block;
	};

	struct Program
	{
		std::vector<std::unique_ptr<FunctionDefiniton>> funDefs;
	};

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