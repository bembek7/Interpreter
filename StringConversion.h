#pragma once
#include <string>
#include <locale>
#include <codecvt>

namespace StringConversion
{
	std::string ToNarrow(const std::wstring& str)
	{
		if (str.empty()) return std::string();

		std::mbstate_t state = std::mbstate_t();
		const wchar_t* src = str.data();
#pragma warning ( suppress: 4996 )
		size_t len = 1 + std::wcsrtombs(nullptr, &src, 0, &state);

		if (len == static_cast<size_t>(-1))
		{
			throw std::runtime_error("Conversion error");
		}

		std::string result(len, '\0');
#pragma warning ( suppress: 4996 )
		std::wcsrtombs(&result[0], &src, len, &state);
		result.pop_back();

		return result;
	}
}
