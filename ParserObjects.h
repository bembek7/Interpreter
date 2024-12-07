#pragma once

#include<variant>
#include<memory>
#include<vector>
#include<string>
#include<optional>

template<typename T>
bool CompareVectorUniqs(const std::vector<std::unique_ptr<T>>& fv, const std::vector<std::unique_ptr<T>>& sv)
{
	if (fv.size() != sv.size())
	{
		return false;
	}
	for (size_t i = 0; i < fv.size(); ++i)
	{
		if (*fv[i] != *sv[i])
		{
			return false;
		}
	}

	return true;
}

template<typename T>
bool CompareNullableUniqs(const std::unique_ptr<T>& fu, const std::unique_ptr<T>& su)
{
	return ((fu == nullptr && fu == su) || *fu == *su);
}

struct Statement
{
	virtual ~Statement() = default;
	friend bool operator==(const Statement&, const Statement&);
protected:
	virtual bool isEqual(const Statement& obj) const { return true; }
};

struct FuncExpression { bool b = false; };
struct Composable { bool b = false; };
struct Bindable { bool b = false; };
struct FunctionLit { bool b = false; };

struct Literal
{
	Literal(const std::variant<bool, int, float, std::wstring>& value) noexcept : value(value) {}
	std::variant<bool, int, float, std::wstring> value;
	friend bool operator==(const Literal&, const Literal&);
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

	friend bool operator==(const Factor&, const Factor&);
};

struct Multiplicative
{
	Multiplicative() = default;
	Multiplicative(std::vector<std::unique_ptr<Factor>> factors, std::vector<MultiplicationOperator> operators = {}) :
		factors(std::move(factors)), operators(operators) {}
	std::vector<std::unique_ptr<Factor>> factors;
	std::vector<MultiplicationOperator> operators;
	friend bool operator==(const Multiplicative&, const Multiplicative&);
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
	friend bool operator==(const Additive&, const Additive&);
};

enum class RelationOperator
{
	Equal,
};

struct Relation
{
	Relation() = default;
	Relation(std::unique_ptr<Additive> firstAdditive, const std::optional<RelationOperator>& relationOperator = std::nullopt, std::unique_ptr<Additive> secondAdditive = nullptr) noexcept :
		firstAdditive(std::move(firstAdditive)), relationOperator(relationOperator), secondAdditive(std::move(secondAdditive)) {}
	std::unique_ptr<Additive> firstAdditive;
	std::optional<RelationOperator> relationOperator;
	std::unique_ptr<Additive> secondAdditive;
	friend bool operator==(const Relation&, const Relation&);
};

struct Conjunction
{
	Conjunction() = default;
	Conjunction(std::vector<std::unique_ptr<Relation>> relations) noexcept :
		relations(std::move(relations)) {}
	std::vector<std::unique_ptr<Relation>> relations;
	friend bool operator==(const Conjunction&, const Conjunction&);
};

struct Expression
{
	Expression() = default;
	Expression(std::vector<std::unique_ptr<Conjunction>> conjunctions) noexcept :
		conjunctions(std::move(conjunctions)) {}
	std::vector<std::unique_ptr<Conjunction>> conjunctions;
	friend bool operator==(const Expression&, const Expression&);
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
	Block(std::vector<std::unique_ptr<Statement>> statements) noexcept :
		statements(std::move(statements)) {}
	std::vector<std::unique_ptr<Statement>> statements;
protected:
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Block&>(obj);
		bool blocksEqual = CompareVectorUniqs(statements, casted.statements);
		return Statement::isEqual(casted) && blocksEqual;
	}
};

struct FunctionCall : Statement
{
	FunctionCall(const std::wstring& identifier, std::vector<std::unique_ptr<Expression>> arguments) noexcept :
		identifier(identifier), arguments(std::move(arguments)) {}
	std::wstring identifier;
	std::vector<std::unique_ptr<Expression>> arguments;
protected:
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const FunctionCall&>(obj);
		bool argumentsEqual = CompareVectorUniqs(arguments, casted.arguments);
		return Statement::isEqual(casted) && casted.identifier == identifier && argumentsEqual;
	}
};

struct Conditional : Statement
{
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Block> ifBlock;
	std::unique_ptr<Block> elseBlock;
protected:
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Conditional&>(obj);
		return Statement::isEqual(casted) && *casted.ifBlock == *ifBlock
			&& CompareNullableUniqs(casted.elseBlock, elseBlock) && *casted.condition == *condition;
	}
};

struct WhileLoop : Statement
{
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Block> block;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const WhileLoop&>(obj);
		return Statement::isEqual(casted) && *casted.block == *block && *casted.condition == *condition;
	}
};

struct Return : Statement
{
	Return(std::unique_ptr<Expression> expression) noexcept :
		expression(std::move(expression)) {}
	std::unique_ptr<Expression> expression;
protected:
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Return&>(obj);
		return Statement::isEqual(casted) && CompareNullableUniqs(casted.expression, expression);
	}
};

struct Declaration : Statement
{
	bool varMutable = false;
	std::wstring identifier;
	std::unique_ptr<Expression> expression;
protected:
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Declaration&>(obj);
		return Statement::isEqual(casted) && casted.varMutable == varMutable
			&& casted.identifier == identifier && CompareNullableUniqs(casted.expression, expression);
	}
};

struct Assignment : Statement
{
	Assignment(const std::wstring& identifier, std::unique_ptr<Expression> expression) noexcept :
		identifier(identifier), expression(std::move(expression)) {}

	std::wstring identifier;
	std::unique_ptr<Expression> expression;
protected:
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Assignment&>(obj);
		return Statement::isEqual(casted) && casted.identifier == identifier && *casted.expression == *expression;
	}
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