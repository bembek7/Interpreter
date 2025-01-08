#include "Statements.h"
#include "../Interpreter.h"

void Block::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretBlock(this);
}

void FunctionCallStatement::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretFunctionCallStatement(this);
}

void Conditional::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretConditional(this);
}

void WhileLoop::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretWhileLoop(this);
}

void Return::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretReturn(this);
}

void Declaration::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretDeclaration(this);
}

void Assignment::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretAssignment(this);
}