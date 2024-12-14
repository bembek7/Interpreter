#include "Parser.h"
#include <iostream>

Parser::Parser(Lexer* const lexer)
{
	SetLexer(lexer);
}

void Parser::SetLexer(Lexer* const newLexer)
{
	if (!newLexer)
	{
		throw std::runtime_error("Passed lexer was a nullptr");
	}
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
	else
	{
		const auto out = std::move(*lastUnusedToken);
		lastUnusedToken = std::nullopt;
		currentPosition = out.GetPosition();
		return out;
	}
}

std::optional<LexToken> Parser::GetExpectedToken(const LexToken::TokenType expectedToken)
{
	auto token = GetNextToken();
	if (token.GetType() != expectedToken)
	{
		lastUnusedToken = std::move(token);
		return std::nullopt;
	}
	return token;
}

bool Parser::ConsumeToken(const LexToken::TokenType expectedToken)
{
	return GetExpectedToken(expectedToken).has_value();
}

bool Parser::CheckToken(const LexToken::TokenType expectedToken)
{
	lastUnusedToken = GetNextToken();
	return lastUnusedToken->GetType() == expectedToken;
}

// program = { function_definition };
std::unique_ptr<Program> Parser::ParseProgram()
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
std::unique_ptr<FunctionDefiniton> Parser::ParseFunctionDefinition()
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

	const auto idToken = GetExpectedToken(LT::Identifier);
	if (!idToken)
	{
		throw ParserException("Expected idenitfier after \"func\" keyword.", currentPosition);
	}

	auto functionDefinition = std::make_unique<FunctionDefiniton>();

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

	return functionDefinition;
}

// parameters = [parameter, { ",", parameter }];
std::vector<Param> Parser::ParseParams()
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
std::optional<Param> Parser::ParseParam()
{
	using LT = LexToken::TokenType;

	auto param = Param();
	param.paramMutable = ConsumeToken(LT::Mut);

	const auto idToken = GetExpectedToken(LT::Identifier);
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
		throw ParserException("Expected block closing bracket.", currentPosition);
	}
	return std::make_unique<Block>(std::move(statements));
}

//statement = function_call_statement
//			| conditional
//			| loop
//			| return_statement
//			| block
//			| declaration
//			| assignment
std::unique_ptr<Statement> Parser::ParseStatement()
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
		const auto identifier = std::get<std::wstring>(idToken->GetValue());
		if (auto assignment = ParseRestOfAssignment(identifier))
		{
			return assignment;
		}
		if (auto funcCall = ParseRestOfFunctionCallStatement(identifier))
		{
			return funcCall;
		}
	}

	return nullptr;
}

// function_call_statement = function_call, ";";
std::unique_ptr<FunctionCallStatement> Parser::ParseRestOfFunctionCallStatement(const std::wstring& identifier)
{
	if (auto funcCall = ParseRestOfFunctionCall(identifier))
	{
		if (!ConsumeToken(LexToken::TokenType::Semicolon))
		{
			throw ParserException("Expected semicolon at the end", currentPosition);
		}
		return std::make_unique<FunctionCallStatement>(std::move(funcCall));
	}
	return nullptr;
}

std::unique_ptr<FunctionCall> Parser::ParseRestOfFunctionCall(const std::wstring& identifier)
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}

	auto arguments = ParseArguments();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses in the function call.", currentPosition);
	}

	return std::make_unique<FunctionCall>(identifier, std::move(arguments));
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
		throw ParserException("Expected opening parentheses after if.", currentPosition);
	}
	auto conditional = std::make_unique<Conditional>();

	conditional->condition = ParseStandardExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after condition.", currentPosition);
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
		throw ParserException("Expected opening parentheses after while.", currentPosition);
	}

	auto whileLoop = std::make_unique<WhileLoop>();

	whileLoop->condition = ParseStandardExpression();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after condition.", currentPosition);
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
		throw ParserException("Expected semicolon after return statement.", currentPosition);
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
			throw ParserException("Expected var keyword after mut keyword in declaration.", currentPosition);
		}
		return nullptr;
	}

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

	if (!ConsumeToken(LT::Semicolon))
	{
		throw ParserException("Expected semicolon at the end of declaration.", currentPosition);
	}

	return declaration;
}

// assignment = identifier, "=", expression, ";";
std::unique_ptr<Assignment> Parser::ParseRestOfAssignment(const std::wstring& identifier)
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
std::unique_ptr<Expression> Parser::ParseExpression()
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
std::unique_ptr<StandardExpression> Parser::ParseStandardExpression()
{
	auto conjunction = ParseConjunction();
	if (!conjunction)
	{
		return nullptr;
	}
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
	return std::make_unique<StandardExpression>(std::move(conjunctions));
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
			throw ParserException("Expected expression after relation operator.", currentPosition);
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
std::unique_ptr<Factor> Parser::ParseFactor()
{
	using LT = LexToken::TokenType;

	auto factor = std::make_unique<Factor>();
	factor->logicallyNegated = ConsumeToken(LT::LogicalNot);

	if (auto literal = ParseLiteral())
	{
		factor->factor = std::move(*literal);
		return factor;
	}

	if (ConsumeToken(LT::LParenth))
	{
		auto expression = ParseStandardExpression();
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
std::optional<Literal> Parser::ParseLiteral()
{
	using LT = LexToken::TokenType;

	if (const auto integer = GetExpectedToken(LT::Integer))
	{
		return Literal(std::get<int>(integer->GetValue()));
	}

	if (const auto floatNumber = GetExpectedToken(LT::Float))
	{
		return Literal(std::get<float>(floatNumber->GetValue()));
	}

	if (const auto string = GetExpectedToken(LT::String))
	{
		return Literal(std::get<std::wstring>(string->GetValue()));
	}
	;
	if (const auto boolean = GetExpectedToken(LT::Boolean))
	{
		return Literal(std::get<bool>(boolean->GetValue()));
	}

	return std::nullopt;
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
		if (composable = ParseComposable())
		{
			composables.push_back(std::move(composable));
		}
		else
		{
			throw ParserException("Expected composable after compose operator.", currentPosition);
		}
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
std::unique_ptr<Bindable> Parser::ParseBindable()
{
	using LT = LexToken::TokenType;

	if (auto funcLit = ParseFunctionLit())
	{
		return std::make_unique<Bindable>(std::move(funcLit));
	}

	auto idToken = GetExpectedToken(LT::Identifier);
	if (idToken)
	{
		const auto identifier = std::get<std::wstring>(idToken->GetValue());
		if (auto functionCall = ParseRestOfFunctionCall(identifier))
		{
			return std::make_unique<Bindable>(std::move(functionCall));
		}
		else
		{
			return std::make_unique<Bindable>(identifier);
		}
	}

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}
	auto bindable = std::make_unique<Bindable>(ParseFuncExpression());
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
std::unique_ptr<FunctionLiteral> Parser::ParseFunctionLit()
{
	using LT = LexToken::TokenType;

	if (!ConsumeToken(LT::LParenth))
	{
		return nullptr;
	}
	auto functionLit = std::make_unique<FunctionLiteral>();
	functionLit->parameters = ParseParams();

	if (!ConsumeToken(LT::RParenth))
	{
		throw ParserException("Expected closing parentheses after parameters list", currentPosition);
	}

	functionLit->block = ParseBlock();

	return functionLit;
}