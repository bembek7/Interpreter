#pragma once

struct Position
{
	Position(const size_t line, const size_t column) noexcept :
		line(line), column(column) {}
	size_t line;
	size_t column;
};