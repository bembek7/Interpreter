#include "Value.h"

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

bool Value::ToBool() const
{
	if (std::holds_alternative<bool>(value))
	{
		return std::get<bool>(value);
	}
	else if (std::holds_alternative<std::wstring>(value))
	{
		if (std::get<std::wstring>(value) == L"true" || std::get<std::wstring>(value) == L"false")
		{
			return std::get<std::wstring>(value) == L"true";
		}
	}
	throw; // error
}

Value Value::operator-() const
{
	if (std::holds_alternative<int>(value))
	{
		return Value(-std::get<int>(value));
	}
	else if (std::holds_alternative<float>(value))
	{
		return Value(-std::get<float>(value));
	}
	throw; // error
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
	if (std::holds_alternative<int>(value) && std::holds_alternative<float>(other.value))
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
		else if (auto floatValue = TryConvertToFloat(str))
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
		else if (auto floatValue = TryConvertToFloat(str))
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
		else if (auto floatValue = TryConvertToFloat(str))
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
		else if (auto floatValue = TryConvertToFloat(str))
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
	throw; // error
}

Value Value::operator+=(const Value& other)
{
	*this = *this + other;
	return *this;
}

Value Value::operator-(const Value& other) const
{
	return Value(false);
}

Value Value::operator-=(const Value& other)
{
	return Value(false);
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
	throw; // error
}

bool Value::operator!=(const Value& other) const
{
	return !(*this == other);
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
	throw; // error
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
	throw; // error
}

bool Value::Compare(const bool boolVal, const std::wstring& str)
{
	if (str == L"true" || str == L"false")
	{
		return str == L"true";
	}
	throw; // error
}