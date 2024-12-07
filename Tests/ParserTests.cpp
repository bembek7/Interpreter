#include <gtest/gtest.h>

#include <sstream>
#include "Parser.h"

template <typename T>
using vecUni = std::vector<std::unique_ptr<T>>;

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

static std::unique_ptr<Expression> MakeExprFromFactor(std::unique_ptr<Factor> factor)
{
	vecUni<Factor> factors;
	factors.push_back(std::move(factor));

	auto multiplicative = std::make_unique<Multiplicative>(std::move(factors));

	vecUni<Multiplicative> multiplicatives;
	multiplicatives.push_back(std::move(multiplicative));
	auto additive = std::make_unique<Additive>(std::move(multiplicatives));

	auto relation = std::make_unique<Relation>(std::move(additive));

	vecUni<Relation> relations;
	relations.push_back(std::move(relation));
	auto conjunction = std::make_unique<Conjunction>(std::move(relations));

	vecUni<Conjunction> conjunctions;
	conjunctions.push_back(std::move(conjunction));
	return std::make_unique<Expression>(std::move(conjunctions));
}

static std::unique_ptr<Expression> MakeExprFromLiteral(std::unique_ptr<Literal> literal)
{
	auto factor = std::make_unique<Factor>(std::move(literal));

	return MakeExprFromFactor(std::move(factor));
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

TEST_F(ParserTest, FunctionWithDeclaration)
{
	std::wstringstream input(L"func Fizz()\n{ mut var a = 10;\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto exDeclaration = std::make_unique<Declaration>();
	exDeclaration->identifier = L"a";
	exDeclaration->varMutable = true;
	exDeclaration->expression = MakeExprFromLiteral(std::make_unique<Literal>(10));

	exFunDef->block->statements.push_back(std::move(exDeclaration));

	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, Conditional)
{
	std::wstringstream input(L"func Fizz()\n{ if(true){}\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto conditional = std::make_unique<Conditional>();
	conditional->condition = MakeExprFromLiteral(std::make_unique<Literal>(true));
	conditional->ifBlock = std::make_unique<Block>();
	exFunDef->block->statements.push_back(std::move(conditional));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, WhileLoop)
{
	std::wstringstream input(L"func Fizz()\n{ while(true){}\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto loop = std::make_unique<WhileLoop>();
	loop->condition = MakeExprFromLiteral(std::make_unique<Literal>(true));
	loop->block = std::make_unique<Block>();
	exFunDef->block->statements.push_back(std::move(loop));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, NestedBlock)
{
	std::wstringstream input(L"func Fizz()\n{{}\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto block = std::make_unique<Block>();
	exFunDef->block->statements.push_back(std::move(block));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionCallWithoutArguments)
{
	std::wstringstream input(L"func Fizz()\n{Buzz();\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto funcCall = std::make_unique<FunctionCall>(std::wstring(L"Buzz"), vecUni<Expression>{});
	exFunDef->block->statements.push_back(std::move(funcCall));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionCallWithArguments)
{
	std::wstringstream input(L"func Fizz()\n{Buzz(10, 5., false, \"string\", a);\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto arguments = vecUni<Expression>{};
	arguments.push_back(MakeExprFromLiteral(std::make_unique<Literal>(10)));
	arguments.push_back(MakeExprFromLiteral(std::make_unique<Literal>(5.f)));
	arguments.push_back(MakeExprFromLiteral(std::make_unique<Literal>(false)));
	arguments.push_back(MakeExprFromLiteral(std::make_unique<Literal>(std::wstring(L"string"))));
	arguments.push_back(MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a"))));
	auto funcCall = std::make_unique<FunctionCall>(std::wstring(L"Buzz"), std::move(arguments));
	exFunDef->block->statements.push_back(std::move(funcCall));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, EmptyReturn)
{
	std::wstringstream input(L"func Fizz()\n{return;\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto returnStatement = std::make_unique<Return>();
	exFunDef->block->statements.push_back(std::move(returnStatement));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, ReturnSomething)
{
	std::wstringstream input(L"func Fizz()\n{return a;\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto returnStatement = std::make_unique<Return>(MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a"))));
	exFunDef->block->statements.push_back(std::move(returnStatement));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}