#include "Parser.h"

Parser::Parser(Lexer* const lexer) noexcept :
	lexer(lexer)
{}

void Parser::SetLexer(Lexer* const newLexer) noexcept
{
	lexer = newLexer;
}

LexToken Parser::GetNextToken()
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

bool Parser::ConsumeToken(const LexToken::TokenType expectedToken) noexcept
{
	const auto idToken = GetNextToken();
	return idToken.GetType() == expectedToken;
}

// program = { function_definition };
Parser::Program Parser::ParseProgram()
{
	Program program;
	while (auto funDef = ParseFunctionDefinition())
	{
		program.funDefs.push_back(std::move(funDef));
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

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		throw "Expected idenitfier after \"func\" keyword.";
	}

	auto functionDefinition = std::make_unique<FunctionDefiniton>();

	functionDefinition->identifier = std::get<std::wstring>(idToken.GetValue());

	if (!ConsumeToken(LT::LParenth))
	{
		throw "Expected \"(\" after identifier.";
	}
	functionDefinition->parameters = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		throw "Expected \")\" to close the params list.";
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
			throw;// error
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

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		if (param->paramMutable)
		{
			throw;// error
		}
		return nullptr;
	}
	param->identifier = std::get<std::wstring>(idToken.GetValue());
	return param;
}

// block = "{", { statement }, "}";
std::unique_ptr<Parser::Block> Parser::ParseBlock()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LBracket))
	{
		// error
	}

	std::vector<std::unique_ptr<Statement>> statements;

	while (auto statement = ParseStatement())
	{
		statements.push_back(std::move(statement));
	}

	if (!ConsumeToken(LT::RBracket))
	{
		// error
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
			// error
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

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		return nullptr;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		// error
	}

	auto arguments = ParseArguments();

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	return std::make_unique<FunctionCall>(std::get<std::wstring>(idToken.GetValue()), std::move(arguments));
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
		// error
	}
	auto conditional = std::make_unique<Conditional>();

	conditional->condition = ParseExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		// error
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
		// error
	}

	auto whileLoop = std::make_unique<WhileLoop>();

	whileLoop->condition = ParseExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		// error
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
		// error
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
		return nullptr;
	}

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		// error
	}
	declaration->identifier = std::get<std::wstring>(idToken.GetValue());

	if (ConsumeToken(LT::Assign))
	{
		declaration->expression = ParseExpression();
		if (!declaration->expression)
		{
			// error
		}
	}

	if (!ConsumeToken(LT::Semicolon))
	{
		// error
	}

	return declaration;
}

// assignment = identifier, "=", expression, ";";
std::unique_ptr<Parser::Assignment> Parser::ParseAssignment()
{
	using LT = LexToken::TokenType;

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		return nullptr;
	}

	if (ConsumeToken(LT::Assign))
	{
		auto expression = ParseExpression();

		if (!ConsumeToken(LT::Semicolon))
		{
			// error
		}
		return std::make_unique<Assignment>(std::get<std::wstring>(idToken.GetValue()), std::move(expression));
	}

	throw "Error";
	// error
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
			throw; //error
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
				throw;// error
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
			throw;// error
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
			// error
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
			throw;// error
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
			throw;// error
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

	const auto idToken = GetNextToken();
	if (idToken.GetType() == LT::Identifier)
	{
		factor->factor = std::get<std::wstring>(idToken.GetValue());
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
		throw; //error
	}

	return nullptr;
}

// literal = number | string | boolean;
std::unique_ptr<Parser::Literal> Parser::ParseLiteral()
{
	using LT = LexToken::TokenType;

	const auto integer = GetNextToken();
	if (integer.GetType() == LT::Integer)
	{
	}

	const auto floatNumber = GetNextToken();
	if (floatNumber.GetType() == LT::Float)
	{
	}

	const auto string = GetNextToken();
	if (string.GetType() == LT::String)
	{
	}

	const auto boolean = GetNextToken();
	if (boolean.GetType() == LT::Boolean)
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

	const auto idToken = GetNextToken();
	if (idToken.GetType() == LT::Identifier)
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