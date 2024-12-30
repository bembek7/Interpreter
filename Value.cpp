#include "Value.h"
#include <stdexcept>
#include "ParserObjects\Core.h"
#include "Interpreter.h"

Value::Value(const Function& function) noexcept :
	value(function)
{
}

Value::Value(const bool val) noexcept :
	value(val)
{
}

Value::Value(const int val) noexcept :
	value(val)
{
}

Value::Value(const float val) noexcept :
	value(val)
{
}

Value::Value(const std::wstring& val) noexcept :
	value(val)
{
}

std::wstring Value::ToString() const
{
	if (std::holds_alternative<int>(value))
	{
		return std::to_wstring(std::get<int>(value));
	}
	if (std::holds_alternative<float>(value))
	{
		return std::to_wstring(std::get<float>(value));
	}
	if (std::holds_alternative<bool>(value))
	{
		return std::get<bool>(value) ? L"true" : L"false";
	}
	if (std::holds_alternative<std::wstring>(value))
	{
		return std::get<std::wstring>(value);
	}
	throw ValueException("Cannot convert value to string");
}

std::wstring Value::ToPrintString() const
{
	if (std::holds_alternative<int>(value) || std::holds_alternative<float>(value) || std::holds_alternative<bool>(value) || std::holds_alternative<std::wstring>(value))
	{
		return ToString();
	}
	if (std::holds_alternative<Function>(value))
	{
		return L"Function";
	}
	throw ValueException("Cannot print value");
}

bool Value::ToBool() const
{
	if (std::holds_alternative<bool>(value))
	{
		return std::get<bool>(value);
	}
	if (std::holds_alternative<std::wstring>(value))
	{
		if (std::get<std::wstring>(value) == L"true" || std::get<std::wstring>(value) == L"false")
		{
			return std::get<std::wstring>(value) == L"true";
		}
	}
	throw ValueException("Cannot convert value to bool");
}

const Value::Function* Value::GetFunction() const noexcept
{
	if (std::holds_alternative<Function>(value))
	{
		return &std::get<Function>(value);
	}
	return nullptr;
}

Value Value::operator-() const
{
	if (std::holds_alternative<int>(value))
	{
		return Value(-std::get<int>(value));
	}
	if (std::holds_alternative<float>(value))
	{
		return Value(-std::get<float>(value));
	}
	throw ValueException("Operator not supported for this value type.");
}

Value Value::operator!() const
{
	return Value(!this->ToBool());
}

Value Value::operator&&(const Value& other) const
{
	return this->ToBool() && other.ToBool();
}

Value Value::operator&=(const Value& other)
{
	*this = *this && other;
	return *this;
}

Value Value::operator||(const Value& other) const
{
	return this->ToBool() || other.ToBool();
}

Value Value::operator|=(const Value& other)
{
	*this = *this || other;
	return *this;
}

Value Value::operator+(const Value& other) const
{
	if (std::holds_alternative<int>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<int>(value) + std::get<int>(other.value);
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<float>(value) + std::get<float>(other.value);
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
	{
		return Value(static_cast<float>(std::get<int>(value)) + std::get<float>(other.value));
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<int>(other.value))
	{
		return Value(std::get<float>(value) + static_cast<float>(std::get<int>(other.value)));
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto integerValue = std::get<int>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue + *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue + *floatValue);
		}
		return Value(std::to_wstring(integerValue) + str);
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<int>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto integerValue = std::get<int>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue + *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue + *floatValue);
		}
		return Value(str + std::to_wstring(integerValue));
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto floatVal = std::get<float>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal + *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal + *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<float>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto floatVal = std::get<float>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal + *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal + *floatValue);
		}
	}
	else if (std::holds_alternative<bool>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		return Value((std::get<bool>(value) ? L"true" : L"false") + std::get<std::wstring>(other.value));
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<bool>(other.value))
	{
		return Value(std::get<std::wstring>(value) + (std::get<bool>(value) ? L"true" : L"false"));
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		return std::get<std::wstring>(value) + std::get<std::wstring>(other.value);
	}
	throw ValueException("Operator not supported for these value type.");
}

Value Value::operator+=(const Value& other)
{
	*this = *this + other;
	return *this;
}

Value Value::operator-(const Value& other) const
{
	if (std::holds_alternative<int>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<int>(value) - std::get<int>(other.value);
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<float>(value) - std::get<float>(other.value);
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
	{
		return Value(static_cast<float>(std::get<int>(value)) - std::get<float>(other.value));
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<int>(other.value))
	{
		return Value(std::get<float>(value) - static_cast<float>(std::get<int>(other.value)));
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto integerValue = std::get<int>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue - *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue - *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<int>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto integerValue = std::get<int>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue - *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue - *floatValue);
		}
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto floatVal = std::get<float>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal - *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal - *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<float>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto floatVal = std::get<float>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal - *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal - *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str1 = std::get<std::wstring>(value);
		const auto intVal1 = TryConvertToInt(str1);
		const auto floatVal1 = TryConvertToFloat(str1);
		const auto& str2 = std::get<std::wstring>(other.value);
		const auto intVal2 = TryConvertToInt(str2);
		const auto floatVal2 = TryConvertToFloat(str2);
		if (intVal1)
		{
			if (intVal2)
			{
				return Value(*intVal1 - *intVal2);
			}
			else if (floatVal2)
			{
				return Value(*intVal1 - *floatVal2);
			}
		}
		else if (floatVal1)
		{
			if (intVal2)
			{
				return Value(*floatVal1 - *intVal2);
			}
			else if (floatVal2)
			{
				return Value(*floatVal1 - *floatVal2);
			}
		}
	}
	throw ValueException("Operator not supported for these value type.");
}

Value Value::operator-=(const Value& other)
{
	*this = *this - other;
	return *this;
}

Value Value::operator*(const Value& other) const
{
	if (std::holds_alternative<int>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<int>(value) * std::get<int>(other.value);
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<float>(value) * std::get<float>(other.value);
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
	{
		return Value(std::get<int>(value) * std::get<float>(other.value));
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<int>(other.value))
	{
		return Value(std::get<float>(value) * std::get<int>(other.value));
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto integerValue = std::get<int>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue * *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue * *floatValue);
		}
		if (integerValue >= 0)
		{
			return Value(MultiplyString(integerValue, str));
		}
		throw ValueException("Operator not supported for these value type.");
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<int>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto integerValue = std::get<int>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue * *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue * *floatValue);
		}
		if (integerValue >= 0)
		{
			return Value(MultiplyString(integerValue, str));
		}
		throw ValueException("Operator not supported for these value type.");
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto floatVal = std::get<float>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal * *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal * *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<float>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto floatVal = std::get<float>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal * *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal * *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str1 = std::get<std::wstring>(value);
		const auto intVal1 = TryConvertToInt(str1);
		const auto floatVal1 = TryConvertToFloat(str1);
		const auto& str2 = std::get<std::wstring>(other.value);
		const auto intVal2 = TryConvertToInt(str2);
		const auto floatVal2 = TryConvertToFloat(str2);
		if (intVal1)
		{
			if (intVal2)
			{
				return Value(*intVal1 * *intVal2);
			}
			else if (floatVal2)
			{
				return Value(*intVal1 * *floatVal2);
			}
		}
		else if (floatVal1)
		{
			if (intVal2)
			{
				return Value(*floatVal1 * *intVal2);
			}
			else if (floatVal2)
			{
				return Value(*floatVal1 * *floatVal2);
			}
		}
	}
	throw ValueException("Operator not supported for these value type.");
}

Value Value::operator*=(const Value& other)
{
	*this = *this * other;
	return *this;
}

Value Value::operator/(const Value& other) const
{
	if (std::holds_alternative<int>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<int>(value) / std::get<int>(other.value);
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<float>(value) / std::get<float>(other.value);
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
	{
		return Value(std::get<int>(value) / std::get<float>(other.value));
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<int>(other.value))
	{
		return Value(std::get<float>(value) / std::get<int>(other.value));
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto integerValue = std::get<int>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue / *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue / *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<int>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto integerValue = std::get<int>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(integerValue / *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(integerValue / *floatValue);
		}
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str = std::get<std::wstring>(other.value);
		const auto floatVal = std::get<float>(value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal / *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal / *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<float>(other.value))
	{
		const auto& str = std::get<std::wstring>(value);
		const auto floatVal = std::get<float>(other.value);
		if (auto intValue = TryConvertToInt(str))
		{
			return Value(floatVal / *intValue);
		}
		if (auto floatValue = TryConvertToFloat(str))
		{
			return Value(floatVal / *floatValue);
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		const auto& str1 = std::get<std::wstring>(value);
		const auto intVal1 = TryConvertToInt(str1);
		const auto floatVal1 = TryConvertToFloat(str1);
		const auto& str2 = std::get<std::wstring>(other.value);
		const auto intVal2 = TryConvertToInt(str2);
		const auto floatVal2 = TryConvertToFloat(str2);
		if (intVal1)
		{
			if (intVal2)
			{
				return Value(*intVal1 / *intVal2);
			}
			else if (floatVal2)
			{
				return Value(*intVal1 / *floatVal2);
			}
		}
		else if (floatVal1)
		{
			if (intVal2)
			{
				return Value(*floatVal1 / *intVal2);
			}
			else if (floatVal2)
			{
				return Value(*floatVal1 / *floatVal2);
			}
		}
	}
	throw ValueException("Operator not supported for these value type.");
}

Value Value::operator/=(const Value& other)
{
	*this = *this / other;
	return *this;
}

bool Value::operator==(const Value& other) const
{
	if (std::holds_alternative<int>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<int>(value) == std::get<int>(other.value);
	}
	if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<int>(value) == std::get<float>(other.value);
	}
	if (std::holds_alternative<float>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<float>(value) == std::get<float>(other.value);
	}
	if (std::holds_alternative<float>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<float>(value) == std::get<int>(other.value);
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		return Compare(std::get<int>(value), std::get<std::wstring>(other.value));
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<int>(other.value))
	{
		return Compare(std::get<int>(other.value), std::get<std::wstring>(value));
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		return Compare(std::get<float>(value), std::get<std::wstring>(other.value));
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<float>(other.value))
	{
		return Compare(std::get<float>(other.value), std::get<std::wstring>(value));
	}
	else if (std::holds_alternative<bool>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		return std::get<bool>(value) == other.ToBool();
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<bool>(other.value))
	{
		return std::get<bool>(other.value) == this->ToBool();
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		return std::get<std::wstring>(other.value) == std::get<std::wstring>(value);
	}
	else if (std::holds_alternative<bool>(value) && std::holds_alternative<bool>(other.value))
	{
		return std::get<bool>(other.value) == std::get<bool>(value);
	}
	throw ValueException("Operator not supported for these value type.");
}

bool Value::operator!=(const Value& other) const
{
	return !(*this == other);
}

bool Value::operator>(const Value& other) const
{
	if (std::holds_alternative<int>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<int>(value) > std::get<int>(other.value);
	}
	if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<int>(value) > std::get<float>(other.value);
	}
	if (std::holds_alternative<float>(value) && std::holds_alternative<float>(other.value))
	{
		return std::get<float>(value) > std::get<float>(other.value);
	}
	if (std::holds_alternative<float>(value) && std::holds_alternative<int>(other.value))
	{
		return std::get<float>(value) > std::get<int>(other.value);
	}
	else if (std::holds_alternative<int>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		if (auto intVal = TryConvertToInt(std::get<std::wstring>(other.value)))
		{
			return std::get<int>(value) > *intVal;
		}
		if (auto floatVal = TryConvertToFloat(std::get<std::wstring>(other.value)))
		{
			return std::get<int>(value) > *floatVal;
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<int>(other.value))
	{
		if (auto intVal = TryConvertToInt(std::get<std::wstring>(value)))
		{
			return std::get<int>(other.value) > *intVal;
		}
		if (auto floatVal = TryConvertToFloat(std::get<std::wstring>(other.value)))
		{
			return std::get<int>(other.value) > *floatVal;
		}
	}
	else if (std::holds_alternative<float>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		if (auto intVal = TryConvertToInt(std::get<std::wstring>(other.value)))
		{
			return std::get<float>(value) > *intVal;
		}
		if (auto floatVal = TryConvertToFloat(std::get<std::wstring>(other.value)))
		{
			return std::get<float>(value) > *floatVal;
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<float>(other.value))
	{
		if (auto intVal = TryConvertToInt(std::get<std::wstring>(value)))
		{
			return std::get<float>(other.value) > *intVal;
		}
		if (auto floatVal = TryConvertToFloat(std::get<std::wstring>(other.value)))
		{
			return std::get<float>(other.value) > *floatVal;
		}
	}
	else if (std::holds_alternative<std::wstring>(value) && std::holds_alternative<std::wstring>(other.value))
	{
		auto intVal1 = TryConvertToInt(std::get<std::wstring>(value));
		auto intVal2 = TryConvertToInt(std::get<std::wstring>(other.value));
		auto floatVal1 = TryConvertToFloat(std::get<std::wstring>(value));
		auto floatVal2 = TryConvertToFloat(std::get<std::wstring>(other.value));

		if (intVal1)
		{
			if (intVal2)
			{
				return *intVal1 > *intVal2;
			}
			if (floatVal2)
			{
				return *intVal1 > *floatVal2;
			}
		}
		else if (floatVal1)
		{
			if (intVal2)
			{
				return *floatVal1 > *intVal2;
			}
			if (floatVal2)
			{
				return *floatVal1 > *floatVal2;
			}
		}
	}
	throw ValueException("Operator not supported for these value type.");
}

bool Value::operator>=(const Value& other) const
{
	return *this > other || *this == other;
}

bool Value::operator<(const Value& other) const
{
	return !(*this >= other);
}

bool Value::operator<=(const Value& other) const
{
	return *this < other || *this == other;
}

std::optional<int> Value::TryConvertToInt(const std::wstring& str)
{
	try
	{
		size_t pos;
		auto result = std::stoi(str, &pos);
		if (pos == str.size())
		{
			return result;
		}
		return std::nullopt;
	}
	catch (...)
	{
		return std::nullopt;
	}
}

std::optional<float> Value::TryConvertToFloat(const std::wstring& str)
{
	try
	{
		size_t pos;
		auto result = std::stof(str, &pos);
		if (pos == str.size())
		{
			return result;
		}
		return std::nullopt;
	}
	catch (...)
	{
		return std::nullopt;
	}
}

bool Value::Compare(const int intVal, const std::wstring& str)
{
	if (auto intValue = TryConvertToInt(str))
	{
		return intValue == intVal;
	}
	else if (auto floatValue = TryConvertToFloat(str))  // add better float comparison
	{
		return intValue == floatValue;
	}
	throw ValueException("String not convertible to number.");
}

bool Value::Compare(const float floatVal, const std::wstring& str)
{
	// add better float comparison
	if (auto intValue = TryConvertToInt(str))
	{
		return floatVal == intValue;
	}
	else if (auto floatValue = TryConvertToFloat(str))
	{
		return floatVal == floatValue;
	}
	throw ValueException("String not convertible to number.");
}

bool Value::Compare(const bool boolVal, const std::wstring& str)
{
	if (str == L"true" || str == L"false")
	{
		return str == L"true";
	}
	throw ValueException("String not convertible to bool.");
}

std::wstring Value::MultiplyString(const unsigned int count, const std::wstring& str)
{
	std::wstring mulStr = L"";
	for (size_t i = 0; i < count; i++)
	{
		mulStr += str;
	}
	return mulStr;
}

Value Value::operator>>(const Value& other) const
{
	if (std::holds_alternative<Function>(value) && std::holds_alternative<Function>(other.value))
	{
		if (std::get<Function>(other.value).parameters.size() > 1)
		{
			throw ValueException("Function that uses other in composition can have only one parameter");
		}
		auto func = std::get<Function>(other.value);
		func.composedOf = &std::get<Function>(value);
		return Value(func);
	}
	throw ValueException("Only function value supports '>>' operator");
}

Value Value::operator<<(const std::vector<Value>& arguments) const
{
	if (std::holds_alternative<Function>(value))
	{
		auto func = std::get<Function>(value);
		func.boundArguments.insert(func.boundArguments.end(), arguments.begin(), arguments.end());

		return Value(func);
	}
	throw ValueException("Only function value supports '<<' operator");
}