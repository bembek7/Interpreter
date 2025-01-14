#include "ParserImpl.h"
#include <iostream>

ParserImpl::ParserImpl(Lexer* const lexer)
{
	SetLexer(lexer);
}

void ParserImpl::SetLexer(Lexer* const newLexer)
{
	if (!newLexer)
	{
		throw std::runtime_error("Passed lexer was a nullptr");
	}
	lexer = newLexer;
}

LexToken ParserImpl::GetTokenFromLexer()
{
	auto lexOut = lexer->ResolveNext();
	auto& token = lexOut.first;
	while (token.GetType() == LexToken::TokenType::Comment)
	{
		lexOut = lexer->ResolveNext();
		token = lexOut.first;
	}
	for (const auto& lexError : lexOut.second)
	{
		std::cout << "Lexical Error [line: " << lexError.GetPosition().line << ", column : " <<
			lexError.GetPosition().column << "] " << lexError.GetMessage() << std::endl;
		if (lexError.IsTerminating())
		{
			throw ParserException("Could not get token, terminating lexer error occured.", currentPosition);
		}
	}

	return token;
}

LexToken ParserImpl::GetNextToken()
{
	if (!lastUnusedToken)
	{
		return GetTokenFromLexer();
	}
	else
	{
		const auto out = std::move(*lastUnusedToken);
		lastUnusedToken = std::nullopt;
		currentPosition = out.GetPosition();
		return out;
	}
}

std::optional<LexToken> ParserImpl::GetExpectedToken(const LexToken::TokenType expectedToken)
{
	auto token = GetNextToken();
	if (token.GetType() != expectedToken)
	{
		lastUnusedToken = std::move(token);
		return std::nullopt;
	}
	return token;
}

bool ParserImpl::ConsumeToken(const LexToken::TokenType expectedToken)
{
	return GetExpectedToken(expectedToken).has_value();
}

bool ParserImpl::CheckToken(const LexToken::TokenType expectedToken)
{
	lastUnusedToken = GetNextToken();
	return lastUnusedToken->GetType() == expectedToken;
}

// program = { function_definition };
std::unique_ptr<Program> ParserImpl::ParseProgram()
{
	auto program = std::make_unique<Program>();
	try
	{
		while (auto funDef = ParseFunctionDefinition())
		{
			program->funDefs.push_back(std::move(funDef));
		}
		if (!ConsumeToken(LexToken::TokenType::EndOfFile))
		{
			throw ParserException("Could not parse next function definition and end of file was not met.", currentPosition);
		}
	}
	catch (const ParserException& pe)
	{
		std::cout << pe.what();
	}

	return program;
}

// function_definition = "func", identifier, "(", parameters, ")", block;
std::unique_ptr<FunctionDefiniton> ParserImpl::ParseFunctionDefinition()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::Func))
	{
		if (CheckToken(LT::EndOfFile))
		{
			return nullptr;
		}
		else
		{
			throw ParserException("Expected \"func\" to start function definition.", currentPosition);
		}
	}
	auto startingPosition = currentPosition;
	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		throw ParserException("Expected idenitfier after \"func\" keyword.", currentPosition);
	}

	auto functionDefinition = std::make_unique<FunctionDefiniton>();
	functionDefinition->startingPosition = startingPosition;
	functionDefinition->identifier = std::get<std::wstring>(idToken->GetValue());

	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected \"(\" after identifier.", currentPosition);
	}
	functionDefinition->parameters = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected \")\" to close the params list.", currentPosition);
	}

	functionDefinition->block = ParseBlock();

	if (!functionDefinition->block)
	{
		throw ParserException("Expected block after function definition.", currentPosition);
	}

	return functionDefinition;
}

// parameters = [parameter, { ",", parameter }];
std::vector<Param> ParserImpl::ParseParams()
{
	std::vector<Param> params;

	auto param = ParseParam();
	if (!param)
	{
		return params;
	}

	params.push_back(std::move(*param));

	while (ConsumeToken(LexToken::TokenType::Comma))
	{
		param = ParseParam();
		if (!param)
		{
			throw ParserException("Expected parameter after coma.", currentPosition);
		}
		params.push_back(std::move(*param));
	}

	return params;
}

// parameter = ["mut"], identifier
std::optional<Param> ParserImpl::ParseParam()
{
	using LT = LexToken::TokenType;

	auto param = Param();
	param.paramMutable = ConsumeToken(LT::Mut);
	auto startingPosition = currentPosition;
	const auto idToken = GetExpectedToken(LT::Identifier);
	param.startingPosition = (param.paramMutable) ? startingPosition : currentPosition;
	if (!idToken)
	{
		if (param.paramMutable)
		{
			throw ParserException("Expected identifier after \"mut\" keyword.", currentPosition);
		}
		return std::nullopt;
	}
	param.identifier = std::get<std::wstring>(idToken->GetValue());
	return param;
}

// block = "{", { statement }, "}";
std::unique_ptr<Block> ParserImpl::ParseBlock()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LBracket))
	{
		return nullptr;
	}
	auto startingPosition = currentPosition;
	std::vector<std::unique_ptr<Statement>> statements;

	while (auto statement = ParseStatement())
	{
		statements.push_back(std::move(statement));
	}

	if (!ConsumeToken(LT::RBracket))
	{
		throw ParserException("Expected block closing bracket.", currentPosition);
	}
	auto block = std::make_unique<Block>(std::move(statements));
	block->startingPosition = startingPosition;
	return block;
}

//statement = function_call_statement
//			| conditional
//			| loop
//			| return_statement
//			| block
//			| declaration
//			| assignment
std::unique_ptr<Statement> ParserImpl::ParseStatement()
{
	if (auto conditional = ParseConditional())
	{
		return conditional;
	}
	if (auto loop = ParseLoop())
	{
		return loop;
	}
	if (auto returnStatement = ParseReturn())
	{
		return returnStatement;
	}
	if (auto block = ParseBlock())
	{
		return block;
	}
	if (auto declaration = ParseDeclaration())
	{
		return declaration;
	}

	auto idToken = GetExpectedToken(LexToken::TokenType::Identifier);
	if (idToken)
	{
		auto startingPosition = currentPosition;
		const auto identifier = std::get<std::wstring>(idToken->GetValue());
		if (auto assignment = ParseRestOfAssignment(identifier))
		{
			assignment->startingPosition = startingPosition;
			return assignment;
		}
		if (auto funcCall = ParseRestOfFunctionCallStatement(identifier))
		{
			funcCall->startingPosition = startingPosition;
			return funcCall;
		}
	}

	return nullptr;
}

// function_call_statement = function_call, ";";
std::unique_ptr<FunctionCallStatement> ParserImpl::ParseRestOfFunctionCallStatement(const std::wstring& identifier)
{
	if (auto funcCall = ParseRestOfFunctionCall(identifier))
	{
		if (!ConsumeToken(LexToken::TokenType::Semicolon))
		{
			throw ParserException("Expected semicolon at the end", currentPosition);
		}
		auto funcCallStatement = std::make_unique<FunctionCallStatement>(std::move(funcCall));
		return funcCallStatement;
	}
	return nullptr;
}

std::unique_ptr<FunctionCall> ParserImpl::ParseRestOfFunctionCall(const std::wstring& identifier)
{
	using LT = LexToken::TokenType;

	auto startingPosition = currentPosition;
	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}

	auto arguments = ParseArguments();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses in the function call.", currentPosition);
	}
	auto funcCall = std::make_unique<FunctionCall>(identifier, std::move(arguments));
	funcCall->startingPosition = startingPosition;
	return funcCall;
}

// conditional = "if", "(", expression, ")", block, ["else", block];
std::unique_ptr<Conditional> ParserImpl::ParseConditional()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::If))
	{
		return nullptr;
	}
	auto startingPosition = currentPosition;
	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected opening parentheses after if.", currentPosition);
	}
	auto conditional = std::make_unique<Conditional>();
	conditional->startingPosition = startingPosition;
	conditional->condition = ParseStandardExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after condition.", currentPosition);
	}

	conditional->ifBlock = ParseBlock();

	if (!conditional->ifBlock)
	{
		throw ParserException("Expected block after if condition.", currentPosition);
	}

	if (ConsumeToken(LT::Else))
	{
		conditional->elseBlock = ParseBlock();

		if (!conditional->elseBlock)
		{
			throw ParserException("Expected block after else keyword.", currentPosition);
		}
	}

	return conditional;
}

// loop = "while", "(", expression, ")", block
std::unique_ptr<WhileLoop> ParserImpl::ParseLoop()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::While))
	{
		return nullptr;
	}
	auto startingPosition = currentPosition;
	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected opening parentheses after while.", currentPosition);
	}

	auto whileLoop = std::make_unique<WhileLoop>();
	whileLoop->startingPosition = startingPosition;
	whileLoop->condition = ParseStandardExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after condition.", currentPosition);
	}

	whileLoop->block = ParseBlock();

	if (!whileLoop->block)
	{
		throw ParserException("Expected block after while condition.", currentPosition);
	}

	return whileLoop;
}

// return_statement = "return", [expression], ";";
std::unique_ptr<Return> ParserImpl::ParseReturn()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::Return))
	{
		return nullptr;
	}
	auto returnStatement = std::make_unique<Return>(ParseExpression());
	returnStatement->startingPosition = currentPosition;
	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon after return statement.", currentPosition);
	}
	return returnStatement;
}

// declaration = ["mut"], "var", identifier, ["=", expression], ";";
std::unique_ptr<Declaration> ParserImpl::ParseDeclaration()
{
	using LT = LexToken::TokenType;

	auto declaration = std::make_unique<Declaration>();

	declaration->varMutable = ConsumeToken(LT::Mut);
	auto startingPosition = currentPosition;
	if (!ConsumeToken(LT::Var))
	{
		if (declaration->varMutable)
		{
			throw ParserException("Expected var keyword after mut keyword in declaration.", currentPosition);
		}
		return nullptr;
	}

	declaration->startingPosition = (declaration->varMutable) ? startingPosition : currentPosition;

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		throw ParserException("Expected identifier after var keyword.", currentPosition);
	}
	declaration->identifier = std::get<std::wstring>(idToken->GetValue());

	if (ConsumeToken(LT::Assign))
	{
		declaration->expression = ParseExpression();
		if (!declaration->expression)
		{
			throw ParserException("Expected expression after = in assignment.", currentPosition);
		}
	}
	else
	{
		if (!declaration->varMutable)
		{
			throw ParserException("Expected expression assignment with declaration if expression is not mutable.", currentPosition);
		}
	}

	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon at the end of declaration.", currentPosition);
	}

	return declaration;
}

// assignment = identifier, "=", expression, ";";
std::unique_ptr<Assignment> ParserImpl::ParseRestOfAssignment(const std::wstring& identifier)
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::Assign))
	{
		return nullptr;
	}

	auto expression = ParseExpression();

	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon at the end of assignment.", currentPosition);
	}
	return std::make_unique<Assignment>(identifier, std::move(expression));
}

// arguments = [expression, { ",", expression }];
std::vector<std::unique_ptr<Expression>> ParserImpl::ParseArguments()
{
	std::vector<std::unique_ptr<Expression>> expressions;

	auto expression = ParseExpression();
	if (expression)
	{
		expressions.push_back(std::move(expression));
	}

	while (ConsumeToken(LexToken::TokenType::Comma))
	{
		if (expression = ParseExpression())
		{
			expressions.push_back(std::move(expression));
		}
		else
		{
			throw ParserException("Expected expression after comma in the arguments list.", currentPosition);
		}
	}

	return expressions;
}

// expression = standard_expression
//			  | "[", func_expression, "]";
std::unique_ptr<Expression> ParserImpl::ParseExpression()
{
	using LT = LexToken::TokenType;

	if (ConsumeToken(LT::LSquareBracket))
	{
		auto fExpr = ParseFuncExpression();
		if (!fExpr)
		{
			throw ParserException("Expected function expression after \"[\".", currentPosition);
		}
		if (!ConsumeToken(LT::RSquareBracket))
		{
			throw ParserException("Expected \"]\" after function expression.", currentPosition);
		}
		return fExpr;
	}
	if (auto stdExpr = ParseStandardExpression())
	{
		return stdExpr;
	}
	return nullptr;
}

// standard_expression   = conjunction, { "||", conjunction }
std::unique_ptr<StandardExpression> ParserImpl::ParseStandardExpression()
{
	auto conjunction = ParseConjunction();
	if (!conjunction)
	{
		return nullptr;
	}
	auto startingPosition = conjunction->startingPosition;
	std::vector<std::unique_ptr<Conjunction>> conjunctions;
	conjunctions.push_back(std::move(conjunction));
	while (ConsumeToken(LexToken::TokenType::LogicalOr))
	{
		if (conjunction = ParseConjunction())
		{
			conjunctions.push_back(std::move(conjunction));
		}
		else
		{
			throw ParserException("Expected expression after \"||\".", currentPosition);
		}
	}
	auto expr = std::make_unique<StandardExpression>(std::move(conjunctions));
	expr->startingPosition = startingPosition;
	return expr;
}

// conjunction = relation_term, { "&&", relation_term };
std::unique_ptr<Conjunction> ParserImpl::ParseConjunction()
{
	auto relation = ParseRelation();
	if (!relation)
	{
		return nullptr;
	}
	auto conjunction = std::make_unique<Conjunction>();
	conjunction->startingPosition = relation->startingPosition;
	conjunction->relations.push_back(std::move(relation));
	
	while (ConsumeToken(LexToken::TokenType::LogicalAnd))
	{
		if (relation = ParseRelation())
		{
			conjunction->relations.push_back(std::move(relation));
		}
		else
		{
			throw ParserException("Expected expression after \"&&\".", currentPosition);
		}
	}

	return conjunction;
}

// relation_term = additive_term, [relation_operator, additive_term];
std::unique_ptr<Relation> ParserImpl::ParseRelation()
{
	using LT = LexToken::TokenType;
	auto additive = ParseAdditive();
	if (!additive)
	{
		return nullptr;
	}

	auto relation = std::make_unique<Relation>();
	relation->startingPosition = additive->startingPosition;
	relation->firstAdditive = std::move(additive);

	bool isRelationOp = false;
	if (ConsumeToken(LT::Less))
	{
		relation->relationOperator = RelationOperator::Less;
		isRelationOp = true;
	}
	else if (ConsumeToken(LT::LessEqual))
	{
		relation->relationOperator = RelationOperator::LessEqual;
		isRelationOp = true;
	}
	else if (ConsumeToken(LT::Greater))
	{
		relation->relationOperator = RelationOperator::Greater;
		isRelationOp = true;
	}
	else if (ConsumeToken(LT::GreaterEqual))
	{
		relation->relationOperator = RelationOperator::GreaterEqual;
		isRelationOp = true;
	}
	else if (ConsumeToken(LT::Equal))
	{
		relation->relationOperator = RelationOperator::Equal;
		isRelationOp = true;
	}
	else if (ConsumeToken(LT::NotEqual))
	{
		relation->relationOperator = RelationOperator::NotEqual;
		isRelationOp = true;
	}
	if (isRelationOp)
	{
		auto nextAdditive = ParseAdditive();
		if (!nextAdditive)
		{
			throw ParserException("Expected expression after relation operator.", currentPosition);
		}
		relation->secondAdditive = std::move(nextAdditive);
	}

	return relation;
}

// additive_term = ["-"], (multiplicative_term, { ("+" | "-"), multiplicative_term });
std::unique_ptr<Additive> ParserImpl::ParseAdditive()
{
	using LT = LexToken::TokenType;

	auto additive = std::make_unique<Additive>();

	additive->negated = ConsumeToken(LT::Minus);
	auto startingPosition = currentPosition;
	auto multiplicative = ParseMultiplicative();
	if (!multiplicative)
	{
		return nullptr;
	}
	additive->startingPosition = (additive->negated) ? startingPosition : multiplicative->startingPosition;

	additive->multiplicatives.push_back(std::move(multiplicative));

	while (true)
	{
		if (ConsumeToken(LT::Plus))
		{
			additive->operators.push_back(AdditionOperator::Plus);
		}
		else
		{
			if (ConsumeToken(LT::Minus))
			{
				additive->operators.push_back(AdditionOperator::Minus);
			}
			else
			{
				break;
			}
		}

		if (multiplicative = ParseMultiplicative())
		{
			additive->multiplicatives.push_back(std::move(multiplicative));
		}
		else
		{
			throw ParserException("Expected expression after addition operator.", currentPosition);
		}
	}

	return additive;
}

// multiplicative_term = factor, { ("*" | "/"), factor };
std::unique_ptr<Multiplicative> ParserImpl::ParseMultiplicative()
{
	using LT = LexToken::TokenType;

	auto factor = ParseFactor();
	if (!factor)
	{
		return nullptr;
	}

	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->startingPosition = factor->startingPosition;
	multiplicative->factors.push_back(std::move(factor));

	while (true)
	{
		if (ConsumeToken(LT::Asterisk))
		{
			multiplicative->operators.push_back(MultiplicationOperator::Multiply);
		}
		else
		{
			if (ConsumeToken(LT::Slash))
			{
				multiplicative->operators.push_back(MultiplicationOperator::Divide);
			}
			else
			{
				break;
			}
		}

		if (factor = ParseFactor())
		{
			multiplicative->factors.push_back(std::move(factor));
		}
		else
		{
			throw ParserException("Expected expression after muliplication operator.", currentPosition);
		}
	}

	return multiplicative;
}

// factor = ["!"], (literal | "(", standard_expression, ")" | identifier | function_call);
std::unique_ptr<Factor> ParserImpl::ParseFactor()
{
	using LT = LexToken::TokenType;

	auto factor = std::make_unique<Factor>();
	factor->logicallyNegated = ConsumeToken(LT::LogicalNot);
	auto startingPosition = currentPosition;

	if (auto literal = ParseLiteral())
	{
		factor->startingPosition = (factor->logicallyNegated) ? startingPosition : literal->startingPosition;
		factor->factor = std::move(*literal);
		return factor;
	}

	if (ConsumeToken(LT::LParenth))
	{
		auto expression = ParseStandardExpression();
		factor->startingPosition = (factor->logicallyNegated) ? startingPosition : expression->startingPosition;
		factor->factor = std::move(expression);
		if (!ConsumeToken(LT::RParenth))
		{
			throw ParserException("Expected closing parentheses after expression", currentPosition);
		}
		return factor;
	}

	auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		factor->startingPosition = (factor->logicallyNegated) ? startingPosition : currentPosition;
		const auto identifier = std::get<std::wstring>(idToken->GetValue());
		if (auto functionCall = ParseRestOfFunctionCall(identifier))
		{
			factor->factor = std::move(functionCall);
			return factor;
		}
		else
		{
			factor->factor = identifier;
			return factor;
		}
	}

	if (factor->logicallyNegated)
	{
		throw ParserException("Expected expression after logical negation.", currentPosition);
	}

	return nullptr;
}

// literal = number | string | boolean;
std::optional<Literal> ParserImpl::ParseLiteral()
{
	using LT = LexToken::TokenType;

	if (const auto integer = GetExpectedToken(LT::Integer))
	{
		return Literal(std::get<int>(integer->GetValue()), currentPosition);
	}

	if (const auto floatNumber = GetExpectedToken(LT::Float))
	{
		return Literal(std::get<float>(floatNumber->GetValue()), currentPosition);
	}

	if (const auto string = GetExpectedToken(LT::String))
	{
		return Literal(std::get<std::wstring>(string->GetValue()), currentPosition);
	}
	;
	if (const auto boolean = GetExpectedToken(LT::Boolean))
	{
		return Literal(std::get<bool>(boolean->GetValue()), currentPosition);
	}

	return std::nullopt;
}

// func_expression = composable, { ">>", composable };
std::unique_ptr<FuncExpression> ParserImpl::ParseFuncExpression()
{
	auto composable = ParseComposable();
	if (!composable)
	{
		return nullptr;
	}
	auto startingPosition = composable->startingPosition;
	std::vector<std::unique_ptr<Composable>> composables;
	composables.push_back(std::move(composable));
	while (ConsumeToken(LexToken::TokenType::FunctionCompose))
	{
		if (composable = ParseComposable())
		{
			composables.push_back(std::move(composable));
		}
		else
		{
			throw ParserException("Expected composable after compose operator.", currentPosition);
		}
	}
	auto funcExpr = std::make_unique<FuncExpression>(std::move(composables));
	funcExpr->startingPosition = startingPosition;
	return funcExpr;
}

// composable = bindable, ["<<", "(", arguments, ")"];
std::unique_ptr<Composable> ParserImpl::ParseComposable()
{
	using LT = LexToken::TokenType;

	auto bindable = ParseBindable();
	if (!bindable)
	{
		return nullptr;
	}

	auto composable = std::make_unique<Composable>();
	composable->startingPosition = bindable->startingPosition;
	composable->bindable = std::move(bindable);

	if (ConsumeToken(LexToken::TokenType::FunctionBind))
	{
		if (!ConsumeToken(LT::LParenth))
		{
			throw ParserException("Expected opening bracket after bind operator.", currentPosition);
		}

		composable->arguments = ParseArguments();

		if (!ConsumeToken(LT::RParenth))
		{
			throw ParserException("Expected closing bracket for arguments list when binding.", currentPosition);
		}
	}

	return composable;
}

// bindable = (function_lit | identifier | function_call | "(", func_expression, ")");
std::unique_ptr<Bindable> ParserImpl::ParseBindable()
{
	using LT = LexToken::TokenType;

	if (auto funcLit = ParseFunctionLit())
	{
		auto startingPosition = funcLit->startingPosition;
		auto bindable = std::make_unique<Bindable>(std::move(funcLit));
		bindable->startingPosition = startingPosition;
		return bindable;
	}

	auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		auto startingPosition = currentPosition;
		const auto identifier = std::get<std::wstring>(idToken->GetValue());
		if (auto functionCall = ParseRestOfFunctionCall(identifier))
		{
			auto bindable = std::make_unique<Bindable>(std::move(functionCall));
			bindable->startingPosition = startingPosition;
			return bindable;
		}
		else
		{
			auto bindable = std::make_unique<Bindable>(identifier);
			bindable->startingPosition = startingPosition;
			return bindable;
		}
	}

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}
	auto funcExpr = ParseFuncExpression();
	auto startingPositon = funcExpr->startingPosition;
	auto bindable = std::make_unique<Bindable>(std::move(funcExpr));
	bindable->startingPosition = startingPositon;
	if (!bindable)
	{
		throw ParserException("Expected function expression after opening parentheses.", currentPosition);
	}

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after function expression", currentPosition);
	}

	return bindable;
}

// function_lit = "(", parameters, ")", block;
std::unique_ptr<FunctionLiteral> ParserImpl::ParseFunctionLit()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}
	auto functionLit = std::make_unique<FunctionLiteral>();
	functionLit->startingPosition = currentPosition;
	functionLit->parameters = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after parameters list", currentPosition);
	}

	functionLit->block = ParseBlock();
	if (!functionLit->block)
	{
		throw ParserException("Expected block after function literal parameters.", currentPosition);
	}
	return functionLit;
}