//
// Created by Shae Bolt on 9/19/2022.
//

#ifndef VULKANWORKSPACE_EXPRESSION_H
#define VULKANWORKSPACE_EXPRESSION_H
#include "token.h"
#include <variant>
namespace hlspv{
    struct Expression{
    };


    struct BinaryOperator;
    struct BinaryExpession;
    struct NumericLiteral;
    struct UnaryOperator;
    struct UnaryExpression;
    struct Grouping;
    using PrimaryExpression = std::variant<std::monostate, NumericLiteral, Grouping>;

    struct BinaryOperator : public Expression{
        Token operator_terminal;
    };

    struct BinaryExpession : public Expression{
        Expression left;
        BinaryOperator binary_operator;
        Expression right;
    };

    struct NumericLiteral : public Expression{
        Token numeric_terminal;
    };

    struct UnaryOperator : public Expression{
        Token operator_terminal;
    };
    //TODO use variant instead of abstract class stuff?

    //Todo use unique ptr here instead, to allow compressing and expanding of expressions;

    struct UnaryExpression : public Expression{
        UnaryOperator unary_operator;
        PrimaryExpression primary_expression;
    };

    struct Grouping : public Expression{
        Expression grouped_expression;
    };


}

#endif //VULKANWORKSPACE_EXPRESSION_H
