#include "Interpreter.h"
#include <iostream>

void Interpreter::InterpretFunDef(const FunctionDefiniton* const funDef)
{
	if (std::find(knownFunctions.begin(), knownFunctions.end(), funDef->identifier) != knownFunctions.end())
	{
		// error
	}
	std::wcout << funDef->identifier << std::endl;
}

void Interpreter::Interpret(const Program* const program)
{
	for (const auto& funDef : program->funDefs)
	{
		InterpretFunDef(funDef.get());
	}
}