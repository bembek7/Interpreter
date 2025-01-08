#pragma once
#include "ParserImpl.h"

class Parser : private ParserImpl
{
public:
	Parser(Lexer* const lexer)
		: ParserImpl(lexer)
	{
	}
	std::unique_ptr<Program> ParseProgram()
	{
		return ParserImpl::ParseProgram();
	}
};