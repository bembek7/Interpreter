// Main.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <sstream>
#include "Lexer.h"
#include <unordered_map>
#include <fstream>
#include "PathConfig.h"
#include "Parser.h"
#include "Interpreter.h"

int main()
{
	/*std::string codeExample = R"(
		mut var a;
		a = 10;

		var b = "123";

		var c = a + b;    # c == 133 prioritizing numbers over strings
	)";

	std::istringstream codeStream(codeExample);
	const auto tokens = lexer.Tokenize(codeStream*/

	std::wifstream codeFile(CodesPath::exampleCodesPath + "TestCode3.txt");
	if (!codeFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 1;
	}

	Lexer lexer = Lexer(&codeFile);

	Parser parser = Parser(&lexer);

	auto program = parser.ParseProgram();

	Interpreter interpreter;
	interpreter.Interpret(program.get());

	codeFile.close();
	return 0;
}