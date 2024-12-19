#pragma once
#include "ParserObjects/ParserObjects.h"
#include "Value.h"

class Interpreter
{
public:
	struct Variable
	{
		Variable(const bool isMutable, const std::wstring& identifier, std::optional<Value> value = std::nullopt) noexcept :
			isMutable(isMutable), identifier(identifier), value(value){}
		bool isMutable;
		std::wstring identifier;
		std::optional<Value> value = std::nullopt;
	};
	struct Scope
	{
		std::vector<Variable> variables;

		std::shared_ptr<Scope> higherScope;
		Variable* GetVariable(const std::wstring& identifier) noexcept;
		bool VariableAlreadyExists(const std::wstring& identifier) const noexcept;
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

	Value EvaluateStandardExpression(const StandardExpression* const expression);

private:
	void InterpretFunDef(const FunctionDefiniton* const funDef, bool valueExpected, std::vector<Value> arguments = {});

	void Print(const std::wstring& msg) const noexcept;
	void InterpretFunctionCall(const FunctionCall* const functionCall, const bool valueExpected);
	const FunctionDefiniton* GetFunctionDefintion(const std::wstring& identifier)const noexcept;
	Value EvaluateExpression(const Expression* const expression);
	Value EvaluateConjunction(const Conjunction* const conjunction);
	Value EvaluateRelation(const Relation* const relation);
	Value EvaluateAdditive(const Additive* const additive);
	Value EvaluateMultiplicative(const Multiplicative* const multiplicative);
	Value EvaluateFactor(const Factor* const factor);
	Value EvaluateLiteral(const Literal& literal);

private:
	unsigned int currentDepth = 0;
	bool valueExpectedInCurrentFunction;
	std::optional<Value> lastReturnedValue = std::nullopt;
	std::shared_ptr<Scope> currentScope;
	std::vector<const FunctionDefiniton*> knownFunctions;
};
