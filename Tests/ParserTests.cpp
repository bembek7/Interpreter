#include <gtest/gtest.h>

#include <sstream>
#include "Parser.h"

class ParserTest : public ::testing::Test
{
};

static void ComparePrograms(const Parser::Program& program, const Parser::Program& expectedProgram)
{
	ASSERT_EQ(program.funDefs.size(), expectedProgram.funDefs.size());

	for (size_t i = 0; i < program.funDefs.size(); ++i)
	{
		const auto& funDef = program.funDefs[i];
		const auto& expectedFunDef = expectedProgram.funDefs[i];

		EXPECT_EQ(funDef->identifier, expectedFunDef->identifier);
	}
}

TEST_F(ParserTest, SimpleMainFunction)
{
	using P = Parser;

	std::wstringstream input(L"func main()\n{\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	P::Program expectedProgram;
	std::vector<std::unique_ptr<P::FunctionDefiniton>> exFunDefs;

	auto exFunDef = std::make_unique<P::FunctionDefiniton>();
	exFunDef->identifier = L"main";
	
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}
