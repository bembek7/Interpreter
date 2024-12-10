#include <gtest/gtest.h>

static void CompareStandardExpressions(const StandardExpression* const expression, const StandardExpression* const expectedExpression);
static void CompareExpressions(const Expression* const expression, const Expression* const expectedExpression);
static void CompareFunctionCalls(const FunctionCall* const funcCall, const FunctionCall* const expectedFuncCall);
static void CompareFunctionCallStatements(const FunctionCallStatement* const funcCall, const FunctionCallStatement* const expectedFuncCall);
static void CompareFuncExpressions(const FuncExpression* const funcExpr, const FuncExpression* const expectedFuncExpr);
static void CompareComposables(const Composable* const composable, const Composable* const expectedComposable);
static void CompareBindables(const Bindable* const bindable, const Bindable* const expectedBindable);
static void CompareFunctionLits(const FunctionLit* const funcLit, const FunctionLit* const expectedFuncLit);
static void CompareMultiplicatives(const Multiplicative* const multiplicative, const Multiplicative* const expectedMultiplicative);
static void CompareAdditives(const Additive* const additive, const Additive* const expectedAdditive);
static void CompareRelations(const Relation* const relation, const Relation* const expectedRelation);
static void CompareConjunctions(const Conjunction* const conjunction, const Conjunction* const expectedConjunction);
static void CompareFactors(const Factor* const factor, const Factor* const expectedFactor);
static void CompareLiterals(const Literal* const literal, const Literal* const expectedLiteral);
static void CompareConditionals(const Conditional* const conditional, const Conditional* const expectedConditional);
static void CompareWhileLoops(const WhileLoop* const whileLoop, const WhileLoop* const expectedWhileLoop);
static void CompareReturns(const Return* const returnStmt, const Return* const expectedReturnStmt);
static void CompareDeclarations(const Declaration* const declaration, const Declaration* const expectedDeclaration);
static void CompareAssignments(const Assignment* const assignment, const Assignment* const expectedAssignment);
static void CompareParams(const Param& param, const Param& expectedParam);
static void CompareStatements(const Statement* const statement, const Statement* const expectedStatement);
static void CompareBlocks(const Block* const block, const Block* const expectedBlock);
static void CompareFunDefs(const FunctionDefiniton* const funDef, const FunctionDefiniton* const expectedFunDef);

static void CompareExpressions(const Expression* const expression, const Expression* const expectedExpression)
{
	if (auto* funcExpr = std::get_if<std::unique_ptr<FuncExpression>>(&expression->expression))
	{
		auto* expectedFuncExpr = std::get_if<std::unique_ptr<FuncExpression>>(&expectedExpression->expression);
		ASSERT_TRUE(expectedFuncExpr != nullptr);
		CompareFuncExpressions(funcExpr->get(), expectedFuncExpr->get());
	}
	else if (auto* stdExpr = std::get_if<std::unique_ptr<StandardExpression>>(&expression->expression))
	{
		auto* expectedStdExpr = std::get_if<std::unique_ptr<StandardExpression>>(&expectedExpression->expression);
		ASSERT_TRUE(expectedStdExpr != nullptr);
		CompareStandardExpressions(stdExpr->get(), expectedStdExpr->get());
	}
}

static void CompareStandardExpressions(const StandardExpression* const expression, const StandardExpression* const expectedExpression)
{
	ASSERT_EQ(expression->conjunctions.size(), expectedExpression->conjunctions.size());
	for (size_t i = 0; i < expression->conjunctions.size(); ++i)
	{
		CompareConjunctions(expression->conjunctions[i].get(), expectedExpression->conjunctions[i].get());
	}
}

static void CompareFunctionCalls(const FunctionCall* const funcCall, const FunctionCall* const expectedFuncCall)
{
	EXPECT_EQ(funcCall->identifier, expectedFuncCall->identifier);
	ASSERT_EQ(funcCall->arguments.size(), expectedFuncCall->arguments.size());

	for (size_t i = 0; i < funcCall->arguments.size(); ++i)
	{
		CompareExpressions(funcCall->arguments[i].get(), expectedFuncCall->arguments[i].get());
	}
}

inline void CompareFunctionCallStatements(const FunctionCallStatement* const funcCall, const FunctionCallStatement* const expectedFuncCall)
{
	CompareFunctionCalls(funcCall->funcCall.get(), expectedFuncCall->funcCall.get());
}

static void CompareFuncExpressions(const FuncExpression* const funcExpr, const FuncExpression* const expectedFuncExpr)
{
	ASSERT_EQ(funcExpr->composables.size(), expectedFuncExpr->composables.size());
	for (size_t i = 0; i < funcExpr->composables.size(); ++i)
	{
		CompareComposables(funcExpr->composables[i].get(), expectedFuncExpr->composables[i].get());
	}
}

static void CompareComposables(const Composable* const composable, const Composable* const expectedComposable)
{
	ASSERT_EQ(composable->arguments.size(), expectedComposable->arguments.size());
	for (size_t i = 0; i < composable->arguments.size(); ++i)
	{
		CompareExpressions(composable->arguments[i].get(), expectedComposable->arguments[i].get());
	}
	CompareBindables(composable->bindable.get(), expectedComposable->bindable.get());
}

static void CompareBindables(const Bindable* const bindable, const Bindable* const expectedBindable)
{
	if (auto* funcLit = std::get_if<std::unique_ptr<FunctionLit>>(&bindable->bindable))
	{
		auto* expectedFuncLit = std::get_if<std::unique_ptr<FunctionLit>>(&expectedBindable->bindable);
		ASSERT_TRUE(expectedFuncLit != nullptr);
		CompareFunctionLits(funcLit->get(), expectedFuncLit->get());
	}
	else if (auto* funcExpr = std::get_if<std::unique_ptr<FuncExpression>>(&bindable->bindable))
	{
		auto* expectedFuncExpr = std::get_if<std::unique_ptr<FuncExpression>>(&expectedBindable->bindable);
		ASSERT_TRUE(expectedFuncExpr != nullptr);
		CompareFuncExpressions(funcExpr->get(), expectedFuncExpr->get());
	}
	else if (auto* funcCall = std::get_if<std::unique_ptr<FunctionCall>>(&bindable->bindable))
	{
		auto* expectedFuncCall = std::get_if<std::unique_ptr<FunctionCall>>(&expectedBindable->bindable);
		ASSERT_TRUE(expectedFuncCall != nullptr);
		CompareFunctionCalls(funcCall->get(), expectedFuncCall->get());
	}
	else if (auto* str = std::get_if<std::wstring>(&bindable->bindable))
	{
		auto* expectedStr = std::get_if<std::wstring>(&expectedBindable->bindable);
		ASSERT_TRUE(expectedStr != nullptr);
		EXPECT_EQ(*str, *expectedStr);
	}
}

static void CompareFunctionLits(const FunctionLit* const funcLit, const FunctionLit* const expectedFuncLit)
{
	ASSERT_EQ(funcLit->parameters.size(), expectedFuncLit->parameters.size());
	for (size_t i = 0; i < funcLit->parameters.size(); ++i)
	{
		CompareParams(funcLit->parameters[i], expectedFuncLit->parameters[i]);
	}
	CompareBlocks(funcLit->block.get(), expectedFuncLit->block.get());
}

static void CompareMultiplicatives(const Multiplicative* const multiplicative, const Multiplicative* const expectedMultiplicative)
{
	ASSERT_EQ(multiplicative->factors.size(), expectedMultiplicative->factors.size());
	for (size_t i = 0; i < multiplicative->factors.size(); ++i)
	{
		CompareFactors(multiplicative->factors[i].get(), expectedMultiplicative->factors[i].get());
	}
	EXPECT_EQ(multiplicative->operators, expectedMultiplicative->operators);
}

static void CompareAdditives(const Additive* const additive, const Additive* const expectedAdditive)
{
	ASSERT_EQ(additive->multiplicatives.size(), expectedAdditive->multiplicatives.size());
	for (size_t i = 0; i < additive->multiplicatives.size(); ++i)
	{
		CompareMultiplicatives(additive->multiplicatives[i].get(), expectedAdditive->multiplicatives[i].get());
	}
	EXPECT_EQ(additive->operators, expectedAdditive->operators);
	EXPECT_EQ(additive->negated, expectedAdditive->negated);
}

static void CompareRelations(const Relation* const relation, const Relation* const expectedRelation)
{
	CompareAdditives(relation->firstAdditive.get(), expectedRelation->firstAdditive.get());
	if (relation->secondAdditive)
	{
		CompareAdditives(relation->secondAdditive.get(), expectedRelation->secondAdditive.get());
	}
	EXPECT_EQ(relation->relationOperator, expectedRelation->relationOperator);
}

static void CompareConjunctions(const Conjunction* const conjunction, const Conjunction* const expectedConjunction)
{
	ASSERT_EQ(conjunction->relations.size(), expectedConjunction->relations.size());
	for (size_t i = 0; i < conjunction->relations.size(); ++i)
	{
		CompareRelations(conjunction->relations[i].get(), expectedConjunction->relations[i].get());
	}
}

static void CompareFactors(const Factor* const factor, const Factor* const expectedFactor)
{
	EXPECT_EQ(factor->logicallyNegated, expectedFactor->logicallyNegated);

	if (auto* lit = std::get_if<Literal>(&factor->factor))
	{
		auto* expectedLit = std::get_if<Literal>(&expectedFactor->factor);
		ASSERT_TRUE(expectedLit != nullptr);
		CompareLiterals(lit, expectedLit);
	}
	else if (auto* expr = std::get_if<std::unique_ptr<StandardExpression>>(&factor->factor))
	{
		auto* expectedExpr = std::get_if<std::unique_ptr<StandardExpression>>(&expectedFactor->factor);
		ASSERT_TRUE(expectedExpr != nullptr);
		CompareStandardExpressions(expr->get(), expectedExpr->get());
	}
	else if (auto* funcCall = std::get_if<std::unique_ptr<FunctionCall>>(&factor->factor))
	{
		auto* expectedFuncCall = std::get_if<std::unique_ptr<FunctionCall>>(&expectedFactor->factor);
		ASSERT_TRUE(expectedFuncCall != nullptr);
		CompareFunctionCalls(funcCall->get(), expectedFuncCall->get());
	}
	else if (auto* str = std::get_if<std::wstring>(&factor->factor))
	{
		auto* expectedStr = std::get_if<std::wstring>(&expectedFactor->factor);
		ASSERT_TRUE(expectedStr != nullptr);
		EXPECT_EQ(*str, *expectedStr);
	}
}

static void CompareLiterals(const Literal* const literal, const Literal* const expectedLiteral)
{
	EXPECT_EQ(literal->value, expectedLiteral->value);
}

static void CompareConditionals(const Conditional* const conditional, const Conditional* const expectedConditional)
{
	CompareExpressions(conditional->condition.get(), expectedConditional->condition.get());
	CompareBlocks(conditional->ifBlock.get(), expectedConditional->ifBlock.get());
	if (conditional->elseBlock != expectedConditional->elseBlock)
	{
		CompareBlocks(conditional->elseBlock.get(), expectedConditional->elseBlock.get());
	}
}

static void CompareWhileLoops(const WhileLoop* const whileLoop, const WhileLoop* const expectedWhileLoop)
{
	CompareExpressions(whileLoop->condition.get(), expectedWhileLoop->condition.get());
	CompareBlocks(whileLoop->block.get(), expectedWhileLoop->block.get());
}

static void CompareReturns(const Return* const returnStmt, const Return* const expectedReturnStmt)
{
	if (returnStmt->expression != expectedReturnStmt->expression)
	{
		CompareExpressions(returnStmt->expression.get(), expectedReturnStmt->expression.get());
	}
}

static void CompareDeclarations(const Declaration* const declaration, const Declaration* const expectedDeclaration)
{
	EXPECT_EQ(declaration->varMutable, expectedDeclaration->varMutable);
	EXPECT_EQ(declaration->identifier, expectedDeclaration->identifier);
	CompareExpressions(declaration->expression.get(), expectedDeclaration->expression.get());
}

static void CompareAssignments(const Assignment* const assignment, const Assignment* const expectedAssignment)
{
	EXPECT_EQ(assignment->identifier, expectedAssignment->identifier);
	CompareExpressions(assignment->expression.get(), expectedAssignment->expression.get());
}

static void CompareParams(const Param& param, const Param& expectedParam)
{
	EXPECT_EQ(param.paramMutable, expectedParam.paramMutable);
	EXPECT_EQ(param.identifier, expectedParam.identifier);
}

static void CompareBlocks(const Block* const block, const Block* const expectedBlock)
{
	ASSERT_EQ(block->statements.size(), expectedBlock->statements.size());

	for (size_t i = 0; i < block->statements.size(); ++i)
	{
		CompareStatements(block->statements[i].get(), expectedBlock->statements[i].get());
	};
}

static void CompareStatements(const Statement* const statement, const Statement* const expectedStatement)
{
	if (auto* blockStmt = dynamic_cast<const Block*>(statement))
	{
		auto* expectedBlockStmt = dynamic_cast<const Block*>(expectedStatement);
		ASSERT_TRUE(expectedBlockStmt != nullptr);
		CompareBlocks(blockStmt, expectedBlockStmt);
	}
	else if (auto* funcCallStmt = dynamic_cast<const FunctionCallStatement*>(statement))
	{
		auto* expectedFuncCallStmt = dynamic_cast<const FunctionCallStatement*>(expectedStatement);
		ASSERT_TRUE(expectedFuncCallStmt != nullptr);
		CompareFunctionCallStatements(funcCallStmt, expectedFuncCallStmt);
	}
	else if (auto* conditionalStmt = dynamic_cast<const Conditional*>(statement))
	{
		auto* expectedConditionalStmt = dynamic_cast<const Conditional*>(expectedStatement);
		ASSERT_TRUE(expectedConditionalStmt != nullptr);
		CompareConditionals(conditionalStmt, expectedConditionalStmt);
	}
	else if (auto* whileLoopStmt = dynamic_cast<const WhileLoop*>(statement))
	{
		auto* expectedWhileLoopStmt = dynamic_cast<const WhileLoop*>(expectedStatement);
		ASSERT_TRUE(expectedWhileLoopStmt != nullptr);
		CompareWhileLoops(whileLoopStmt, expectedWhileLoopStmt);
	}
	else if (auto* returnStmt = dynamic_cast<const Return*>(statement))
	{
		auto* expectedReturnStmt = dynamic_cast<const Return*>(expectedStatement);
		ASSERT_TRUE(expectedReturnStmt != nullptr);
		CompareReturns(returnStmt, expectedReturnStmt);
	}
	else if (auto* declarationStmt = dynamic_cast<const Declaration*>(statement))
	{
		auto* expectedDeclarationStmt = dynamic_cast<const Declaration*>(expectedStatement);
		ASSERT_TRUE(expectedDeclarationStmt != nullptr);
		CompareDeclarations(declarationStmt, expectedDeclarationStmt);
	}
	else if (auto* assignmentStmt = dynamic_cast<const Assignment*>(statement))
	{
		auto* expectedAssignmentStmt = dynamic_cast<const Assignment*>(expectedStatement);
		ASSERT_TRUE(expectedAssignmentStmt != nullptr);
		CompareAssignments(assignmentStmt, expectedAssignmentStmt);
	}
	else
	{
		FAIL() << "Unknown statement type.";
	}
}

static void CompareFunDefs(const FunctionDefiniton* const funDef, const FunctionDefiniton* const expectedFunDef)
{
	EXPECT_EQ(funDef->identifier, expectedFunDef->identifier);
	ASSERT_EQ(funDef->parameters.size(), expectedFunDef->parameters.size());

	for (size_t i = 0; i < funDef->parameters.size(); ++i)
	{
		CompareParams(funDef->parameters[i], expectedFunDef->parameters[i]);
	}

	CompareBlocks(funDef->block.get(), expectedFunDef->block.get());
}

static void ComparePrograms(const Program& program, const Program& expectedProgram)
{
	ASSERT_EQ(program.funDefs.size(), expectedProgram.funDefs.size());

	for (size_t i = 0; i < program.funDefs.size(); ++i)
	{
		CompareFunDefs(program.funDefs[i].get(), expectedProgram.funDefs[i].get());
	}
}