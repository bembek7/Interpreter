#pragma once
#include <memory>
#include <string>
#include <vector>
#include <variant>

struct Param;
struct Block;
struct FunctionCall;
struct Expression;
struct FuncExpression;

struct FunctionLit
{
	std::vector<Param> parameters;
	std::unique_ptr<Block> block;
};

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