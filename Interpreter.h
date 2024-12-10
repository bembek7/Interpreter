#pragma once
#include "ParserObjects/ParserObjects.h"

class Interpreter
{
public:
	void Interpret(const Program* const program);

private:
	void InterpretFunDef(const FunctionDefiniton* const funDef);

private:
	std::vector<std::wstring> knownFunctions;
};
