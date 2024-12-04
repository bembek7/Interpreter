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
	if (!lastUnusedToken)
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
		const auto out = std::move(*lastUnusedToken);
		lastUnusedToken = std::nullopt;
		return out;
	}
}

std::optional<LexToken> Parser::GetExpectedToken(const LexToken::TokenType expectedToken)
{
	const auto token = GetNextToken();
	if (token.GetType() != expectedToken)
	{
		lastUnusedToken = token;

		return std::nullopt;
	}
	return token;
}

bool Parser::ConsumeToken(const LexToken::TokenType expectedToken)
{
	const auto token = GetNextToken();
	if (token.GetType() != expectedToken)
	{
		lastUnusedToken = token;

		return false;
	}
	return true;
}

// program = { function_definition };
Parser::Program Parser::ParseProgram()
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
std::unique_ptr<Parser::FunctionDefiniton> Parser::ParseFunctionDefinition()
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
std::vector<std::unique_ptr<Parser::Param>> Parser::ParseParams()
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
std::unique_ptr<Parser::Param> Parser::ParseParam()
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
std::unique_ptr<Parser::Block> Parser::ParseBlock()
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
std::unique_ptr<Parser::Statement> Parser::ParseStatement()
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
std::unique_ptr<Parser::FunctionCall> Parser::ParseFunctionCall()
{
	using LT = LexToken::TokenType;

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		return nullptr;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		throw ParserException("Expected opening parentheses after identifier in the function call.");
	}

	auto arguments = ParseArguments();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses in the function call.");
	}

	return std::make_unique<FunctionCall>(std::get<std::wstring>(idToken->GetValue()), std::move(arguments));
}

// conditional = "if", "(", expression, ")", block, ["else", block];
std::unique_ptr<Parser::Conditional> Parser::ParseConditional()
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
std::unique_ptr<Parser::WhileLoop> Parser::ParseLoop()
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
std::unique_ptr<Parser::Return> Parser::ParseReturn()
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
std::unique_ptr<Parser::Declaration> Parser::ParseDeclaration()
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
std::unique_ptr<Parser::Assignment> Parser::ParseAssignment()
{
	using LT = LexToken::TokenType;

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		return nullptr;
	}

	if (ConsumeToken(LT::Assign))
	{
		auto expression = ParseExpression();

		if (!ConsumeToken(LT::Semicolon))
		{
			throw ParserException("Expected semicolon at the end of assignment.");
		}
		return std::make_unique<Assignment>(std::get<std::wstring>(idToken->GetValue()), std::move(expression));
	}

	throw ParserException("Expected \"=\" after identifier in the assignment.");
}

// arguments = [expression, { ",", expression }];
std::vector<std::unique_ptr<Parser::Expression>> Parser::ParseArguments()
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
//			 | func_expression;
std::unique_ptr<Parser::Expression> Parser::ParseExpression()
{
	auto conjunction = ParseConjunction();
	if (conjunction)
	{
		auto expression = std::make_unique<Expression>();
		expression->conjunctions.push_back(std::move(conjunction));
		while (ConsumeToken(LexToken::TokenType::LogicalOr))
		{
			auto conjunction = ParseConjunction();
			if (!conjunction)
			{
				throw ParserException("Expected expression after \"||\".");
			}

			expression->conjunctions.push_back(std::move(conjunction));
		}
		return expression;
	}
	else
	{
		//return ParseFuncExpression();
	}
	return nullptr;
}

// conjunction = relation_term, { "&&", relation_term };
std::unique_ptr<Parser::Conjunction> Parser::ParseConjunction()
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
std::unique_ptr<Parser::Relation> Parser::ParseRelation()
{
	using LT = LexToken::TokenType;
	auto additive = ParseAdditive();
	if (!additive)
	{
		return nullptr;
	}

	auto relation = std::make_unique<Relation>();
	relation->firstAdditive = std::move(additive);

	if (ConsumeToken(LT::Less) || ConsumeToken(LT::LessEqual)
		|| ConsumeToken(LT::Greater) || ConsumeToken(LT::GreaterEqual)
		|| ConsumeToken(LT::Equal) || ConsumeToken(LT::NotEqual))
	{
		// proper realtion term resolving
		relation->relationOperator = RelationOperator::Equal;

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
std::unique_ptr<Parser::Additive> Parser::ParseAdditive()
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

	while (ConsumeToken(LT::Plus) || ConsumeToken(LT::Minus))
	{
		//resolve the operator
		additive->operators.push_back(AdditionOperator::Plus);

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
std::unique_ptr<Parser::Multiplicative> Parser::ParseMultiplicative()
{
	using LT = LexToken::TokenType;

	auto factor = ParseFactor();
	if (!factor)
	{
		return nullptr;
	}

	auto multiplicative = std::make_unique<Multiplicative>();
	multiplicative->factors.push_back(std::move(factor));

	while (ConsumeToken(LT::Asterisk) || ConsumeToken(LT::Slash))
	{
		// resolve operator
		multiplicative->operators.push_back(MultiplicationOperator::Multiple);

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
std::unique_ptr<Parser::Factor> Parser::ParseFactor()
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

	//if (ConsumeToken(LT::LParenth))
	//{
	//	auto expression = ParseExpression();
	//	factor.factor = std::move(expression);
	//	if (!ConsumeToken(LT::RParenth))
	//	{
	//		// error
	//	}
	//	return factor;
	//}

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		factor->factor = std::get<std::wstring>(idToken->GetValue());
		return factor;
	}

	/*auto functionCall = ParseFunctionCall();
	if (functionCall)
	{
		factor.factor = *(functionCall.release());
		return factor;
	}*/

	if (factor->logicallyNegated)
	{
		throw ParserException("Expected expression after logical negation.");
	}

	return nullptr;
}

// literal = number | string | boolean;
std::unique_ptr<Parser::Literal> Parser::ParseLiteral()
{
	using LT = LexToken::TokenType;

	const auto integer = GetExpectedToken(LT::Integer);
	if (integer)
	{
	}

	const auto floatNumber = GetExpectedToken(LT::Float);
	if (floatNumber)
	{
	}

	const auto string = GetExpectedToken(LT::String);
	if (string)
	{
	}

	const auto boolean = GetExpectedToken(LT::Boolean);
	if (boolean)
	{
	}

	return nullptr;
}

// func_expression = composable, { ">>", composable };
std::optional<Parser::FuncExpression> Parser::ParseFuncExpression()
{
	auto composable = ParseComposable();
	if (!composable)
	{
		return std::nullopt;
	}

	std::vector<Composable> composables;
	composables.push_back(std::move(*composable));
	while (ConsumeToken(LexToken::TokenType::FunctionCompose))
	{
		composable = ParseComposable();
		if (!composable)
		{
			throw;// error
		}

		composables.push_back(std::move(*composable));
	}
	//return composables;
	return std::nullopt;
}

// composable = bindable, ["<<", "(", arguments, ")"];
std::optional<Parser::Composable> Parser::ParseComposable()
{
	using LT = LexToken::TokenType;

	auto bindable = ParseBindable();
	if (!bindable)
	{
		return std::nullopt;
	}

	if (ConsumeToken(LexToken::TokenType::FunctionBind))
	{
		if (!ConsumeToken(LT::LParenth))
		{
			// error
		}

		const auto arguments = ParseArguments();

		if (!ConsumeToken(LT::RParenth))
		{
			// error
		}
	}

	// CHANGE
	return std::nullopt;
}

// bindable = (function_lit | identifier | func_expression);
std::optional<Parser::Bindable> Parser::ParseBindable()
{
	using LT = LexToken::TokenType;

	const auto funcLit = ParseFunctionLit();
	if (funcLit)
	{
		//return funcLit;
	}

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		//return idToken;
	}

	//return ParseFuncExpression();
	return std::nullopt;
}

// function_lit = "(", parameters, ")", block;
std::optional<Parser::FunctionLit> Parser::ParseFunctionLit()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LParenth))
	{
		return std::nullopt;
	}
	const auto params = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	const auto block = ParseBlock();

	return std::nullopt;
}