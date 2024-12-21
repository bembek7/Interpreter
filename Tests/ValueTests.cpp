#include "gtest/gtest.h"
#include "Value.h"

// Test unary minus operator
TEST(ValueTest, UnaryMinusInt)
{
    Value val(42);
    EXPECT_EQ(-val, Value(-42));
}

TEST(ValueTest, UnaryMinusFloat)
{
    Value val(3.14f);
    EXPECT_EQ(-val, Value(-3.14f));
}

// Test logical AND operator
TEST(ValueTest, LogicalAndOperator)
{
    Value val1(true), val2(true);
    EXPECT_EQ(val1 && val2, Value(true));

    val2 = Value(false);
    EXPECT_EQ(val1 && val2, Value(false));
}

// Test logical AND assignment operator
TEST(ValueTest, LogicalAndAssignmentOperator)
{
    Value val1(true), val2(false);
    val1 &= val2;
    EXPECT_EQ(val1, Value(false));
}

// Test logical OR operator
TEST(ValueTest, LogicalOrOperator)
{
    Value val1(false), val2(true);
    EXPECT_EQ(val1 || val2, Value(true));

    val1 = Value(true);
    EXPECT_EQ(val1 || val2, Value(true));
}

// Test logical OR assignment operator
TEST(ValueTest, LogicalOrAssignmentOperator)
{
    Value val1(false), val2(true);
    val1 |= val2;
    EXPECT_EQ(val1, Value(true));
}

// Test addition operator
TEST(ValueTest, AdditionOperatorIntFloat)
{
    Value val1(42), val2(3.14f);
    EXPECT_EQ(val1 + val2, Value(45.14f));
}

TEST(ValueTest, AdditionOperatorWStringInt)
{
    Value val1(std::wstring(L"Test "));
    Value val2(42);
    EXPECT_EQ(val1 + val2, Value(std::wstring(L"Test 42")));
}

TEST(ValueTest, AdditionOperatorWStringFloat)
{
    Value val1(std::wstring(L"3.0")), val2(3.14f);
    auto val3 = val1 + val2;
    EXPECT_FLOAT_EQ(std::get<float>(val3.value), std::get<float>(Value(6.14f).value));
}

// Test addition assignment operator
TEST(ValueTest, AdditionAssignmentOperator)
{
    Value val1(42), val2(3.14f);
    val1 += val2;
    EXPECT_EQ(val1, Value(45.14f));
}

// Test subtraction operator
TEST(ValueTest, SubtractionOperator) {
    Value val1(42), val2(10);
    EXPECT_EQ(val1 - val2, Value(32));

    val1 = Value(42.0f);
    val2 = Value(10.0f);
    EXPECT_EQ(val1 - val2, Value(32.0f));
}

// Test subtraction assignment operator
TEST(ValueTest, SubtractionAssignmentOperator) {
    Value val1(42), val2(10);
    val1 -= val2;
    EXPECT_EQ(val1, Value(32));
}

// Test equality operator
TEST(ValueTest, EqualityOperatorIntInt)
{
    Value val1(42), val2(42);
    EXPECT_TRUE(val1 == val2);

    val2 = Value(10);
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorIntFloat)
{
    Value val1(42), val2(42.0f);
    EXPECT_TRUE(val1 == val2);

    val2 = Value(10.0f);
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorWStringInt)
{
    Value val1(std::wstring(L"42")), val2(42);
    EXPECT_TRUE(val1 == val2);

    val2 = Value(10);
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorWStringFloat)
{
    Value val1(std::wstring(L"3.14")), val2(3.14f);
    EXPECT_TRUE(val1 == val2);

    val2 = Value(10.0f);
    EXPECT_FALSE(val1 == val2);
}

// Test inequality operator
TEST(ValueTest, InequalityOperator)
{
    Value val1(42), val2(10);
    EXPECT_TRUE(val1 != val2);

    val2 = Value(42);
    EXPECT_FALSE(val1 != val2);
}

// Test multiplication operator
TEST(ValueTest, Multiply_IntAndInt)
{
    Value a(5), b(3);
    Value result = a * b;
    EXPECT_EQ(result, Value(15));
}

TEST(ValueTest, Multiply_FloatAndFloat)
{
    Value a(2.5f), b(4.0f);
    Value result = a * b;
    EXPECT_EQ(result, Value(10.0f));
}

TEST(ValueTest, Multiply_IntAndFloat)
{
    Value a(6), b(2.5f);
    Value result = a * b;
    EXPECT_EQ(result, Value(15.0f));
}

TEST(ValueTest, Multiply_FloatAndInt)
{
    Value a(2.5f), b(6);
    Value result = a * b;
    EXPECT_EQ(result, Value(15.0f));
}

TEST(ValueTest, Multiply_IntAndString)
{
    Value a(3), b(std::wstring(L"abc"));
    Value result = a * b;
    EXPECT_EQ(result, Value(std::wstring(L"abcabcabc")));
}

TEST(ValueTest, Multiply_StringAndInt)
{
    Value a(std::wstring(L"abc")), b(3);
    Value result = a * b;
    EXPECT_EQ(result, Value(std::wstring(L"abcabcabc")));
}

// Test division operator
TEST(ValueTest, Divide_IntAndInt)
{
    Value a(10), b(2);
    Value result = a / b;
    EXPECT_EQ(result, Value(5));
}

TEST(ValueTest, Divide_FloatAndFloat)
{
    Value a(7.5f), b(2.5f);
    Value result = a / b;
    EXPECT_EQ(result, Value(3.0f));
}

TEST(ValueTest, Divide_IntAndFloat)
{
    Value a(10), b(4.0f);
    Value result = a / b;
    EXPECT_EQ(result, Value(2.5f));
}

TEST(ValueTest, Divide_FloatAndInt)
{
    Value a(10.0f), b(4);
    Value result = a / b;
    EXPECT_EQ(result, Value(2.5f));
}

TEST(ValueTest, Divide_IntAndString)
{
    Value a(3), b(std::wstring(L"abc"));
    EXPECT_THROW(a / b, std::runtime_error);
}

// Test multiplication assignment operator
TEST(ValueTest, MultiplyAssign_IntAndInt)
{
    Value a(5), b(3);
    a *= b;
    EXPECT_EQ(a, Value(15));
}

TEST(ValueTest, MultiplyAssign_FloatAndFloat)
{
    Value a(2.5f), b(4.0f);
    a *= b;
    EXPECT_EQ(a, Value(10.0f));
}

TEST(ValueTest, MultiplyAssign_IntAndFloat)
{
    Value a(6), b(2.5f);
    a *= b;
    EXPECT_EQ(a, Value(15.0f));
}

// Test division assignment operator
TEST(ValueTest, DivideAssign_IntAndInt)
{
    Value a(10), b(2);
    a /= b;
    EXPECT_EQ(a, Value(5));
}

TEST(ValueTest, DivideAssign_FloatAndFloat)
{
    Value a(7.5f), b(2.5f);
    a /= b;
    EXPECT_EQ(a, Value(3.0f));
}

// Test invalid operations
TEST(ValueTest, InvalidOperation)
{
    Value a(std::wstring(L"test")), b(std::wstring(L"abc"));
    EXPECT_THROW(a * b, std::runtime_error);
    EXPECT_THROW(a / b, std::runtime_error);
}

TEST(ValueTest, EqualityOperatorFloatFloat)
{
    Value val1(3.14f), val2(3.14f);
    EXPECT_TRUE(val1 == val2);

    val2 = Value(2.71f);
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorFloatInt)
{
    Value val1(3.14f), val2(3);
    EXPECT_FALSE(val1 == val2);

    val1 = Value(3.0f);
    EXPECT_TRUE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorIntString)
{
    Value val1(42), val2(std::wstring(L"42"));
    EXPECT_TRUE(val1 == val2);

    val2 = Value(std::wstring(L"10"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorStringInt)
{
    Value val1(std::wstring(L"42")), val2(42);
    EXPECT_TRUE(val1 == val2);

    val1 = Value(std::wstring(L"10"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorFloatString)
{
    Value val1(3.14f), val2(std::wstring(L"3.14"));
    EXPECT_TRUE(val1 == val2);

    val2 = Value(std::wstring(L"2.71"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorStringFloat)
{
    Value val1(std::wstring(L"3.14")), val2(3.14f);
    EXPECT_TRUE(val1 == val2);

    val1 = Value(std::wstring(L"2.71"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorBoolString)
{
    Value val1(true), val2(std::wstring(L"true"));
    EXPECT_TRUE(val1 == val2);

    val2 = Value(std::wstring(L"false"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorStringBool)
{
    Value val1(std::wstring(L"true")), val2(true);
    EXPECT_TRUE(val1 == val2);

    val1 = Value(std::wstring(L"false"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorStringString)
{
    Value val1(std::wstring(L"test")), val2(std::wstring(L"test"));
    EXPECT_TRUE(val1 == val2);

    val2 = Value(std::wstring(L"not test"));
    EXPECT_FALSE(val1 == val2);
}

TEST(ValueTest, EqualityOperatorBoolBool)
{
    Value val1(true), val2(true);
    EXPECT_TRUE(val1 == val2);

    val2 = Value(false);
    EXPECT_FALSE(val1 == val2);
}

// Test greater than operator
TEST(ValueTest, GreaterThanOperator)
{
    Value val1(42), val2(10);
    EXPECT_TRUE(val1 > val2);

    val2 = Value(42);
    EXPECT_FALSE(val1 > val2);
}

// Test greater than or equal operator
TEST(ValueTest, GreaterThanOrEqualOperator)
{
    Value val1(42), val2(42);
    EXPECT_TRUE(val1 >= val2);

    val2 = Value(10);
    EXPECT_TRUE(val1 >= val2);

    val2 = Value(50);
    EXPECT_FALSE(val1 >= val2);
}

// Test less than operator
TEST(ValueTest, LessThanOperator)
{
    Value val1(10), val2(42);
    EXPECT_TRUE(val1 < val2);

    val2 = Value(10);
    EXPECT_FALSE(val1 < val2);
}

// Test less than or equal operator
TEST(ValueTest, LessThanOrEqualOperator)
{
    Value val1(42), val2(42);
    EXPECT_TRUE(val1 <= val2);

    val1 = Value(10);
    EXPECT_TRUE(val1 <= val2);

    val1 = Value(50);
    EXPECT_FALSE(val1 <= val2);
}

//// Test TryConvertToInt
//TEST(ValueTest, TryConvertToInt) {
//    auto result = Value::TryConvertToInt(L"42");
//    ASSERT_TRUE(result.has_value());
//    EXPECT_EQ(result.value(), 42);
//
//    result = Value::TryConvertToInt(L"invalid");
//    EXPECT_FALSE(result.has_value());
//}
//
//// Test TryConvertToFloat
//TEST(ValueTest, TryConvertToFloat) {
//    auto result = Value::TryConvertToFloat(L"3.14");
//    ASSERT_TRUE(result.has_value());
//    EXPECT_FLOAT_EQ(result.value(), 3.14f);
//
//    result = Value::TryConvertToFloat(L"invalid");
//    EXPECT_FALSE(result.has_value());
//}
//
//// Test CompareInt
//TEST(ValueTest, CompareInt) {
//    EXPECT_TRUE(Value::Compare(42, L"42"));
//    EXPECT_FALSE(Value::Compare(42, L"43"));
//}
//
//// Test CompareFloat
//TEST(ValueTest, CompareFloat) {
//    EXPECT_TRUE(Value::Compare(3.14f, L"3.14"));
//    EXPECT_FALSE(Value::Compare(3.14f, L"3.15"));
//}
//
//// Test CompareBool
//TEST(ValueTest, CompareBool) {
//    EXPECT_TRUE(Value::Compare(true, L"true"));
//    EXPECT_FALSE(Value::Compare(true, L"false"));
//}
//
//// Test MultiplyString
//TEST(ValueTest, MultiplyString) {
//    EXPECT_EQ(Value::MultiplyString(3, L"test"), L"testtesttest");
//}
