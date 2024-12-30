#include "gtest/gtest.h"
#include "Value.h"

class ValueTest : public Value
{
public:
	static std::optional<int> TryConvertToInt(const std::wstring& str)
	{
		return Value::TryConvertToInt(str);
	}
	static std::optional<float> TryConvertToFloat(const std::wstring& str)
	{
		return Value::TryConvertToFloat(str);
	}
	static bool Compare(const int intVal, const std::wstring& str)
	{
		return Value::Compare(intVal, str);
	}
	static bool Compare(const float floatVal, const std::wstring& str)
	{
		return Value::Compare(floatVal, str);
	}
	static bool Compare(const bool boolVal, const std::wstring& str)
	{
		return Value::Compare(boolVal, str);
	}
	static std::wstring MultiplyString(const int count, const std::wstring& str)
	{
		return Value::MultiplyString(count, str);
	}
};

class ValueTests : public ::testing::Test
{
};

// Test unary minus operator
TEST(ValueTests, UnaryMinusInt)
{
	Value val(42);
	EXPECT_EQ(-val, Value(-42));
}

TEST(ValueTests, UnaryMinusFloat)
{
	Value val(3.14f);
	EXPECT_EQ(-val, Value(-3.14f));
}

// Test logical AND operator
TEST(ValueTests, LogicalAndOperator)
{
	Value val1(true), val2(true);
	EXPECT_EQ(val1 && val2, Value(true));

	val2 = Value(false);
	EXPECT_EQ(val1 && val2, Value(false));
}

// Test logical AND assignment operator
TEST(ValueTests, LogicalAndAssignmentOperator)
{
	Value val1(true), val2(false);
	val1 &= val2;
	EXPECT_EQ(val1, Value(false));
}

// Test logical OR operator
TEST(ValueTests, LogicalOrOperator)
{
	Value val1(false), val2(true);
	EXPECT_EQ(val1 || val2, Value(true));

	val1 = Value(true);
	EXPECT_EQ(val1 || val2, Value(true));
}

// Test logical OR assignment operator
TEST(ValueTests, LogicalOrAssignmentOperator)
{
	Value val1(false), val2(true);
	val1 |= val2;
	EXPECT_EQ(val1, Value(true));
}

// Test addition operator
TEST(ValueTests, AdditionOperatorIntFloat)
{
	Value val1(42), val2(3.14f);
	EXPECT_EQ(val1 + val2, Value(45.14f));
}

TEST(ValueTests, AdditionOperatorWStringInt)
{
	Value val1(std::wstring(L"Test "));
	Value val2(42);
	EXPECT_EQ(val1 + val2, Value(std::wstring(L"Test 42")));
}

TEST(ValueTests, AdditionOperatorWStringFloat)
{
	Value val1(std::wstring(L"3.0")), val2(3.14f);
	auto val3 = val1 + val2;
	EXPECT_FLOAT_EQ(std::get<float>(val3.value), std::get<float>(Value(6.14f).value));
}

// Test addition assignment operator
TEST(ValueTests, AdditionAssignmentOperator)
{
	Value val1(42), val2(3.14f);
	val1 += val2;
	EXPECT_EQ(val1, Value(45.14f));
}

// Test subtraction operator
TEST(ValueTests, SubtractionOperator) {
	Value val1(42), val2(10);
	EXPECT_EQ(val1 - val2, Value(32));

	val1 = Value(42.0f);
	val2 = Value(10.0f);
	EXPECT_EQ(val1 - val2, Value(32.0f));
}

// Test subtraction assignment operator
TEST(ValueTests, SubtractionAssignmentOperator) {
	Value val1(42), val2(10);
	val1 -= val2;
	EXPECT_EQ(val1, Value(32));
}

// Test equality operator
TEST(ValueTests, EqualityOperatorIntInt)
{
	Value val1(42), val2(42);
	EXPECT_TRUE(val1 == val2);

	val2 = Value(10);
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorIntFloat)
{
	Value val1(42), val2(42.0f);
	EXPECT_TRUE(val1 == val2);

	val2 = Value(10.0f);
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorWStringInt)
{
	Value val1(std::wstring(L"42")), val2(42);
	EXPECT_TRUE(val1 == val2);

	val2 = Value(10);
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorWStringFloat)
{
	Value val1(std::wstring(L"3.14")), val2(3.14f);
	EXPECT_TRUE(val1 == val2);

	val2 = Value(10.0f);
	EXPECT_FALSE(val1 == val2);
}

// Test inequality operator
TEST(ValueTests, InequalityOperator)
{
	Value val1(42), val2(10);
	EXPECT_TRUE(val1 != val2);

	val2 = Value(42);
	EXPECT_FALSE(val1 != val2);
}

// Test multiplication operator
TEST(ValueTests, Multiply_IntAndInt)
{
	Value a(5), b(3);
	Value result = a * b;
	EXPECT_EQ(result, Value(15));
}

TEST(ValueTests, Multiply_FloatAndFloat)
{
	Value a(2.5f), b(4.0f);
	Value result = a * b;
	EXPECT_EQ(result, Value(10.0f));
}

TEST(ValueTests, Multiply_IntAndFloat)
{
	Value a(6), b(2.5f);
	Value result = a * b;
	EXPECT_EQ(result, Value(15.0f));
}

TEST(ValueTests, Multiply_FloatAndInt)
{
	Value a(2.5f), b(6);
	Value result = a * b;
	EXPECT_EQ(result, Value(15.0f));
}

TEST(ValueTests, Multiply_IntAndString)
{
	Value a(3), b(std::wstring(L"abc"));
	Value result = a * b;
	EXPECT_EQ(result, Value(std::wstring(L"abcabcabc")));
}

TEST(ValueTests, Multiply_StringAndInt)
{
	Value a(std::wstring(L"abc")), b(3);
	Value result = a * b;
	EXPECT_EQ(result, Value(std::wstring(L"abcabcabc")));
}

// Test division operator
TEST(ValueTests, Divide_IntAndInt)
{
	Value a(10), b(2);
	Value result = a / b;
	EXPECT_EQ(result, Value(5));
}

TEST(ValueTests, Divide_FloatAndFloat)
{
	Value a(7.5f), b(2.5f);
	Value result = a / b;
	EXPECT_EQ(result, Value(3.0f));
}

TEST(ValueTests, Divide_IntAndFloat)
{
	Value a(10), b(4.0f);
	Value result = a / b;
	EXPECT_EQ(result, Value(2.5f));
}

TEST(ValueTests, Divide_FloatAndInt)
{
	Value a(10.0f), b(4);
	Value result = a / b;
	EXPECT_EQ(result, Value(2.5f));
}

TEST(ValueTests, Divide_IntAndString)
{
	Value a(3), b(std::wstring(L"abc"));
	EXPECT_THROW(a / b, std::runtime_error);
}

// Test multiplication assignment operator
TEST(ValueTests, MultiplyAssign_IntAndInt)
{
	Value a(5), b(3);
	a *= b;
	EXPECT_EQ(a, Value(15));
}

TEST(ValueTests, MultiplyAssign_FloatAndFloat)
{
	Value a(2.5f), b(4.0f);
	a *= b;
	EXPECT_EQ(a, Value(10.0f));
}

TEST(ValueTests, MultiplyAssign_IntAndFloat)
{
	Value a(6), b(2.5f);
	a *= b;
	EXPECT_EQ(a, Value(15.0f));
}

// Test division assignment operator
TEST(ValueTests, DivideAssign_IntAndInt)
{
	Value a(10), b(2);
	a /= b;
	EXPECT_EQ(a, Value(5));
}

TEST(ValueTests, DivideAssign_FloatAndFloat)
{
	Value a(7.5f), b(2.5f);
	a /= b;
	EXPECT_EQ(a, Value(3.0f));
}

// Test invalid operations
TEST(ValueTests, InvalidOperation)
{
	Value a(std::wstring(L"test")), b(std::wstring(L"abc"));
	EXPECT_THROW(a * b, std::runtime_error);
	EXPECT_THROW(a / b, std::runtime_error);
}

TEST(ValueTests, EqualityOperatorFloatFloat)
{
	Value val1(3.14f), val2(3.14f);
	EXPECT_TRUE(val1 == val2);

	val2 = Value(2.71f);
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorFloatInt)
{
	Value val1(3.14f), val2(3);
	EXPECT_FALSE(val1 == val2);

	val1 = Value(3.0f);
	EXPECT_TRUE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorIntString)
{
	Value val1(42), val2(std::wstring(L"42"));
	EXPECT_TRUE(val1 == val2);

	val2 = Value(std::wstring(L"10"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorStringInt)
{
	Value val1(std::wstring(L"42")), val2(42);
	EXPECT_TRUE(val1 == val2);

	val1 = Value(std::wstring(L"10"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorFloatString)
{
	Value val1(3.14f), val2(std::wstring(L"3.14"));
	EXPECT_TRUE(val1 == val2);

	val2 = Value(std::wstring(L"2.71"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorStringFloat)
{
	Value val1(std::wstring(L"3.14")), val2(3.14f);
	EXPECT_TRUE(val1 == val2);

	val1 = Value(std::wstring(L"2.71"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorBoolString)
{
	Value val1(true), val2(std::wstring(L"true"));
	EXPECT_TRUE(val1 == val2);

	val2 = Value(std::wstring(L"false"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorStringBool)
{
	Value val1(std::wstring(L"true")), val2(true);
	EXPECT_TRUE(val1 == val2);

	val1 = Value(std::wstring(L"false"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorStringString)
{
	Value val1(std::wstring(L"test")), val2(std::wstring(L"test"));
	EXPECT_TRUE(val1 == val2);

	val2 = Value(std::wstring(L"not test"));
	EXPECT_FALSE(val1 == val2);
}

TEST(ValueTests, EqualityOperatorBoolBool)
{
	Value val1(true), val2(true);
	EXPECT_TRUE(val1 == val2);

	val2 = Value(false);
	EXPECT_FALSE(val1 == val2);
}

// Test greater than operator
TEST(ValueTests, GreaterThanOperator)
{
	Value val1(42), val2(10);
	EXPECT_TRUE(val1 > val2);

	val2 = Value(42);
	EXPECT_FALSE(val1 > val2);
}

// Test greater than or equal operator
TEST(ValueTests, GreaterThanOrEqualOperator)
{
	Value val1(42), val2(42);
	EXPECT_TRUE(val1 >= val2);

	val2 = Value(10);
	EXPECT_TRUE(val1 >= val2);

	val2 = Value(50);
	EXPECT_FALSE(val1 >= val2);
}

// Test less than operator
TEST(ValueTests, LessThanOperator)
{
	Value val1(10), val2(42);
	EXPECT_TRUE(val1 < val2);

	val2 = Value(10);
	EXPECT_FALSE(val1 < val2);
}

// Test less than or equal operator
TEST(ValueTests, LessThanOrEqualOperator)
{
	Value val1(42), val2(42);
	EXPECT_TRUE(val1 <= val2);

	val1 = Value(10);
	EXPECT_TRUE(val1 <= val2);

	val1 = Value(50);
	EXPECT_FALSE(val1 <= val2);
}

// Test TryConvertToInt
TEST(ValueTests, TryConvertToInt_ValidInt)
{
	auto result = ValueTest::TryConvertToInt(L"42");
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result.value(), 42);
}

TEST(ValueTests, TryConvertToInt_InvalidInt)
{
	auto result = ValueTest::TryConvertToInt(L"invalid");
	EXPECT_FALSE(result.has_value());
}

TEST(ValueTests, TryConvertToInt_EmptyString)
{
	auto result = ValueTest::TryConvertToInt(L"");
	EXPECT_FALSE(result.has_value());
}

TEST(ValueTests, TryConvertToInt_FloatString)
{
	auto result = ValueTest::TryConvertToInt(L"3.14");
	EXPECT_FALSE(result.has_value());
}

// Test TryConvertToFloat
TEST(ValueTests, TryConvertToFloat_ValidFloat)
{
	auto result = ValueTest::TryConvertToFloat(L"3.14");
	ASSERT_TRUE(result.has_value());
	EXPECT_FLOAT_EQ(result.value(), 3.14f);
}

TEST(ValueTests, TryConvertToFloat_InvalidFloat)
{
	auto result = ValueTest::TryConvertToFloat(L"invalid");
	EXPECT_FALSE(result.has_value());
}

TEST(ValueTests, TryConvertToFloat_EmptyString)
{
	auto result = ValueTest::TryConvertToFloat(L"");
	EXPECT_FALSE(result.has_value());
}

TEST(ValueTests, TryConvertToFloat_IntString)
{
	auto result = ValueTest::TryConvertToFloat(L"42");
	ASSERT_TRUE(result.has_value());
	EXPECT_FLOAT_EQ(result.value(), 42.0f);
}

// Test CompareInt
TEST(ValueTests, CompareInt_Equal)
{
	EXPECT_TRUE(ValueTest::Compare(42, L"42"));
}

TEST(ValueTests, CompareInt_NotEqual)
{
	EXPECT_FALSE(ValueTest::Compare(42, L"43"));
}

TEST(ValueTests, CompareInt_InvalidString)
{
	EXPECT_THROW(ValueTest::Compare(42, L"invalid"), Value::ValueException);
}

// Test CompareFloat
TEST(ValueTests, CompareFloat_Equal)
{
	EXPECT_TRUE(ValueTest::Compare(3.14f, L"3.14"));
}

TEST(ValueTests, CompareFloat_NotEqual)
{
	EXPECT_FALSE(ValueTest::Compare(3.14f, L"3.15"));
}

TEST(ValueTests, CompareFloat_InvalidString)
{
	EXPECT_THROW(ValueTest::Compare(3.14f, L"invalid"), Value::ValueException);
}

// Test CompareBool
TEST(ValueTests, CompareBool_True)
{
	EXPECT_TRUE(ValueTest::Compare(true, L"true"));
}

TEST(ValueTests, CompareBool_False)
{
	EXPECT_FALSE(ValueTest::Compare(true, L"false"));
}

TEST(ValueTests, CompareBool_InvalidString)
{
	EXPECT_THROW(ValueTest::Compare(true, L"invalid"), Value::ValueException);
}

// Test MultiplyString
TEST(ValueTests, MultiplyString_Valid)
{
	EXPECT_EQ(ValueTest::MultiplyString(3, L"test"), L"testtesttest");
}

TEST(ValueTests, MultiplyString_Zero)
{
	EXPECT_EQ(ValueTest::MultiplyString(0, L"test"), L"");
}

TEST(ValueTests, ToString_Int)
{
	Value val(42);
	EXPECT_EQ(val.ToPrintString(), L"42");
}

TEST(ValueTests, ToString_Float)
{
	Value val(3.14f);
	EXPECT_EQ(val.ToPrintString(), L"3.140000");
}

TEST(ValueTests, ToString_BoolTrue)
{
	Value val(true);
	EXPECT_EQ(val.ToPrintString(), L"true");
}

TEST(ValueTests, ToString_BoolFalse)
{
	Value val(false);
	EXPECT_EQ(val.ToPrintString(), L"false");
}

TEST(ValueTests, ToString_WString)
{
	Value val(std::wstring(L"test"));
	EXPECT_EQ(val.ToPrintString(), L"test");
}

// Test ToBool
TEST(ValueTests, ToBool_BoolTrue)
{
	Value val(true);
	EXPECT_TRUE(val.ToBool());
}

TEST(ValueTests, ToBool_BoolFalse)
{
	Value val(false);
	EXPECT_FALSE(val.ToBool());
}

TEST(ValueTests, ToBool_WStringTrue)
{
	Value val(std::wstring(L"true"));
	EXPECT_TRUE(val.ToBool());
}

TEST(ValueTests, ToBool_WStringFalse)
{
	Value val(std::wstring(L"false"));
	EXPECT_FALSE(val.ToBool());
}

TEST(ValueTests, ToBool_InvalidWString)
{
	Value val(std::wstring(L"invalid"));
	EXPECT_THROW(val.ToBool(), Value::ValueException);
}

// Test GetFunction
TEST(ValueTests, GetFunction_ValidFunction)
{
	Block block;
	std::vector<Param> params = { Param(L"param1"), Param(L"param2") };
	Value::Function func(&block, params);
	Value val(func);
	const Value::Function* result = val.GetFunction();
	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->block, &block);
	EXPECT_EQ(result->parameters.size(), 2);
	EXPECT_EQ(result->parameters[0].identifier, L"param1");
	EXPECT_EQ(result->parameters[1].identifier, L"param2");
}

TEST(ValueTests, GetFunction_InvalidFunction)
{
	Value val(42);
	EXPECT_EQ(val.GetFunction(), nullptr);
}

TEST(ValueTests, OperatorRightShift_InvalidFunctionComposition)
{
	Value val1(42);
	Value val2(3.14f);

	EXPECT_THROW(val1 >> val2, Value::ValueException);
}

// Test operator<<
TEST(ValueTests, OperatorLeftShift_ValidFunctionArguments)
{
	Block block;
	std::vector<Param> params = { Param(L"param1"), Param(L"param2") };
	Value::Function func(&block, params);

	Value val(func);
	std::vector<Value> arguments = { Value(42), Value(3.14f) };

	Value result = val << arguments;
	const Value::Function* funcWithArgs = result.GetFunction();
	ASSERT_NE(funcWithArgs, nullptr);
	ASSERT_EQ(funcWithArgs->boundArguments.size(), 2);
	EXPECT_EQ(std::get<int>(funcWithArgs->boundArguments[0].value), 42);
	EXPECT_EQ(std::get<float>(funcWithArgs->boundArguments[1].value), 3.14f);
}

TEST(ValueTests, OperatorLeftShift_InvalidFunctionArguments)
{
	Value val(42);
	std::vector<Value> arguments = { Value(3.14f) };

	EXPECT_THROW(val << arguments, Value::ValueException);
}

TEST(ValueTests, OperatorRightShift_FunctionWithMultipleParameters)
{
	Block block1, block2;
	std::vector<Param> params1 = { Param(L"param1") };
	std::vector<Param> params2 = { Param(L"param2"), Param(L"param3") };
	Value::Function func1(&block1, params1);
	Value::Function func2(&block2, params2);

	Value val1(func1);
	Value val2(func2);

	EXPECT_THROW(val1 >> val2, Value::ValueException);
}

TEST(ValueTests, OperatorLeftShift_EmptyArguments)
{
	Block block;
	std::vector<Param> params = { Param(L"param1"), Param(L"param2") };
	Value::Function func(&block, params);

	Value val(func);
	std::vector<Value> arguments = {};

	Value result = val << arguments;
	const Value::Function* funcWithArgs = result.GetFunction();
	ASSERT_NE(funcWithArgs, nullptr);
	EXPECT_TRUE(funcWithArgs->boundArguments.empty());
}

TEST(ValueTests, OperatorLeftShift_MultipleArguments)
{
	Block block;
	std::vector<Param> params = { Param(L"param1"), Param(L"param2") };
	Value::Function func(&block, params);

	Value val(func);
	std::vector<Value> arguments = { Value(42), Value(3.14f), Value(true) };

	Value result = val << arguments;
	const Value::Function* funcWithArgs = result.GetFunction();
	ASSERT_NE(funcWithArgs, nullptr);
	ASSERT_EQ(funcWithArgs->boundArguments.size(), 3);
	EXPECT_EQ(std::get<int>(funcWithArgs->boundArguments[0].value), 42);
	EXPECT_EQ(std::get<float>(funcWithArgs->boundArguments[1].value), 3.14f);
	EXPECT_EQ(std::get<bool>(funcWithArgs->boundArguments[2].value), true);
}