#pragma once
#include "ParserObjects/ParserObjects.h"

class Interpreter
{
public:
	struct Variable
	{
		Variable(const bool isMutable, const std::wstring& identifier) noexcept:
			isMutable(isMutable), identifier(identifier) {}
		bool isMutable;
		std::wstring identifier;
	};
	struct Scope
	{
		std::vector<Variable> variables;

		std::shared_ptr<Scope> higherScope;
	};

public:
	void Interpret(const Program* const program);

	void InterpretBlock(const Block* const block);
	void InterpretStatement(const FunctionCallStatement* const block);
	void InterpretStatement(const WhileLoop* const block);
	void InterpretStatement(const Return* const block);
	void InterpretStatement(const Conditional* const block);
	void InterpretStatement(const Declaration* const block);
	void InterpretStatement(const Assignment* const block);
private:
	void InterpretFunDef(const FunctionDefiniton* const funDef);
	void Print(const std::wstring& msg) const noexcept;

private:
	unsigned int currentDepth = 0;
	std::shared_ptr<Scope> currentScope;
	std::vector<std::wstring> knownFunctions;
};
