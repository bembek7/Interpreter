#pragma once
#include <variant>
#include <string>
#include <optional>
#include "Position.h"
#include "InterpreterException.h"
#include <vector>
#include "ParserObjects\Core.h"
#include "ParserObjects\Statements.h"

class Value
{
public:
	struct Function
	{
		Function(Block* block, const std::vector<Param>& parameters) noexcept :
			block(block), parameters(parameters)
		{
		}

		Block* block;
		std::vector<Param> parameters;
		std::vector<Value> boundArguments;
		const Function* composedOf = nullptr;
	};
	class ValueException : public InterpreterException
	{
	public:
		ValueException(const char* msg)
			: InterpreterException(msg)
		{
			std::stringstream ss;
			ss << "Value Error : " << msg;
			message = ss.str();
		}
	};

	Value() = default;
	Value(const Function& function) noexcept;
	Value(const bool val) noexcept;
	Value(const int val) noexcept;
	Value(const float val) noexcept;
	Value(const std::wstring& val) noexcept;
	std::variant<bool, int, float, std::wstring, Function> value;

	std::wstring ToString() const;
	bool ToBool() const;
	const Function* GetFunction() const noexcept;

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

	Value operator>>(const Value& other) const;
	Value operator<<(const std::vector<Value>& arguments) const;

private:
	static std::optional<int> TryConvertToInt(const std::wstring& str);
	static std::optional<float> TryConvertToFloat(const std::wstring& str);
	static bool Compare(const int intVal, const std::wstring& str);
	static bool Compare(const float floatVal, const std::wstring& str);
	static bool Compare(const bool boolVal, const std::wstring& str);
	static std::wstring MultiplyString(const int count, const std::wstring& str);
};