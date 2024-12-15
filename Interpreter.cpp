#include "Interpreter.h"
#include <iostream>

void Interpreter::Interpret(const Program* const program)
{
	const FunctionDefiniton* mainFunction = nullptr;
	for (const auto& funDef : program->funDefs)
	{
		if (funDef->identifier == L"Main")
		{
			mainFunction = funDef.get();
		}
		knownFunctions.push_back(funDef.get());
		
	}
	if (mainFunction)
	{
		InterpretFunDef(mainFunction, false);
	}
	else
	{
		throw; // error
	}
}

void Interpreter::InterpretFunDef(const FunctionDefiniton* const funDef, bool valueExpected, std::vector<Val> arguments)
{
	Print(L"Function: " + funDef->identifier + L" Arguments: ");
	auto newScope = std::make_shared<Scope>();
	newScope->higherScope = currentScope;
	currentScope = std::move(newScope);
	valueExpectedInCurrentFunction = valueExpected;
	if (funDef->parameters.size() != arguments.size())
	{
		throw; // error
	}
	for (size_t i = 0; i < arguments.size(); ++i)
	{
		currentScope->variables.push_back({ funDef->parameters[i].paramMutable, funDef->parameters[i].identifier /*argumnent*/});
	}

	InterpretBlock(funDef->block.get());
}

void Interpreter::InterpretBlock(const Block* const block)
{
	++currentDepth;
	for (const auto& statement : block->statements)
	{
		statement->InterpretThis(*this);
		if (dynamic_cast<Return*>(statement.get()))
		{
			break;
		}
	}
	--currentDepth;
}

void Interpreter::InterpretFunctionCallStatement(const FunctionCallStatement* const functionCallStatement)
{
	Print(L"FunctionCallStatement");
	InterpretFunctionCall(functionCallStatement->funcCall.get(), false);
}

void Interpreter::InterpretFunctionCall(const FunctionCall* const functionCall, const bool valueExpected)
{
	auto function = GetFunctionDefintion(functionCall->identifier);
	// evaluate artguments
	if (function)
	{
		InterpretFunDef(function, valueExpected /*arguments*/);
	}
	else
	{
		throw;// error
	}
}

void Interpreter::InterpretWhileLoop(const WhileLoop* const whileLoop)
{
	Print(L"While");
	auto conditionExpression = EvaluateExpression(whileLoop->condition.get());
	if (auto condition = std::get_if<bool>(&conditionExpression))
	{
		while(*condition)
		{
			InterpretBlock(whileLoop->block.get());
		}
	}
	else
	{
		throw; // error
	}
}

void Interpreter::InterpretReturn(const Return* const returnStatement)
{
	Print(L"Return");
	if (valueExpectedInCurrentFunction)
	{
		if (returnStatement->expression)
		{
			auto returnValue = EvaluateExpression(returnStatement->expression.get());
		}
		else
		{
			throw; // error
		}
	}
}

void Interpreter::InterpretConditional(const Conditional* const conditional)
{
	Print(L"Conditional");
	auto conditionExpression = EvaluateExpression(conditional->condition.get());
	if (auto condition = std::get_if<bool>(&conditionExpression))
	{
		if (*condition)
		{
			InterpretBlock(conditional->ifBlock.get());
		}
		else
		{
			InterpretBlock(conditional->elseBlock.get());
		}
	}
	else
	{
		throw; // error
	}
}

void Interpreter::InterpretDeclaration(const Declaration* const declaration)
{
	Print(L"Declaration");
	if (currentScope->VariableAlreadyExists(declaration->identifier))
	{
		throw; // error
	}
	currentScope->variables.push_back(Variable(declaration->varMutable, declaration->identifier));
	if (declaration->expression)
	{
		auto value = EvaluateExpression(declaration->expression.get());
		currentScope->variables.back().value = value;
	}
}

void Interpreter::InterpretAssignment(const Assignment* const assignment)
{
	Print(L"Assignment");
	auto variable = currentScope->GetVariable(assignment->identifier);
	if(!variable)
	{
		throw; // error
	}
	variable->value = EvaluateExpression(assignment->expression.get());
}

Interpreter::Variable* Interpreter::Scope::GetVariable(const std::wstring& identifier) noexcept
{
	for (auto& var : variables)
	{
		if (var.identifier == identifier)
		{
			return &var;
		}
	}
	if (higherScope)
	{
		return higherScope->GetVariable(identifier);
	}
	return nullptr;
}

bool Interpreter::Scope::VariableAlreadyExists(const std::wstring& identifier) const noexcept
{
	for (auto& var : variables)
	{
		if (var.identifier == identifier)
		{
			return true;
		}
	}
	if (higherScope)
	{
		return higherScope->VariableAlreadyExists(identifier);
	}
	return false;
}

const FunctionDefiniton* Interpreter::GetFunctionDefintion(const std::wstring& identifier) const noexcept
{
	for (const auto function : knownFunctions)
	{
		if (function->identifier == identifier)
		{
			return function;
		}
	}
	return nullptr;
}

std::variant<bool, int, float, std::wstring> Interpreter::EvaluateExpression(const Expression* const expression)
{
	return false;
}

void Interpreter::Print(const std::wstring& msg) const noexcept
{
	std::wstring tabulation(currentDepth, L'\t');
	std::wcout << tabulation << msg << std::endl;
}