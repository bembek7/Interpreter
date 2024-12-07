#include "ParserObjects.h"

bool operator==(const Statement& lhs, const Statement& rhs)
{
	return typeid(lhs) == typeid(rhs) && lhs.isEqual(rhs);
}

bool operator==(const Additive& lhs, const Additive& rhs)
{
	return lhs.negated == rhs.negated && lhs.operators == rhs.operators && CompareVectorUniqs(lhs.multiplicatives, rhs.multiplicatives);
}

bool operator==(const Relation& lhs, const Relation& rhs)
{
	return *lhs.firstAdditive == *rhs.firstAdditive && lhs.relationOperator == rhs.relationOperator &&
		CompareNullableUniqs(lhs.secondAdditive, rhs.secondAdditive);
}

bool operator==(const Conjunction& lhs, const Conjunction& rhs)
{
	return CompareVectorUniqs(lhs.relations, rhs.relations);
}

bool operator==(const Expression& lhs, const Expression& rhs)
{
	return CompareVectorUniqs(lhs.conjunctions, rhs.conjunctions);
}

bool operator==(const Literal& lhs, const Literal& rhs)
{
	return lhs.value == rhs.value;
}

bool operator==(const Factor& lhs, const Factor& rhs)
{
    if (lhs.logicallyNegated != rhs.logicallyNegated)
        return false;

    return std::visit([](const auto& left, const auto& right) -> bool 
        {
            using LeftType = std::decay_t<decltype(left)>;
            using RightType = std::decay_t<decltype(right)>;

            if constexpr (std::is_same_v<LeftType, RightType>)
            {
                if constexpr (std::is_same_v<LeftType, std::unique_ptr<Literal>> ||
                    std::is_same_v<LeftType, std::unique_ptr<Expression>> ||
                    std::is_same_v<LeftType, std::unique_ptr<FunctionCall>>)
                {
                    return left && right && *left == *right;
                }
                else
                {
                    return left == right;
                }
            }
            else
            {
                return false;
            }
        }, lhs.factor, rhs.factor);
}

bool operator==(const Multiplicative& lhs, const Multiplicative& rhs)
{
	return CompareVectorUniqs(lhs.factors, rhs.factors) && lhs.operators == rhs.operators;
}
