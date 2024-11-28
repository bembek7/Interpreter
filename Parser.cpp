#include "Parser.h"

Parser::Parser(Lexer* const lexer) noexcept :
	lexer(lexer)
{}

void Parser::SetLexer(Lexer* const newLexer) noexcept
{
	lexer = newLexer;
}

Parser::Program Parser::ParseProgram()
{
	std::vector<FunctionDefiniton> funDefs;
	while (const auto funDef = ParseFunctionDefinition())
	{
		funDefs.push_back(funDef.value());
	}

	return Program();
}

LexToken Parser::GetNextToken()
{
	const auto lexOut = lexer->ResolveNext();
	const auto& token = std::move(lexOut.first);
	while (token.GetType() == LexToken::TokenType::Comment)
	{
		currToken = lexer.nextToken();
	}
	// errors check
	return token.first;
}

bool Parser::ConsumeToken(const LexToken::TokenType expectedToken) noexcept
{
	const auto idToken = GetNextToken();
	return idToken.GetType() == expectedToken;
}

// function_definition = "func", identifier, "(", parameters, ")", block;
std::optional<Parser::FunctionDefiniton> Parser::ParseFunctionDefinition()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::Func))
	{
		return std::nullopt;
	}

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		// error
	}

	if (!ConsumeToken(LT::LParenth))
	{
		// error
	}
	const auto params = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	const auto block = ParseBlock();

	return FunctionDefiniton();
}

// parameters = [parameter, { ",", parameter }];
std::vector<Parser::Param> Parser::ParseParams()
{
	std::vector<Param> params;

	auto param = ParseParam();
	if (!param)
	{
		return params;
	}

	params.push_back(param.value());

	while (ConsumeToken(LexToken::TokenType::Comma))
	{
		param = ParseParam();
		if (!param)
		{
			// error
		}
		params.push_back(param.value());
	}

	return params;
}

// parameter = ["mut"], identifier
std::optional<Parser::Param> Parser::ParseParam()
{
	using LT = LexToken::TokenType;

	if (ConsumeToken(LT::Mut))
	{
		// add mut
	}

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		// error
	}
	return std::optional<Param>();
}

// block = "{", { statement }, "}";
std::vector<Parser::Statement> Parser::ParseBlock()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LBracket))
	{
		// error
	}

	std::vector<Statement> statements;

	while (const auto statement = ParseStatement())
	{
		statements.push_back(statement.value());
	}

	if (!ConsumeToken(LT::RBracket))
	{
		// error
	}
	return statements;
}

//statement = function_call, ";"
//			| conditional
//			| loop
//			| return_statement
//			| block
//			| declaration
//			| assignment
std::optional<Parser::Statement> Parser::ParseStatement()
{
	if (const auto funcCall = ParseFunctionCall())
	{
		if (!ConsumeToken(LexToken::TokenType::Semicolon)
		{
			// error
		}
		return funcCall.value();
	}
	if (const auto conditional = ParseConditional())
	{
		return conditional.value();
	}
	if (const auto loop = ParseLoop())
	{
		return loop.value();
	}
	if (const auto returnStatement = ParseReturn())
	{
		return returnStatement.value();
	}
	if (const auto block = ParseBlock())
	{
		return block;
	}
	if (const auto declaration = ParseDeclaration())
	{
		return declaration.value();
	}
	if (const auto assignment = ParseAssignment())
	{
		return assignment.value();
	}

	return std::optional<Statement>();
}

// function_call = identifier, "(", arguments, ")";
std::optional<Parser::FunctionCall> Parser::ParseFunctionCall()
{
	using LT = LexToken::TokenType;

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		return std::nullopt;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		// error
	}

	const auto arguments = ParseArguments();
	if (!arguments)
	{
		// error
	}

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	return std::optional<FunctionCall>();
}

// conditional = "if", "(", expression, ")", block, ["else", block];
std::optional<Parser::Conditional> Parser::ParseConditional()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::If))
	{
		return std::nullopt;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		// error
	}

	const auto expression = ParseExpression();
	if (!expression)
	{
		// error
	}

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	const auto ifBlock = ParseBlock();
	if (!ifBlock)
	{
		// error
	}

	if (ConsumeToken(LT::Else))
	{
		const auto elseBlock = ParseBlock();
		if (!elseBlock)
		{
			// error
		}
	}

	return std::optional<Conditional>();
}

// loop = "while", "(", expression, ")", block
std::optional<Parser::Loop> Parser::ParseLoop()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::While))
	{
		return std::nullopt;
	}

	if (!ConsumeToken(LT::LParenth))
	{
		// error
	}

	const auto expression = ParseExpression();
	if (!expression)
	{
		// error
	}

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	const auto ifBlock = ParseBlock();
	if (!ifBlock)
	{
		// error
	}

	return std::optional<Loop>();
}

// return_statement = "return", [expression], ";";
std::optional<Parser::Return> Parser::ParseReturn()
{
	using LT = LexToken::TokenType;
	if (!ConsumeToken(LT::Return))
	{
		return std::nullopt;
	}

	const auto expression = ParseExpression();
	if (!expression)
	{

	}

	if (!ConsumeToken(LT::Semicolon))
	{
		return std::nullopt;
	}
	return std::optional<Return>();
}

// declaration = ["mut"], "var", identifier, ["=", expression], ";";
std::optional<Parser::Declaration> Parser::ParseDeclaration()
{
	using LT = LexToken::TokenType;
	if (ConsumeToken(LT::Mut))
	{

	}

	if (!ConsumeToken(LT::Var))
	{
		return std::nullopt;
	}

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		// error
	}

	if (ConsumeToken(LT::Assign))
	{
		const auto expresion = ParseExpression();
		if (!expresion)
		{
			// error
		}
	}

	if (!ConsumeToken(LT::Semicolon))
	{
		// error
	}

	return std::optional<Declaration>();
}

// assignment = identifier, "=", expression, ";";
std::optional<Parser::Assignment> Parser::ParseAssignment()
{
	using LT = LexToken::TokenType;

	const auto idToken = GetNextToken();
	if (idToken.GetType() != LT::Identifier)
	{
		return std::nullopt;
	}

	if (ConsumeToken(LT::Assign))
	{
		const auto expresion = ParseExpression();
		if (!expresion)
		{
			// error
		}
	}

	if (!ConsumeToken(LT::Semicolon))
	{
		// error
	}
}

// arguments = [expression, { ",", expression }];
std::optional<std::vector<Parser::Expression>> Parser::ParseArguments()
{
	std::vector<Expression> expressions;

	auto expression = ParseExpression();
	if (!expression)
	{
		return expressions;
	}

	while (ConsumeToken(LexToken::TokenType::Comma))
	{
		expression = ParseExpression();
		if (!expression)
		{
			// error
		}

		expressions.push_back(std::move(expression.value()));
	}
	
	return expressions;
}

// expression = conjunction, { "||", conjunction }
//			 | func_expression;
std::optional<Parser::Expression> Parser::ParseExpression()
{
	auto conjunction = ParseConjunction();
	if (conjunction)
	{
		std::vector<Conjunction> conjunctions;
		conjunctions.push_back(std::move(conjunction.value()));
		while (ConsumeToken(LexToken::TokenType::LogicalOr))
		{
			auto conjunction = ParseConjunction();
			if (!conjunction)
			{
				// error
			}

			conjunctions.push_back(std::move(conjunction.value()));
		}
		return conjunctions;
	}
	else
	{
		return ParseFuncExpression();
	}
	return std::optional<Expression>();
}

// conjunction = relation_term, { "&&", relation_term };
std::optional<Parser::Conjunction> Parser::ParseConjunction()
{
	auto relation = ParseRelation();
	if (!relation)
	{
		return std::nullopt;
	}

	std::vector<Relation> relations;
	relations.push_back(std::move(relation.value()));
	while (ConsumeToken(LexToken::TokenType::LogicalAnd))
	{
		auto relation = ParseRelation();
		if (!relation)
		{
			// error
		}

		relations.push_back(std::move(relation.value()));
	}
	return relations;
}

// relation_term = additive_term, [relation_operator, additive_term];
std::optional<Parser::Relation> Parser::ParseRelation()
{
	using LT = LexToken::TokenType;
	const auto additive = ParseAdditive();
	if (additive)
	{
		if(ConsumeToken(LT::Less) || ConsumeToken(LT::LessEqual) 
			|| ConsumeToken(LT::Greater) || ConsumeToken(LT::GreaterEqual)
			|| ConsumeToken(LT::Equal) || ConsumeToken(LT::NotEqual))
		{
			const auto nextAdditive = ParseAdditive();
			if (!nextAdditive)
			{
				// error
			}
		}

		else
		{
			return additive;
		}
	}

	return std::nullopt;
}

// additive_term = ["-"], (multiplicative_term, { ("+" | "-"), multiplicative_term });
std::optional<Parser::Additive> Parser::ParseAdditive()
{
	using LT = LexToken::TokenType;
	if (ConsumeToken(LT::Minus))
	{

	}

	auto multiplicative = ParseMultiplicative();
	if (!multiplicative)
	{
		return std::nullopt;
	}

	std::vector<Multiplicative> multiplicatives;
	multiplicatives.push_back(std::move(multiplicative.value()));
	while (ConsumeToken(LT::Plus) || ConsumeToken(LT::Minus))
	{
		multiplicative = ParseMultiplicative();
		if (!multiplicative)
		{
			// error
		}

		multiplicatives.push_back(std::move(multiplicative.value()));
	}
	return multiplicatives;
}

// multiplicative_term = factor, { ("*" | "/"), factor };
std::optional<Parser::Multiplicative> Parser::ParseMultiplicative()
{
	auto factor = ParseFactor();
	if (!factor)
	{
		return std::nullopt;
	}

	std::vector<Factor> factors;
	factors.push_back(std::move(factor.value()));
	while (ConsumeToken(LexToken::TokenType::LogicalAnd))
	{
		factor = ParseFactor();
		if (!factor)
		{
			// error
		}

		factors.push_back(std::move(factor.value()));
	}
	return factors;
}

// factor = ["!"], (literal | "(", expression, ")" | identifier | function_call);
std::optional<Parser::Factor> Parser::ParseFactor()
{
	using LT = LexToken::TokenType;
	if (ConsumeToken(LT::NotEqual))
	{

	}

	const auto literal = ParseLiteral();
	if (literal)
	{
		return std::nullopt;
	}

	if (ConsumeToken(LT::LParenth))
	{
		const auto expression = ParseExpression();
		if (!expression)
		{
			// error
		}
		if (!ConsumeToken(LT::RParenth))
		{
			// error
		}
	}

	const auto idToken = GetNextToken();
	if (idToken.GetType() == LT::Identifier)
	{
		
	}

	const auto functionCall = ParseFunctionCall();
	if (functionCall)
	{
		
	}

	return std::nullopt;
}

// literal = number | string | boolean;
std::optional<Parser::Literal> Parser::ParseLiteral()
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

	return std::nullopt;
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
	composables.push_back(std::move(composable.value()));
	while (ConsumeToken(LexToken::TokenType::FunctionCompose))
	{
		composable = ParseComposable();
		if (!composable)
		{
			// error
		}

		composables.push_back(std::move(composable.value()));
	}
	return composables;
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

	if(ConsumeToken(LexToken::TokenType::FunctionBind))
	{
		if (!ConsumeToken(LT::LParenth))
		{
			// error
		}

		const auto arguments = ParseArguments();
		if (!arguments)
		{
			// error
		}

		if (!ConsumeToken(LT::RParenth))
		{
			// error
		}
	}
	
	//
}

// bindable = (function_lit | identifier | func_expression);
std::optional<Parser::Bindable> Parser::ParseBindable()
{
	using LT = LexToken::TokenType;

	const auto funcLit = ParseFunctionLit();
	if (funcLit)
	{
		return funcLit;
	}

	const auto idToken = GetNextToken();
	if (idToken.GetType() == LT::Identifier)
	{
		return idToken;
	}

	return ParseFuncExpression();
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
}
