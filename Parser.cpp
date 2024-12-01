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
		throw "Expected idenitfier after \"func\" keyword.";
	}

	if (!ConsumeToken(LT::LParenth))
	{
		throw "Expected \"(\" after identifier.";
	}
	const auto params = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		throw "Expected \")\" to close the params list.";
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

	const auto arguments = ParseArguments();
	if (!arguments)
	{
		// error
	}

	if (!ConsumeToken(LT::RParenth))
	{
		// error
	}

	return std::make_unique<FunctionCall>(std::get<std::wstring>(idToken.GetValue()), arguments.value());
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

	const auto expression = ParseExpression();
	if (!expression)
	{
		// error
	}

	auto conditional = std::make_unique<Conditional>();

	conditional->condition = expression.value();

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

	const auto expression = ParseExpression();
	if (!expression)
	{
		// error
	}

	auto whileLoop = std::make_unique<WhileLoop>();

	whileLoop->condition = expression.value();

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

	if (!returnStatement->expression)
	{
	}

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
		const auto expression = ParseExpression();
		if (!expression)
		{
			// error
		}

		if (!ConsumeToken(LT::Semicolon))
		{
			// error
		}
		return std::make_unique<Assignment>(std::get<std::wstring>(idToken.GetValue()), expression.value());
	}

	throw "Error";
	// error
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
		//return conjunctions;
	}
	else
	{
		//return ParseFuncExpression();
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
	//return relations;
	return std::nullopt;
}

// relation_term = additive_term, [relation_operator, additive_term];
std::optional<Parser::Relation> Parser::ParseRelation()
{
	using LT = LexToken::TokenType;
	const auto additive = ParseAdditive();
	if (additive)
	{
		if (ConsumeToken(LT::Less) || ConsumeToken(LT::LessEqual)
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
			//return additive;
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
	//return multiplicatives;
	return std::nullopt;
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
	//return factors;
	return std::nullopt;
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
		if (!arguments)
		{
			// error
		}

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