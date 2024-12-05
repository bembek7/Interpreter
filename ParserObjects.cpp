#include "ParserObjects.h"

bool operator==(const Statement& lhs, const Statement& rhs)
{
	return typeid(lhs) == typeid(rhs) && lhs.isEqual(rhs);
}