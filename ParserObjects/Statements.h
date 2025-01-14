#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Expressions.h"

class Interpreter;

struct Statement
{
	virtual ~Statement() = default;
	virtual void InterpretThis(Interpreter& interpreter) const = 0;
	Position startingPosition = Position(0, 0);
};

struct Block : Statement
{
	Block(std::vector<std::unique_ptr<Statement>> statements = {}) noexcept :
		statements(std::move(statements)) {
	}
	std::vector<std::unique_ptr<Statement>> statements;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};

struct FunctionCall
{
	FunctionCall(const std::wstring& identifier, std::vector<std::unique_ptr<Expression>> arguments = {}) noexcept :
		identifier(identifier), arguments(std::move(arguments)) {
	}
	std::wstring identifier;
	std::vector<std::unique_ptr<Expression>> arguments;
	Position startingPosition = Position(0, 0);
};

struct FunctionCallStatement : Statement
{
	FunctionCallStatement(std::unique_ptr<FunctionCall> funcCall) noexcept :
		funcCall(std::move(funcCall)) {
	}
	std::unique_ptr<FunctionCall> funcCall;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};

struct Conditional : Statement
{
	std::unique_ptr<StandardExpression> condition;
	std::unique_ptr<Block> ifBlock;
	std::unique_ptr<Block> elseBlock;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};

struct WhileLoop : Statement
{
	std::unique_ptr<StandardExpression> condition;
	std::unique_ptr<Block> block;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};

struct Return : Statement
{
	Return(std::unique_ptr<Expression> expression = nullptr) noexcept :
		expression(std::move(expression)) {
	}
	std::unique_ptr<Expression> expression;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};

struct Declaration : Statement
{
	bool varMutable = false;
	std::wstring identifier;
	std::unique_ptr<Expression> expression;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};

struct Assignment : Statement
{
	Assignment(const std::wstring& identifier, std::unique_ptr<Expression> expression) noexcept :
		identifier(identifier), expression(std::move(expression)) {
	}

	std::wstring identifier;
	std::unique_ptr<Expression> expression;
	virtual void InterpretThis(Interpreter& interpreter) const override;
};