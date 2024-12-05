#include "OverflowChecks.h"
#include <limits>

bool OfC::WillMultiplicationOverflow(const int a, const int b) noexcept
{
	if (a == 0 || b == 0)
	{
		return false;
	}
	return a > std::numeric_limits<int>::max() / b;
}

bool OfC::WillAdditionOverflow(const int a, const int b) noexcept
{
	return b > std::numeric_limits<int>::max() - a;
}

bool OfC::WillMultiplicationOverflow(const float a, const float b) noexcept
{
	if (a == 0 || b == 0)
	{
		return false;
	}
	return a > std::numeric_limits<int>::max() / b;
}

bool OfC::WillAdditionOverflow(const float a, const float b) noexcept
{
	return b > std::numeric_limits<float>::max() - a;
}