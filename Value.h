#pragma once
#include <variant>
#include <string>
#include <optional>

struct Function { std::string b = "Function"; };
struct Value
{
	Value(const bool val) noexcept;
	Value(const int val) noexcept;
	Value(const float val) noexcept;
	Value(const std::wstring& val) noexcept;
	std::variant<bool, int, float, std::wstring, Function> value;

	bool ToBool() const;
	Value operator-() const;
	Value operator!() const;
	Value operator&&(const Value& other) const;
	Value operator&=(const Value& other);
	Value operator||(const Value& other) const;
	Value operator|=(const Value& other);
	Value operator+(const Value& other) const;
	Value operator+=(const Value& other);
	Value operator-(const Value& other) const;
	Value operator-=(const Value& other);
	Value operator*(const Value& other) const;
	Value operator*=(const Value& other);
	Value operator/(const Value& other) const;
	Value operator/=(const Value& other);
	bool operator==(const Value& other) const;
	bool operator!=(const Value& other) const;
	bool operator>(const Value& other) const;
	bool operator>=(const Value& other) const;
	bool operator<(const Value& other) const;
	bool operator<=(const Value& other) const;


private:
	static std::optional<int> TryConvertToInt(const std::wstring& str);
	static std::optional<float> TryConvertToFloat(const std::wstring& str);
	static bool Compare(const int intVal, const std::wstring& str);
	static bool Compare(const float floatVal, const std::wstring& str);
	static bool Compare(const bool boolVal, const std::wstring& str);
	static std::wstring MultiplyString(const int count, const std::wstring& str);
};