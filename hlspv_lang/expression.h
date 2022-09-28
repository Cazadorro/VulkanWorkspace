//
// Created by Shae Bolt on 9/19/2022.
//

#ifndef VULKANWORKSPACE_EXPRESSION_H
#define VULKANWORKSPACE_EXPRESSION_H

#include "token.h"
#include <variant>
#include <memory>

namespace hlspv {

    struct PrimaryExpression;
    struct BinaryOperator;
    struct BinaryExpression;
    struct NumericLiteral;
    struct UnaryOperator;
    struct UnaryExpression;
    struct GroupingExpression;

    struct Expression {
        using expr_variant = std::variant<
                std::unique_ptr<PrimaryExpression>,
                std::unique_ptr<BinaryOperator>,
                std::unique_ptr<BinaryExpression>,
                std::unique_ptr<NumericLiteral>,
                std::unique_ptr<UnaryOperator>,
                std::unique_ptr<UnaryExpression>,
                std::unique_ptr<GroupingExpression>>;
        expr_variant expr;

        template<typename T>
        Expression(T value) : expr(std::make_unique<T>(std::move(value))) {

        }

//        Expression(const Expression&) = default;
        Expression(Expression &&) = default;

//        Expression& operator=(const Expression&) = default;
        Expression &operator=(Expression &&) = default;

        ~Expression() = default;

//        template<typename Visitor>
//        auto accept(Visitor& visitor){
//            return std::visit([&visitor](auto& field){
//                return visitor.visit(field);
//            }, expr);
//        }
        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }
    };

    struct PrimaryExpression {
        using expr_variant = std::variant<
                std::unique_ptr<NumericLiteral>,
                std::unique_ptr<GroupingExpression>>;
        expr_variant expr;

        template<typename T>
        PrimaryExpression(T value) : expr(std::make_unique<T>(std::move(value))) {

        }

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        PrimaryExpression(PrimaryExpression &&) = default;

        PrimaryExpression &operator=(PrimaryExpression &&) = default;

        ~PrimaryExpression() = default;
    };

    struct BinaryOperator {
        Token terminal;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        BinaryOperator(Token terminal);
        BinaryOperator(BinaryOperator &&) = default;

        BinaryOperator &operator=(BinaryOperator &&) = default;

        ~BinaryOperator() = default;
    };

    struct BinaryExpression {
        Expression left;
        BinaryOperator op;
        Expression right;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        BinaryExpression(Expression left,
                         BinaryOperator op,
                         Expression right);

        BinaryExpression(BinaryExpression &&) = default;

        BinaryExpression &operator=(BinaryExpression &&) = default;

        ~BinaryExpression() = default;
    };

    struct NumericLiteral {
        Token terminal;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        NumericLiteral(Token terminal);

        NumericLiteral(NumericLiteral &&) = default;

        NumericLiteral &operator=(NumericLiteral &&) = default;

        ~NumericLiteral() = default;
    };

    struct UnaryOperator {
        Token terminal;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        UnaryOperator(Token terminal);

        UnaryOperator(UnaryOperator &&) = default;

        UnaryOperator &operator=(UnaryOperator &&) = default;

        ~UnaryOperator() = default;
    };

    struct UnaryExpression {
        UnaryOperator op;
        PrimaryExpression expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        UnaryExpression(UnaryOperator op, PrimaryExpression expr);

        UnaryExpression(UnaryExpression &&) = default;

        UnaryExpression &operator=(UnaryExpression &&) = default;

        ~UnaryExpression() = default;
    };

    struct GroupingExpression {
        Expression expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        GroupingExpression(Expression expr);

        GroupingExpression(GroupingExpression &&) = default;

        GroupingExpression &operator=(GroupingExpression &&) = default;

        ~GroupingExpression() = default;
    };


}

#endif //VULKANWORKSPACE_EXPRESSION_H
