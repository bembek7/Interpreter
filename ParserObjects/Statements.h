#pragma once
#include <memory>
#include <string>
#include <vector>

struct Expression;

struct Statement
{
	virtual ~Statement() = default;
};

struct Block : Statement
{
	Block(std::vector<std::unique_ptr<Statement>> statements = {}) noexcept :
		statements(std::move(statements)) {}
	std::vector<std::unique_ptr<Statement>> statements;
};

struct FunctionCall
{
	FunctionCall(const std::wstring& identifier, std::vector<std::unique_ptr<Expression>> arguments = {}) noexcept :
		identifier(identifier), arguments(std::move(arguments)) {}
	std::wstring identifier;
	std::vector<std::unique_ptr<Expression>> arguments;
};

struct FunctionCallStatement : Statement
{
	FunctionCallStatement(std::unique_ptr<FunctionCall> funcCall) noexcept :
		funcCall(std::move(funcCall)) {}
	std::unique_ptr<FunctionCall> funcCall;
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
