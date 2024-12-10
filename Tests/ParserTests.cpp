#include <gtest/gtest.h>

#include <sstream>
#include "Parser.h"
#include "ComparePrograms.h"

template <typename T>
using vecUni = std::vector<std::unique_ptr<T>>;

class ParserTest : public ::testing::Test
{
};

static std::unique_ptr<Multiplicative> MakeMultiplicativeFromFactor(std::unique_ptr<Factor> factor)
{
	vecUni<Factor> factors;
	factors.push_back(std::move(factor));

	return std::make_unique<Multiplicative>(std::move(factors));
}

static std::unique_ptr<StandardExpression> MakeExprFromFactor(std::unique_ptr<Factor> factor)
{
	vecUni<Multiplicative> multiplicatives;
	multiplicatives.push_back(MakeMultiplicativeFromFactor(std::move(factor)));
	auto additive = std::make_unique<Additive>(std::move(multiplicatives));

	auto relation = std::make_unique<Relation>(std::move(additive));

	vecUni<Relation> relations;
	relations.push_back(std::move(relation));
	auto conjunction = std::make_unique<Conjunction>(std::move(relations));

	vecUni<Conjunction> conjunctions;
	conjunctions.push_back(std::move(conjunction));
	return std::make_unique<StandardExpression>(std::move(conjunctions));
}

static std::unique_ptr<StandardExpression> MakeExprFromLiteral(const Literal& literal)
{
	auto factor = std::make_unique<Factor>(literal);

	return MakeExprFromFactor(std::move(factor));
}

static std::unique_ptr<Composable> MakeComposableFromString(const std::wstring& string)
{
	auto composable = std::make_unique<Composable>();
	auto bindable = std::make_unique<Bindable>(string);
	composable->bindable = std::move(bindable);
	return composable;
}

static std::unique_ptr<Composable> MakeComposableFromFunctionLit(std::unique_ptr<FunctionLit> fLiteral)
{
	auto composable = std::make_unique<Composable>();
	auto bindable = std::make_unique<Bindable>(std::move(fLiteral));
	composable->bindable = std::move(bindable);
	return composable;
}

static std::unique_ptr<StandardExpression> MakeRelationExpression(std::unique_ptr<Factor> firstFactor, RelationOperator relOp, std::unique_ptr<Factor> secondFactor)
{
	auto expr = MakeExprFromFactor(std::move(firstFactor));
	auto& relation = expr->conjunctions[0]->relations[0];
	relation->relationOperator = relOp;
	relation->secondAdditive = std::make_unique<Additive>();

	relation->secondAdditive->multiplicatives.push_back(MakeMultiplicativeFromFactor(std::move(secondFactor)));

	return expr;
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
	exDeclaration->expression = std::make_unique<Expression>(MakeExprFromLiteral(Literal(10)));

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
	conditional->condition = std::make_unique<Expression>(MakeExprFromLiteral(Literal(true)));
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
	loop->condition = std::make_unique<Expression>(MakeExprFromLiteral(Literal(true)));
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
	exFunDef->block->statements.push_back(std::make_unique<FunctionCallStatement>(std::move(funcCall)));
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
	arguments.push_back(std::make_unique<Expression>(MakeExprFromLiteral(Literal(10))));
	arguments.push_back(std::make_unique<Expression>(MakeExprFromLiteral(Literal(5.f))));
	arguments.push_back(std::make_unique<Expression>(MakeExprFromLiteral(Literal(false))));
	arguments.push_back(std::make_unique<Expression>(MakeExprFromLiteral(Literal(std::wstring(L"string")))));
	arguments.push_back(std::make_unique<Expression>(MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a")))));
	auto funcCall = std::make_unique<FunctionCall>(std::wstring(L"Buzz"), std::move(arguments));
	exFunDef->block->statements.push_back(std::make_unique<FunctionCallStatement>(std::move(funcCall)));
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

	auto returnStatement = std::make_unique<Return>(std::make_unique<Expression>(MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a")))));
	exFunDef->block->statements.push_back(std::move(returnStatement));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, MultipleFunDefs)
{
	std::wstringstream input(L"func Fizz()\n{\n}\nfunc Buzz()\n{\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef1 = std::make_unique<FunctionDefiniton>();
	exFunDef1->identifier = L"Fizz";
	exFunDef1->block = std::make_unique<Block>();

	auto exFunDef2 = std::make_unique<FunctionDefiniton>();
	exFunDef2->identifier = L"Buzz";
	exFunDef2->block = std::make_unique<Block>();

	exFunDefs.push_back(std::move(exFunDef1));
	exFunDefs.push_back(std::move(exFunDef2));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, SampleFunctionalCode)
{
	std::wstringstream input(L"func Double(mut a)\n{\nvar b = a;\nreturn a + b;\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Double";
	exFunDef->block = std::make_unique<Block>();
	exFunDef->parameters.push_back(std::make_unique<Param>(L"a", true));

	auto exDeclaration = std::make_unique<Declaration>();
	exDeclaration->identifier = L"b";
	exDeclaration->expression = std::make_unique<Expression>(MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a"))));
	exFunDef->block->statements.push_back(std::move(exDeclaration));

	auto a = MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a")));
	a->conjunctions[0]->relations[0]->firstAdditive->operators.push_back(AdditionOperator::Plus);

	auto b = MakeMultiplicativeFromFactor(std::make_unique<Factor>(std::wstring(L"b")));
	a->conjunctions[0]->relations[0]->firstAdditive->multiplicatives.push_back(std::move(b));
	auto returnStatement = std::make_unique<Return>(std::make_unique<Expression>(std::move(a)));

	exFunDef->block->statements.push_back(std::move(returnStatement));

	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, Compose)
{
	std::wstringstream input(L"func Fizz()\n{\nf = [a >> c];\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto fExpr = std::make_unique<FuncExpression>();
	fExpr->composables.push_back(MakeComposableFromString(L"a"));
	fExpr->composables.push_back(MakeComposableFromString(L"c"));

	auto assignment = std::make_unique<Assignment>(L"f", std::make_unique<Expression>(std::move(fExpr)));

	exFunDef->block->statements.push_back(std::move(assignment));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, Bind)
{
	std::wstringstream input(L"func Fizz()\n{\nf = [a << (10, false)];\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto fExpr = std::make_unique<FuncExpression>();
	auto composable = std::make_unique<Composable>();
	auto bindable = std::make_unique<Bindable>(std::wstring(L"a"));
	composable->bindable = std::move(bindable);
	composable->arguments.push_back(std::make_unique<Expression>(MakeExprFromLiteral(Literal(10))));
	composable->arguments.push_back(std::make_unique<Expression>(MakeExprFromLiteral(Literal(false))));

	fExpr->composables.push_back(std::move(composable));

	auto assignment = std::make_unique<Assignment>(L"f", std::make_unique<Expression>(std::move(fExpr)));

	exFunDef->block->statements.push_back(std::move(assignment));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionLiteral)
{
	std::wstringstream input(L"func Fizz()\n{\nf = [(){ var a = 10;}];\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Fizz";
	exFunDef->block = std::make_unique<Block>();

	auto fExpr = std::make_unique<FuncExpression>();
	auto fLiteral = std::make_unique<FunctionLit>();

	auto exDeclaration = std::make_unique<Declaration>();
	exDeclaration->identifier = L"a";
	exDeclaration->expression = std::make_unique<Expression>(MakeExprFromLiteral(Literal(10)));
	fLiteral->block = std::make_unique<Block>();
	fLiteral->block->statements.push_back(std::move(exDeclaration));

	fExpr->composables.push_back(MakeComposableFromFunctionLit(std::move(fLiteral)));

	auto assignment = std::make_unique<Assignment>(L"f", std::make_unique<Expression>(std::move(fExpr)));

	exFunDef->block->statements.push_back(std::move(assignment));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, NestedConditionalAndLoops)
{
	std::wstringstream input(L"func Complex()\n"
		L"{\n"
		L"    if(a > 10)\n"
		L"    {\n"
		L"        while(b < 20)\n"
		L"        {\n"
		L"            if(c == 0)\n"
		L"            {\n"
		L"                return;\n"
		L"            }\n"
		L"        }\n"
		L"    }\n"
		L"}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Complex";
	exFunDef->block = std::make_unique<Block>();

	auto ifConditional = std::make_unique<Conditional>();
	ifConditional->condition = std::make_unique<Expression>(MakeRelationExpression(std::make_unique<Factor>(std::wstring(L"a")), RelationOperator::Greater,
		std::make_unique<Factor>(Literal(10))));
	ifConditional->ifBlock = std::make_unique<Block>();

	auto whileLoop = std::make_unique<WhileLoop>();
	whileLoop->condition = std::make_unique<Expression>(MakeRelationExpression(std::make_unique<Factor>(std::wstring(L"b")), RelationOperator::Less,
		std::make_unique<Factor>(Literal(20))));
	whileLoop->block = std::make_unique<Block>();

	auto nestedIf = std::make_unique<Conditional>();
	nestedIf->condition = std::make_unique<Expression>(MakeRelationExpression(std::make_unique<Factor>(L"c"), RelationOperator::Equal,
		std::make_unique<Factor>(Literal(0))));
	nestedIf->ifBlock = std::make_unique<Block>();

	auto returnStmt = std::make_unique<Return>();
	nestedIf->ifBlock->statements.push_back(std::move(returnStmt));

	whileLoop->block->statements.push_back(std::move(nestedIf));
	ifConditional->ifBlock->statements.push_back(std::move(whileLoop));
	exFunDef->block->statements.push_back(std::move(ifConditional));

	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionExpressionSimpleLiteral)
{
	std::wstringstream input(L"func Test()\n{\nf = [() { return 42; }];\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Test";
	exFunDef->block = std::make_unique<Block>();

	auto funcLiteral = std::make_unique<FunctionLit>();
	funcLiteral->block = std::make_unique<Block>();
	funcLiteral->block->statements.push_back(
		std::make_unique<Return>(std::make_unique<Expression>(MakeExprFromLiteral(Literal(42)))));

	auto funcExpr = std::make_unique<FuncExpression>();

	funcExpr->composables.push_back(MakeComposableFromFunctionLit(std::move(funcLiteral)));

	auto assignment = std::make_unique<Assignment>(L"f", std::make_unique<Expression>(std::move(funcExpr)));

	exFunDef->block->statements.push_back(std::move(assignment));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionLiteralWithParameters)
{
	std::wstringstream input(L"func Test()\n{\nf = [(a, b) { return a + b; }];\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Test";
	exFunDef->block = std::make_unique<Block>();

	auto funcLiteral = std::make_unique<FunctionLit>();

	funcLiteral->parameters.push_back(std::make_unique<Param>(L"a", false));
	funcLiteral->parameters.push_back(std::make_unique<Param>(L"b", false));
	funcLiteral->block = std::make_unique<Block>();

	auto expr = MakeExprFromFactor(std::make_unique<Factor>(std::wstring(L"a")));
	auto additive = std::make_unique<Additive>();
	additive->multiplicatives.push_back(MakeMultiplicativeFromFactor(std::make_unique<Factor>(std::wstring(L"a"))));
	additive->multiplicatives.push_back(MakeMultiplicativeFromFactor(std::make_unique<Factor>(std::wstring(L"b"))));
	additive->operators.push_back(AdditionOperator::Plus);
	expr->conjunctions[0]->relations[0]->firstAdditive = std::move(additive);

	funcLiteral->block->statements.push_back(std::make_unique<Return>(std::make_unique<Expression>(std::move(expr))));

	auto funcExpr = std::make_unique<FuncExpression>();
	funcExpr->composables.push_back(MakeComposableFromFunctionLit(std::move(funcLiteral)));

	auto assignment = std::make_unique<Assignment>(L"f", std::make_unique<Expression>(std::move(funcExpr)));

	exFunDef->block->statements.push_back(std::move(assignment));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);

	ComparePrograms(program, expectedProgram);
}

TEST_F(ParserTest, FunctionExpressionWithComposition)
{
	std::wstringstream input(L"func Test()\n{\nf = [a >> b];\n}");
	auto lexer = Lexer(&input);
	Parser parser = Parser(&lexer);

	const auto program = parser.ParseProgram();

	Program expectedProgram;
	vecUni<FunctionDefiniton> exFunDefs;

	auto exFunDef = std::make_unique<FunctionDefiniton>();
	exFunDef->identifier = L"Test";
	exFunDef->block = std::make_unique<Block>();

	auto funcExpr = std::make_unique<FuncExpression>();
	funcExpr->composables.push_back(MakeComposableFromString(L"a"));
	funcExpr->composables.push_back(MakeComposableFromString(L"b"));

	auto assignment = std::make_unique<Assignment>(
		L"f", std::make_unique<Expression>(std::move(funcExpr)));

	exFunDef->block->statements.push_back(std::move(assignment));
	exFunDefs.push_back(std::move(exFunDef));
	expectedProgram.funDefs = std::move(exFunDefs);
	ComparePrograms(program, expectedProgram);
}