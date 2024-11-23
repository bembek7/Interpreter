#pragma once

namespace OfC
{
	bool WillMultiplicationOverflow(const int a, const int b) noexcept;
	bool WillAdditionOverflow(const int a, const int b) noexcept;
	bool WillMultiplicationOverflow(const float a, const float b) noexcept;
	bool WillAdditionOverflow(const float a, const float b) noexcept;
}