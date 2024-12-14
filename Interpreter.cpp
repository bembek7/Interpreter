#include "Interpreter.h"
#include <iostream>

void Interpreter::InterpretFunDef(const FunctionDefiniton* const funDef)
{
	if (std::find(knownFunctions.begin(), knownFunctions.end(), funDef->identifier) != knownFunctions.end())
	{
		// error
	}
	std::wcout << funDef->identifier << std::endl;
	auto newScope = std::make_shared<Scope>();
	newScope->higherScope = currentScope;
	currentScope = std::move(newScope);
	for (const auto& param : funDef->parameters)
	{
		currentScope->variables.push_back({ param.paramMutable, param.identifier });
	}
	InterpretBlock(funDef->block.get());
}

void Interpreter::InterpretBlock(const Block* const block)
{
	std::wcout << L"Block: " << std::endl;
	for (const auto& statement : block->statements)
	{
		statement->InterpretThis(*this);
	}
	std::wcout << L"End of Block: " << std::endl;
}

void Interpreter::InterpretStatement(const FunctionCallStatement* const functionCallStatement)
{
	std::wcout << L"FunctionCallStatement " << std::endl;
}

void Interpreter::InterpretStatement(const WhileLoop* const whileLoop)
{
	std::wcout << L"FunctionCallStatement " << std::endl;
}

void Interpreter::InterpretStatement(const Return* const returnStatement)
{
	std::wcout << L"Return " << std::endl;
}

void Interpreter::InterpretStatement(const Conditional* const conditional)
{
	std::wcout << L"Conditional " << std::endl;
}

void Interpreter::InterpretStatement(const Declaration* const declaration)
{
	std::wcout << L"Declaration " << std::endl;
}

void Interpreter::InterpretStatement(const Assignment* const assignment)
{
	std::wcout << L"Assignment " << std::endl;
}

void Interpreter::Interpret(const Program* const program)
{
	for (const auto& funDef : program->funDefs)
	{
		InterpretFunDef(funDef.get());
	}
}