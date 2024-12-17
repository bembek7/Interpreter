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

//TEST(ValueTest, AdditionOperatorWStringFloat)
//{
//	Value val1(std::wstring(L"Test ")), val2(3.14f);
//	EXPECT_EQ(val1 + val2, Value(std::wstring(L"Test 3.14")));
//}

// Test addition assignment operator
TEST(ValueTest, AdditionAssignmentOperator)
{
	Value val1(42), val2(3.14f);
	val1 += val2;
	EXPECT_EQ(val1, Value(45.14f));
}
//
//// Test subtraction operator
//TEST(ValueTest, SubtractionOperator) {
//    Value val1(42), val2(10);
//    EXPECT_EQ(val1 - val2, Value(32));
//
//    val1 = Value(42.0f);
//    val2 = Value(10.0f);
//    EXPECT_EQ(val1 - val2, Value(32.0f));
//}
//
//// Test subtraction assignment operator
//TEST(ValueTest, SubtractionAssignmentOperator) {
//    Value val1(42), val2(10);
//    val1 -= val2;
//    EXPECT_EQ(val1, Value(32));
//}

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