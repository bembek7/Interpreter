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
		std::variant<bool, int, float, std::wstring> value;
	};
	struct Scope
	{
		std::vector<Variable> variables;

		std::shared_ptr<Scope> higherScope;
		Variable* GetVariable(const std::wstring& identifier) noexcept;
		bool VariableAlreadyExists(const std::wstring& identifier) const noexcept;
	};

	struct Val
	{
		bool b;
	};
public:
	void Interpret(const Program* const program);

	void InterpretBlock(const Block* const block);

	void InterpretFunctionCallStatement(const FunctionCallStatement* const functionCallStatement);

	void InterpretWhileLoop(const WhileLoop* const whileLoop);
	void InterpretReturn(const Return* const returnStatement);
	void InterpretConditional(const Conditional* const conditional);
	void InterpretDeclaration(const Declaration* const declaration);
	void InterpretAssignment(const Assignment* const assignment);
private:
	void InterpretFunDef(const FunctionDefiniton* const funDef, bool valueExpected, std::vector<Val> arguments = {});
	
	void Print(const std::wstring& msg) const noexcept;
	void InterpretFunctionCall(const FunctionCall* const functionCall, const bool valueExpected);
	const FunctionDefiniton* GetFunctionDefintion(const std::wstring& identifier)const noexcept;
	std::variant<bool, int, float, std::wstring> EvaluateExpression(const Expression* const expression);

private:
	unsigned int currentDepth = 0;
	bool valueExpectedInCurrentFunction;
	std::shared_ptr<Scope> currentScope;
	std::vector<const FunctionDefiniton*> knownFunctions;
};
