#include "InterpreterException.h"

InterpreterException::InterpreterException(const char* msg) :
	std::runtime_error(msg), position(0, 0)
{}

InterpreterException::InterpreterException(const char* msg, const Position pos) :
	std::runtime_error(msg), position(pos)
{
	std::stringstream ss;
	ss << "Interpreter Error [line: " << position.line << ", column : " << position.column << "] " << msg << std::endl;
	message = ss.str();
}
const char* InterpreterException::what() const noexcept
{
	return message.c_str();
}