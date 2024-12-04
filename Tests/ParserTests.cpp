#include <gtest/gtest.h>

#include <sstream>
#include "Parser.h"

class ParserTest : public ::testing::Test
{
};

static void CompareStatements(const Parser::Statement* const param, const Parser::Statement* const expectedParam)
{

}

static void CompareParams(const Parser::Param* const param, const Parser::Param* const expectedParam)
{
	EXPECT_EQ(param->paramMutable, expectedParam->paramMutable);
	EXPECT_EQ(param->identifier, expectedParam->identifier);
}

static void CompareBlocks(const Parser::Block* const block, const Parser::Block* const expectedBlock)
{
	ASSERT_EQ(block->statements.size(), expectedBlock->statements.size());

	for (size_t i = 0; i < block->statements.size(); ++i)
	{
		CompareStatements(block->statements[i].get(), expectedBlock->statements[i].get());
	}
}

static void CompareFunDefs(const Parser::FunctionDefiniton* const funDef, const Parser::FunctionDefiniton* const expectedFunDef)
{
	EXPECT_EQ(funDef->identifier, expectedFunDef->identifier);
	ASSERT_EQ(funDef->parameters.size(), expectedFunDef->parameters.size());

	for (size_t i = 0; i < funDef->parameters.size(); ++i)
	{
		CompareParams(funDef->parameters[i].get(), expectedFunDef->parameters[i].get());
	}
}

static void ComparePrograms(const Parser::Program& program, const Parser::Program& expectedProgram)
{
	ASSERT_EQ(program.funDefs.size(), expectedProgram.funDefs.size());

	for (size_t i = 0; i < program.funDefs.size(); ++i)
	{
		CompareFunDefs(program.funDefs[i].get(), expectedProgram.funDefs[i].get());
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

TEST_F(ParserTest, FunctionWithParameters)
{
	using P = Parser;

	std::wstringstream input(L"func Fizz(mut a, c, mut d)\n{\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	P::Program expectedProgram;
	std::vector<std::unique_ptr<P::FunctionDefiniton>> exFunDefs;

	auto exFunDef = std::make_unique<P::FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->parameters.push_back(std::make_unique<P::Param>(L"a", true));
	exFunDef->parameters.push_back(std::make_unique<P::Param>(L"c"));
	exFunDef->parameters.push_back(std::make_unique<P::Param>(L"d", true));

	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}
