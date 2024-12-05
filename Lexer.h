#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include "Position.h"
#include "LexToken.h"
#include "LexicalError.h"

class Lexer
{
public:
	Lexer(std::wistream* const  source) noexcept;
	void SetNewSource(std::wistream* const  newSource) noexcept;

	std::pair<std::vector<LexToken>, std::vector<LexicalError>> ResolveAllRemaining();
	std::pair<LexToken, std::vector<LexicalError>> ResolveNext();
private:
	LexToken BuildToken();

	std::optional<LexToken> TryBuildComment();
	std::optional<LexToken> TryBuildNumber();
	std::optional<LexToken> TryBuildWord();
	std::optional<LexToken> TryBuildSymbolsMix();
	std::optional<LexToken> TryBuildSingleSymbol();
	std::optional<LexToken> TryBuildTwoCharsOperator();
	std::optional<LexToken> TryBuildStringLiteral();

	bool SkipNumber(bool dotOccured);
	bool SkipComment();
	bool SkipStringLiteral();
	bool SkipIdentifier();

private:
	std::wistream* source = nullptr;
	std::vector<LexicalError> currentErrors;
	wchar_t currentChar = {};
	Position currentPosition = { 1, 1 };
};