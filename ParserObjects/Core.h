#pragma once
#include <string>
#include <memory>
#include <vector>
#include "../Position.h"

struct Param
{
	Param() = default;
	Param(const std::wstring& identifier, bool paramMutable = false) noexcept :
		identifier(identifier), paramMutable(paramMutable) {}
	bool paramMutable = false;
	std::wstring identifier;
	Position startingPosition = Position(0, 0);
};

struct FunctionDefiniton
{
	std::wstring identifier;
	std::vector<Param> parameters;
	std::unique_ptr<struct Block> block;
	Position startingPosition = Position(0, 0);
};

struct Program
{
	std::vector<std::unique_ptr<FunctionDefiniton>> funDefs;
};