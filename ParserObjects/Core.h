#pragma once
#include <string>
#include <memory>
#include <vector>

struct Param
{
	Param() = default;
	Param(const std::wstring& identifier, bool paramMutable = false) noexcept :
		identifier(identifier), paramMutable(paramMutable) {}
	bool paramMutable = false;
	std::wstring identifier;
};

struct FunctionDefiniton
{
	std::wstring identifier;
	std::vector<std::unique_ptr<Param>> parameters;
	std::unique_ptr<struct Block> block;
};

struct Program
{
	std::vector<std::unique_ptr<FunctionDefiniton>> funDefs;
};