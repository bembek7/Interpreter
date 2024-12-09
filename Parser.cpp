#include "Parser.h"
#include <iostream>

Parser::Parser(Lexer* const lexer) noexcept :
	lexer(lexer)
{}

void Parser::SetLexer(Lexer* const newLexer) noexcept
{
	lexer = newLexer;
}

LexToken Parser::GetNextToken()
{
	if (unusedTokens.empty())
	{
		auto lexOut = lexer->ResolveNext();
		auto& token = lexOut.first;
		while (token.GetType() == LexToken::TokenType::Comment)
		{
			lexOut = lexer->ResolveNext();
			token = lexOut.first;
		}
		// errors check
		return token;
	}
	else
	{
		const auto out = std::move(unusedTokens.front());
		unusedTokens.pop();
		return out;
	}
}

std::optional<LexToken> Parser::GetExpectedToken(const LexToken::TokenType expectedToken)
{
	const auto token = GetNextToken();
	if (token.GetType() != expectedToken)
	{
		unusedTokens.push(token);
		return std::nullopt;
	}
	return token;
}

bool Parser::ConsumeToken(const LexToken::TokenType expectedToken, std::optional<LexToken> boundTokenToReset)
{
	const auto token = GetNextToken();
	if (token.GetType() != expectedToken)
	{
		if (boundTokenToReset)
		{
			unusedTokens.push(*boundTokenToReset);
		}
		unusedTokens.push(token);

		return false;
	}
	return true;
}

// program = { function_definition };
Program Parser::ParseProgram()
{
	Program program;
	try
	{
		while (auto funDef = ParseFunctionDefinition())
		{
			program.funDefs.push_back(std::move(funDef));
		}
	}
	catch (const ParserException& pe)
	{
		std::cout << "Error when parsing: " << pe.what() << "\n";
	}

	return program;
}

// function_definition = "func", identifier, "(", parameters, ")", block;
std::unique_ptr<FunctionDefiniton> Parser::ParseFunctionDefinition()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::Func))
	{
		return nullptr;
	}

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		throw ParserException("Expected idenitfier after \"func\" keyword.");
	}

	auto functionDefinition = std::make_unique<FunctionDefiniton>();

	functionDefinition->identifier = std::get<std::wstring>(idToken->GetValue());

	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected \"(\" after identifier.");
	}
	functionDefinition->parameters = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected \")\" to close the params list.");
	}

	functionDefinition->block = ParseBlock();

	return functionDefinition;
}

// parameters = [parameter, { ",", parameter }];
std::vector<std::unique_ptr<Param>> Parser::ParseParams()
{
	std::vector<std::unique_ptr<Param>> params;

	auto param = ParseParam();
	if (!param)
	{
		return params;
	}

	params.push_back(std::move(param));

	while (ConsumeToken(LexToken::TokenType::Comma))
	{
		param = ParseParam();
		if (!param)
		{
			throw ParserException("Expected parameter after coma.");
		}
		params.push_back(std::move(param));
	}

	return params;
}

// parameter = ["mut"], identifier
std::unique_ptr<Param> Parser::ParseParam()
{
	using LT = LexToken::TokenType;

	auto param = std::make_unique<Param>();
	param->paramMutable = ConsumeToken(LT::Mut);

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		if (param->paramMutable)
		{
			throw ParserException("Expected identifier after \"mut\" keyword.");
		}
		return nullptr;
	}
	param->identifier = std::get<std::wstring>(idToken->GetValue());
	return param;
}

// block = "{", { statement }, "}";
std::unique_ptr<Block> Parser::ParseBlock()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LBracket))
	{
		return nullptr;
	}

	std::vector<std::unique_ptr<Statement>> statements;

	while (auto statement = ParseStatement())
	{
		statements.push_back(std::move(statement));
	}

	if (!ConsumeToken(LT::RBracket))
	{
		throw ParserException("Expected block closing bracket.");
	}
	return std::make_unique<Block>(std::move(statements));
}

//statement = function_call, ";"
//			| conditional
//			| loop
//			| return_statement
//			| block
//			| declaration
//			| assignment
std::unique_ptr<Statement> Parser::ParseStatement()
{
	if (auto funcCall = ParseFunctionCall())
	{
		if (!ConsumeToken(LexToken::TokenType::Semicolon))
		{
			throw ParserException("Expected semicolon at the end");
		}
		return funcCall;
	}
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
	if (auto assignment = ParseAssignment())
	{
		return assignment;
	}

	return nullptr;
}

// function_call = identifier, "(", arguments, ")";
std::unique_ptr<FunctionCall> Parser::ParseFunctionCall()
{
	using LT = LexToken::TokenType;

	auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		return nullptr;
	}

	if (!ConsumeToken(LT::LParenth, idToken))
	{
		return nullptr;
	}

	auto arguments = ParseArguments();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses in the function call.");
	}

	return std::make_unique<FunctionCall>(std::get<std::wstring>(idToken->GetValue()), std::move(arguments));
}

// conditional = "if", "(", expression, ")", block, ["else", block];
std::unique_ptr<Conditional> Parser::ParseConditional()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::If))
	{
		return nullptr;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected opening parentheses after if.");
	}
	auto conditional = std::make_unique<Conditional>();

	conditional->condition = ParseExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after condition.");
	}

	conditional->ifBlock = ParseBlock();

	if (ConsumeToken(LT::Else))
	{
		conditional->elseBlock = ParseBlock();
	}

	return conditional;
}

// loop = "while", "(", expression, ")", block
std::unique_ptr<WhileLoop> Parser::ParseLoop()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::While))
	{
		return nullptr;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected opening parentheses after while.");
	}

	auto whileLoop = std::make_unique<WhileLoop>();

	whileLoop->condition = ParseExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after condition.");
	}

	whileLoop->block = ParseBlock();

	return whileLoop;
}

// return_statement = "return", [expression], ";";
std::unique_ptr<Return> Parser::ParseReturn()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::Return))
	{
		return nullptr;
	}

	auto returnStatement = std::make_unique<Return>(ParseExpression());

	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon after return statement.");
	}
	return returnStatement;
}

// declaration = ["mut"], "var", identifier, ["=", expression], ";";
std::unique_ptr<Declaration> Parser::ParseDeclaration()
{
	using LT = LexToken::TokenType;

	auto declaration = std::make_unique<Declaration>();

	declaration->varMutable = ConsumeToken(LT::Mut);

	if (!ConsumeToken(LT::Var))
	{
		if (declaration->varMutable)
		{
			throw ParserException("Expected var keyword after mut keyword in declaration.");
		}
		return nullptr;
	}

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		throw ParserException("Expected identifier after var keyword.");
	}
	declaration->identifier = std::get<std::wstring>(idToken->GetValue());

	if (ConsumeToken(LT::Assign))
	{
		declaration->expression = ParseExpression();
		if (!declaration->expression)
		{
			throw ParserException("Expected expression after = in assignment.");
		}
	}

	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon at the end of declaration.");
	}

	return declaration;
}

// assignment = identifier, "=", expression, ";";
std::unique_ptr<Assignment> Parser::ParseAssignment()
{
	using LT = LexToken::TokenType;

	auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		return nullptr;
	}

	if (!ConsumeToken(LT::Assign, idToken))
	{
		return nullptr;
	}

	auto expression = ParseExpression();

	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon at the end of assignment.");
	}
	return std::make_unique<Assignment>(std::get<std::wstring>(idToken->GetValue()), std::move(expression));
}

// arguments = [expression, { ",", expression }];
std::vector<std::unique_ptr<Expression>> Parser::ParseArguments()
{
	std::vector<std::unique_ptr<Expression>> expressions;

	auto expression = ParseExpression();
	if (expression)
	{
		expressions.push_back(std::move(expression));
	}

	while (ConsumeToken(LexToken::TokenType::Comma))
	{
		expression = ParseExpression();

		if (!expression)
		{
			throw ParserException("Expected expression after comma in the arguments list.");
		}
		expressions.push_back(std::move(expression));
	}

	return expressions;
}

// expression = conjunction, { "||", conjunction }
//			 | "[", func_expression ,"]";
std::unique_ptr<Expression> Parser::ParseExpression()
{
	using LT = LexToken::TokenType;

	auto conjunction = ParseConjunction();
	if (conjunction)
	{
		std::vector<std::unique_ptr<Conjunction>> conjunctions;
		conjunctions.push_back(std::move(conjunction));
		while (ConsumeToken(LexToken::TokenType::LogicalOr))
		{
			auto conjunction = ParseConjunction();
			if (!conjunction)
			{
				throw ParserException("Expected expression after \"||\".");
			}

			conjunctions.push_back(std::move(conjunction));
		}
		return std::make_unique<Expression>(std::move(conjunctions));
	}
	else
	{
		if (ConsumeToken(LT::LSquareBracket))
		{
			auto fExpr = ParseFuncExpression();
			if (!fExpr)
			{
				throw ParserException("Expected function expression after \"[\".");
			}
			if (!ConsumeToken(LT::RSquareBracket))
			{
				throw ParserException("Expected \"]\" after function expression.");
			}
			return std::make_unique<Expression>(std::move(fExpr));
		}
	}
	return nullptr;
}

// conjunction = relation_term, { "&&", relation_term };
std::unique_ptr<Conjunction> Parser::ParseConjunction()
{
	auto relation = ParseRelation();
	if (!relation)
	{
		return nullptr;
	}

	auto conjunction = std::make_unique<Conjunction>();
	conjunction->relations.push_back(std::move(relation));
	while (ConsumeToken(LexToken::TokenType::LogicalAnd))
	{
		auto relation = ParseRelation();
		if (!relation)
		{
			throw ParserException("Expected expression after \"&&\".");
		}

		conjunction->relations.push_back(std::move(relation));
	}

	return conjunction;
}

// relation_term = additive_term, [relation_operator, additive_term];
std::unique_ptr<Relation> Parser::ParseRelation()
{
	using LT = LexToken::TokenType;
	auto additive = ParseAdditive();
	if (!additive)
	{
		return nullptr;
	}

	auto relation = std::make_unique<Relation>();
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
			throw ParserException("Expected expression after relation operator.");
		}
		relation->secondAdditive = std::move(nextAdditive);
	}

	return relation;
}

// additive_term = ["-"], (multiplicative_term, { ("+" | "-"), multiplicative_term });
std::unique_ptr<Additive> Parser::ParseAdditive()
{
	using LT = LexToken::TokenType;

	auto additive = std::make_unique<Additive>();

	additive->negated = ConsumeToken(LT::Minus);

	auto multiplicative = ParseMultiplicative();
	if (!multiplicative)
	{
		return nullptr;
	}

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

		multiplicative = ParseMultiplicative();
		if (!multiplicative)
		{
			throw ParserException("Expected expression after addition operator.");
		}

		additive->multiplicatives.push_back(std::move(multiplicative));
	}

	return additive;
}

// multiplicative_term = factor, { ("*" | "/"), factor };
std::unique_ptr<Multiplicative> Parser::ParseMultiplicative()
{
	using LT = LexToken::TokenType;

	auto factor = ParseFactor();
	if (!factor)
	{
		return nullptr;
	}

	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->factors.push_back(std::move(factor));

	while (true)
	{
		if (ConsumeToken(LT::Asterisk))
		{
			multiplicative->operators.push_back(MultiplicationOperator::Multiple);
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

		factor = ParseFactor();
		if (!factor)
		{
			throw ParserException("Expected expression after muliplication operator.");
		}

		multiplicative->factors.push_back(std::move(factor));
	}

	return multiplicative;
}

// factor = ["!"], (literal | "(", expression, ")" | identifier | function_call);
std::unique_ptr<Factor> Parser::ParseFactor()
{
	using LT = LexToken::TokenType;

	auto factor = std::make_unique<Factor>();
	factor->logicallyNegated = ConsumeToken(LT::NotEqual);

	auto literal = ParseLiteral();
	if (literal)
	{
		factor->factor = std::move(literal);
		return factor;
	}

	if (ConsumeToken(LT::LParenth))
	{
		auto expression = ParseExpression();
		factor->factor = std::move(expression);
		if (!ConsumeToken(LT::RParenth))
		{
			// error
		}
		return factor;
	}

	auto functionCall = ParseFunctionCall();
	if (functionCall)
	{
		factor->factor = std::move(functionCall);
		return factor;
	}

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		factor->factor = std::get<std::wstring>(idToken->GetValue());
		return factor;
	}

	if (factor->logicallyNegated)
	{
		throw ParserException("Expected expression after logical negation.");
	}

	return nullptr;
}

// literal = number | string | boolean;
std::unique_ptr<Literal> Parser::ParseLiteral()
{
	using LT = LexToken::TokenType;

	const auto integer = GetExpectedToken(LT::Integer);
	if (integer)
	{
		return std::make_unique<Literal>(std::get<int>(integer->GetValue()));
	}

	const auto floatNumber = GetExpectedToken(LT::Float);
	if (floatNumber)
	{
		return std::make_unique<Literal>(std::get<float>(floatNumber->GetValue()));
	}

	const auto string = GetExpectedToken(LT::String);
	if (string)
	{
		return std::make_unique<Literal>(std::get<std::wstring>(string->GetValue()));
	}

	const auto boolean = GetExpectedToken(LT::Boolean);
	if (boolean)
	{
		return std::make_unique<Literal>(std::get<bool>(boolean->GetValue()));
	}

	return nullptr;
}

// func_expression = composable, { ">>", composable };
std::unique_ptr<FuncExpression> Parser::ParseFuncExpression()
{
	auto composable = ParseComposable();
	if (!composable)
	{
		return nullptr;
	}

	std::vector<std::unique_ptr<Composable>> composables;
	composables.push_back(std::move(composable));
	while (ConsumeToken(LexToken::TokenType::FunctionCompose))
	{
		composable = ParseComposable();
		if (!composable)
		{
			throw;// error
		}

		composables.push_back(std::move(composable));
	}
	return std::make_unique<FuncExpression>(std::move(composables));
}

// composable = bindable, ["<<", "(", arguments, ")"];
std::unique_ptr<Composable> Parser::ParseComposable()
{
	using LT = LexToken::TokenType;

	auto bindable = ParseBindable();
	if (!bindable)
	{
		return nullptr;
	}

	auto composable = std::make_unique<Composable>();

	composable->bindable = std::move(bindable);

	if (ConsumeToken(LexToken::TokenType::FunctionBind))
	{
		if (!ConsumeToken(LT::LParenth))
		{
			// error
		}

		composable->arguments = ParseArguments();

		if (!ConsumeToken(LT::RParenth))
		{
			// error
		}
	}

	return composable;
}

// bindable = (function_lit | identifier | function_call | "(", func_expression, ")");
std::unique_ptr<Bindable> Parser::ParseBindable()
{
	using LT = LexToken::TokenType;

	auto funcLit = ParseFunctionLit();
	if (funcLit)
	{
		return std::make_unique<Bindable>(std::move(funcLit));
	}

	auto functionCall = ParseFunctionCall();
	if (functionCall)
	{
		return std::make_unique<Bindable>(std::move(functionCall));
	}

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		return std::make_unique<Bindable>(std::get<std::wstring>(idToken->GetValue()));
	}

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}
	auto bindable = std::make_unique<Bindable>(ParseFuncExpression());
	if (!bindable)
	{
		// error
	}

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	return bindable;
}

// function_lit = "(", parameters, ")", block;
std::unique_ptr<FunctionLit> Parser::ParseFunctionLit()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}
	auto functionLit = std::make_unique<FunctionLit>();
	functionLit->parameters = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	functionLit->block = ParseBlock();

	return functionLit;
}