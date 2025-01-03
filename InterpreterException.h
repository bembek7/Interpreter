#pragma once
#include "Position.h"
#include <sstream>

class InterpreterException : public std::runtime_error 
{
public:
	InterpreterException(const char* msg, const Position pos);

	const char* what() const noexcept override;

protected:
	InterpreterException(const char* msg);
protected:
	std::string message;
	Position position;
};