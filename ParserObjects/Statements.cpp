#include "Statements.h"
#include "../Interpreter.h"

void Block::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretBlock(this);
}

void FunctionCallStatement::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretStatement(this);
}

void Conditional::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretStatement(this);
}

void WhileLoop::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretStatement(this);
}

void Return::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretStatement(this);
}

void Declaration::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretStatement(this);
}

void Assignment::InterpretThis(Interpreter& interpreter) const
{
	interpreter.InterpretStatement(this);
}