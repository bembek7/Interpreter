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

void Interpreter::Print(const std::wstring& msg) const noexcept
{
	std::wstring tabulation(currentDepth, L'\t');
	std::wcout << tabulation << msg << std::endl;
}

void Interpreter::InterpretBlock(const Block* const block)
{
	++currentDepth;
	for (const auto& statement : block->statements)
	{
		statement->InterpretThis(*this);
	}
	--currentDepth;
}

void Interpreter::InterpretStatement(const FunctionCallStatement* const functionCallStatement)
{
	Print(L"FunctionCallStatement");
}

void Interpreter::InterpretStatement(const WhileLoop* const whileLoop)
{
	Print(L"While");
}

void Interpreter::InterpretStatement(const Return* const returnStatement)
{
	Print(L"Return");
}

void Interpreter::InterpretStatement(const Conditional* const conditional)
{
	Print(L"Conditional");
}

void Interpreter::InterpretStatement(const Declaration* const declaration)
{
	Print(L"Declaration");
}

void Interpreter::InterpretStatement(const Assignment* const assignment)
{
	Print(L"Assignment");
}

void Interpreter::Interpret(const Program* const program)
{
	for (const auto& funDef : program->funDefs)
	{
		InterpretFunDef(funDef.get());
	}
}