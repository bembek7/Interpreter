#pragma once
#include "ParserObjects/ParserObjects.h"
#include "Value.h"
#include <stack>
#include "Position.h"
#include <sstream>

class Interpreter
{
	class InterpreterException : public std::runtime_error {
	private:
		std::string message;
		Position position;

	public:
		InterpreterException(const char* msg, const Position pos)
			: std::runtime_error(msg), position(pos)
		{
			std::stringstream ss;
			ss << "Interpreter Error [line: " << position.line << ", column : " << position.column << "] " << msg << std::endl;
			message = ss.str();
		}

		const char* what() const noexcept override
		{
			return message.c_str();
		}
	};
public:
	struct Variable
	{
		Variable(const bool isMutable, const std::wstring& identifier, std::optional<Value> value = std::nullopt) noexcept :
			isMutable(isMutable), identifier(identifier), value(value) {
		}
		bool isMutable;
		std::wstring identifier;
		std::optional<Value> value = std::nullopt;
	};
	struct Scope
	{
		std::vector<Variable> variables;

		bool valueExpectedInCurrentFunction = false;
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
	void InterpretFunDef(const FunctionDefiniton* const funDef, std::vector<Value> arguments = {});

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
	std::optional<Value> lastReturnedValue = std::nullopt;
	std::shared_ptr<Scope> currentScope;
	std::stack<std::shared_ptr<Scope>> previousScopes;
	std::vector<const FunctionDefiniton*> knownFunctions;
	Position currentPosition;
};
