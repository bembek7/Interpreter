#include <gtest/gtest.h>

#include <sstream>
#include "Parser.h"

class ParserTest : public ::testing::Test
{
};

static void CompareParams(const Param* const param, const Param* const expectedParam)
{
	EXPECT_EQ(param->paramMutable, expectedParam->paramMutable);
	EXPECT_EQ(param->identifier, expectedParam->identifier);
}

static void CompareFunDefs(const FunctionDefiniton* const funDef, const FunctionDefiniton* const expectedFunDef)
{
	EXPECT_EQ(funDef->identifier, expectedFunDef->identifier);
	ASSERT_EQ(funDef->parameters.size(), expectedFunDef->parameters.size());

	for (size_t i = 0; i < funDef->parameters.size(); ++i)
	{
		CompareParams(funDef->parameters[i].get(), expectedFunDef->parameters[i].get());
	}

	EXPECT_EQ(*funDef->block, *expectedFunDef->block);
}

static void ComparePrograms(const Program& program, const Program& expectedProgram)
{
	ASSERT_EQ(program.funDefs.size(), expectedProgram.funDefs.size());

	for (size_t i = 0; i < program.funDefs.size(); ++i)
	{
		CompareFunDefs(program.funDefs[i].get(), expectedProgram.funDefs[i].get());
	}
}

TEST_F(ParserTest, SimpleMainFunction)
{
	std::wstringstream input(L"func main()\n{\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	std::vector<std::unique_ptr<FunctionDefiniton>> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"main";
	exFunDef->block = std::make_unique<Block>(std::vector<std::unique_ptr<Statement>>{});

	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionWithParameters)
{
	std::wstringstream input(L"func Fizz(mut a, c, mut d)\n{\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	std::vector<std::unique_ptr<FunctionDefiniton>> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->parameters.push_back(std::make_unique<Param>(L"a", true));
	exFunDef->parameters.push_back(std::make_unique<Param>(L"c"));
	exFunDef->parameters.push_back(std::make_unique<Param>(L"d", true));
	exFunDef->block = std::make_unique<Block>(std::vector<std::unique_ptr<Statement>>{});
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}