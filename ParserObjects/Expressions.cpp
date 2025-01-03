#include "Expressions.h"
#include "../Interpreter.h"
#include "../Value.h"

Value StandardExpression::EvaluateThis(Interpreter& interpreter) const
{
	return interpreter.EvaluateStandardExpression(this);
}

Value FuncExpression::EvaluateThis(Interpreter& interpreter) const
{
	return interpreter.EvaluateFuncExpression(this);
}