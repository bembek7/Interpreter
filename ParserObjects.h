#pragma once

#include<variant>
#include<memory>
#include<vector>
#include<string>
#include<optional>

struct Statement
{
	virtual ~Statement() = default;
	friend bool operator==(const Statement&, const Statement&);
	virtual bool isEqual(const Statement& obj) const { return true; }
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
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Block&>(obj);
		bool blocksEqual = (statements.size() == casted.statements.size());
		if (!blocksEqual)
		{
			return false;
		}
		for (size_t i = 0; i < statements.size(); ++i)
		{
			if (*statements[i] != *casted.statements[i])
			{
				return false;
			}
		}
		return Statement::isEqual(casted) && blocksEqual;
	}
};

struct FunctionCall : Statement
{
	FunctionCall(const std::wstring& identifier, std::vector<std::unique_ptr<Expression>> arguments) noexcept :
		identifier(identifier), arguments(std::move(arguments)) {}
	std::wstring identifier;
	std::vector<std::unique_ptr<Expression>> arguments;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const FunctionCall&>(obj);
		//add stuff
		return Statement::isEqual(casted) && casted.identifier == identifier;
	}
};

struct Conditional : Statement
{
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Block> ifBlock;
	std::unique_ptr<Block> elseBlock;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Conditional&>(obj);
		//add stuff
		return Statement::isEqual(casted) && casted.ifBlock == ifBlock
			&& casted.elseBlock == elseBlock;
	}
};

struct WhileLoop : Statement
{
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Block> block;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const WhileLoop&>(obj);
		//add stuff
		return Statement::isEqual(casted) && casted.block == block;
	}
};

struct Return : Statement
{
	Return(std::unique_ptr<Expression> expression) noexcept :
		expression(std::move(expression)) {}
	std::unique_ptr<Expression> expression;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Return&>(obj);
		//add stuff
		return Statement::isEqual(casted);
	}
};

struct Declaration : Statement
{
	bool varMutable = false;
	std::wstring identifier;
	std::unique_ptr<Expression> expression;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Declaration&>(obj);
		//add stuff
		return Statement::isEqual(casted) && casted.varMutable == varMutable
			&& casted.identifier == identifier;
	}
};

struct Assignment : Statement
{
	Assignment(const std::wstring& identifier, std::unique_ptr<Expression> expression) noexcept :
		identifier(identifier), expression(std::move(expression)) {}

	std::wstring identifier;
	std::unique_ptr<Expression> expression;
	virtual bool isEqual(const Statement& obj) const override
	{
		auto& casted = static_cast<const Assignment&>(obj);
		//add stuff
		return Statement::isEqual(casted) && casted.identifier == identifier;
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