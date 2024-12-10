#pragma once
#include<variant>
#include<memory>
#include<string>
#include<optional>

struct FuncExpression;
struct StandardExpression;
struct FunctionCall;

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

struct Factor
{
	Factor() = default;

	Factor(const Literal& literal, bool logicallyNegated = false)
		: factor(literal), logicallyNegated(logicallyNegated) {}

	Factor(std::unique_ptr<StandardExpression> expression, bool logicallyNegated = false)
		: factor(std::move(expression)), logicallyNegated(logicallyNegated) {}

	Factor(std::unique_ptr<FunctionCall> functionCall, bool logicallyNegated = false)
		: factor(std::move(functionCall)), logicallyNegated(logicallyNegated) {}

	Factor(const std::wstring& string, bool logicallyNegated = false)
		: factor(string), logicallyNegated(logicallyNegated) {}

	bool logicallyNegated = false;
	std::variant<std::wstring, Literal, std::unique_ptr<StandardExpression>, std::unique_ptr<FunctionCall>> factor;
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

struct StandardExpression
{
	StandardExpression() = default;
	StandardExpression(std::vector<std::unique_ptr<Conjunction>> conjunctions) noexcept :
		conjunctions(std::move(conjunctions)) {}
	std::vector<std::unique_ptr<Conjunction>> conjunctions;
};

struct Expression
{
	Expression() = default;
	Expression(std::unique_ptr<FuncExpression> funcExpression) noexcept :
		expression(std::move(funcExpression)) {}
	Expression(std::unique_ptr<StandardExpression> stdExpression) noexcept :
		expression(std::move(stdExpression)) {}
	std::variant<std::unique_ptr<FuncExpression>, std::unique_ptr<StandardExpression>> expression;
};
