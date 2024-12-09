#pragma once

#include<variant>
#include<memory>
#include<vector>
#include<string>
#include<optional>

struct Statement
{
	virtual ~Statement() = default;
};

struct Block;
struct Param;
struct FunctionLit
{
	std::vector<std::unique_ptr<Param>> parameters;
	std::unique_ptr<Block> block;
};

struct FunctionCall;
struct FuncExpression;
struct Bindable
{
	Bindable(std::unique_ptr<FunctionLit> bindable) :
		bindable(std::move(bindable)) {}
	Bindable(std::unique_ptr<FuncExpression> bindable) :
		bindable(std::move(bindable)) {}
	Bindable(std::unique_ptr<FunctionCall> bindable) :
		bindable(std::move(bindable)) {}
	Bindable(const std::wstring& bindable) :
		bindable(bindable) {}
	std::variant<std::unique_ptr<FunctionLit>, std::unique_ptr<FuncExpression>, std::unique_ptr<FunctionCall>, std::wstring> bindable;
};

struct Expression;
struct Composable
{
	std::unique_ptr<Bindable> bindable;
	std::vector<std::unique_ptr<Expression>> arguments;
};

struct FuncExpression
{
	FuncExpression(std::vector<std::unique_ptr<Composable>> composables = {}) :
		composables(std::move(composables)) {}
	std::vector<std::unique_ptr<Composable>> composables;
};

struct Literal
{
	Literal(const std::variant<bool, int, float, std::wstring>& value) noexcept : value(value) {}
	std::variant<bool, int, float, std::wstring> value;
};

enum class MultiplicationOperator
{
	Multiple,
	Divide
};

struct Expression;
struct FunctionCall;
struct Factor
{
	Factor() = default;

	Factor(std::unique_ptr<Literal> literal, bool logicallyNegated = false)
		: factor(std::move(literal)), logicallyNegated(logicallyNegated) {}

	Factor(std::unique_ptr<Expression> expression, bool logicallyNegated = false)
		: factor(std::move(expression)), logicallyNegated(logicallyNegated) {}

	Factor(std::unique_ptr<FunctionCall> functionCall, bool logicallyNegated = false)
		: factor(std::move(functionCall)), logicallyNegated(logicallyNegated) {}

	Factor(std::wstring string, bool logicallyNegated = false)
		: factor(std::move(string)), logicallyNegated(logicallyNegated) {}

	bool logicallyNegated = false;
	std::variant<std::unique_ptr<Literal>, std::unique_ptr<Expression>, std::unique_ptr<FunctionCall>, std::wstring> factor;
};

struct Multiplicative
{
	Multiplicative() = default;
	Multiplicative(std::vector<std::unique_ptr<Factor>> factors, std::vector<MultiplicationOperator> operators = {}) :
		factors(std::move(factors)), operators(operators) {}
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
	Additive() = default;
	Additive(std::vector<std::unique_ptr<Multiplicative>> multiplicatives, std::vector<AdditionOperator> operators = {}, const bool negated = false) :
		multiplicatives(std::move(multiplicatives)), operators(operators), negated(negated) {}
	bool negated = false;
	std::vector<std::unique_ptr<Multiplicative>> multiplicatives;
	std::vector<AdditionOperator> operators;
};

enum class RelationOperator
{
	Equal,
	NotEqual,
	Greater,
	GreaterEqual,
	Less,
	LessEqual,
};

struct Relation
{
	Relation() = default;
	Relation(std::unique_ptr<Additive> firstAdditive, const std::optional<RelationOperator>& relationOperator = std::nullopt, std::unique_ptr<Additive> secondAdditive = nullptr) noexcept :
		firstAdditive(std::move(firstAdditive)), relationOperator(relationOperator), secondAdditive(std::move(secondAdditive)) {}
	std::unique_ptr<Additive> firstAdditive;
	std::optional<RelationOperator> relationOperator;
	std::unique_ptr<Additive> secondAdditive;
};

struct Conjunction
{
	Conjunction() = default;
	Conjunction(std::vector<std::unique_ptr<Relation>> relations) noexcept :
		relations(std::move(relations)) {}
	std::vector<std::unique_ptr<Relation>> relations;
};

struct Expression
{
	Expression() = default;
	Expression(std::unique_ptr<FuncExpression> funcExpression) noexcept :
		expression(std::move(funcExpression)) {}
	Expression(std::vector<std::unique_ptr<Conjunction>> conjunctions) noexcept :
		expression(std::move(conjunctions)) {}
	std::variant<std::unique_ptr<FuncExpression>, std::vector<std::unique_ptr<Conjunction>>> expression;
};

struct Param
{
	Param() = default;
	Param(const std::wstring& identifier, bool paramMutable = false) noexcept :
		identifier(identifier), paramMutable(paramMutable) {}
	bool paramMutable = false;
	std::wstring identifier;
};

struct Block : Statement
{
	Block(std::vector<std::unique_ptr<Statement>> statements = {}) noexcept :
		statements(std::move(statements)) {}
	std::vector<std::unique_ptr<Statement>> statements;
};

struct FunctionCall : Statement
{
	FunctionCall(const std::wstring& identifier, std::vector<std::unique_ptr<Expression>> arguments = {}) noexcept :
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
	Return(std::unique_ptr<Expression> expression = nullptr) noexcept :
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