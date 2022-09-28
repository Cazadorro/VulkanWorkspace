//
// Created by Shae Bolt on 9/27/2022.
//

#ifndef VULKANWORKSPACE_EBNF_EXPR_H
#define VULKANWORKSPACE_EBNF_EXPR_H

#include "ebnf_token.h"
#include <variant>
#include <memory>
#include <vector>
#include <string>

namespace hlspv::ebnf {
    struct RuleDefinitionExpr;
    struct RhsExpr;
    struct AlternationExpr;
    struct ConcatenationExpr;
    struct PostFixableExpr;
    struct OptionalExpr;
    struct AtLeastOneExpr;
    struct RepeatExpr;
    struct GroupingExpr;
    struct NonSymbolExpr;

    enum class ExprType{
        Expression,
        RuleDefinitionExpr,
        RhsExpr,
        AlternationExpr,
        ConcatenationExpr,
        PostFixableExpr,
        OptionalExpr,
        AtLeastOneExpr,
        RepeatExpr,
        GroupingExpr,
        NonSymbolExpr
    };

    [[nodiscard]]
    std::string to_string(ExprType type);



    struct Expression {
        static constexpr auto type = ExprType::Expression;
        std::vector<RuleDefinitionExpr> exprs;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }
        Expression(std::vector<RuleDefinitionExpr> exprs);

        Expression(Expression &&) = default;

        Expression &operator=(Expression &&) = default;

        ~Expression() = default;
    };

    //TODO handle recursive grammar rule(fully recursive?, should just need to parse in order?)
    struct RhsExpr{
        static constexpr auto type = ExprType::RhsExpr;
        //technically Rhs expr is also allowed, but by definition, that would be any of these other expressions, which creates the indirection for us.
        using expr_variant = std::variant<
                std::unique_ptr<AlternationExpr>,
                std::unique_ptr<ConcatenationExpr>,
                std::unique_ptr<OptionalExpr>,
                std::unique_ptr<AtLeastOneExpr>,
                std::unique_ptr<RepeatExpr>,
                std::unique_ptr<GroupingExpr>,
                std::unique_ptr<NonSymbolExpr>
        >;
        expr_variant expr;

        template<typename T>
        explicit RhsExpr(T value) : expr(std::make_unique<T>(std::move(value))) {

        }

        RhsExpr(RhsExpr &&) = default;

        RhsExpr &operator=(RhsExpr &&) = default;

        ~RhsExpr() = default;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }
    };

    struct RuleDefinitionExpr {
        static constexpr auto type = ExprType::RuleDefinitionExpr;
        EbnfToken non_terminal;
        RhsExpr right;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        RuleDefinitionExpr(EbnfToken non_terminal, RhsExpr right);

        RuleDefinitionExpr(RuleDefinitionExpr &&) = default;

        RuleDefinitionExpr &operator=(RuleDefinitionExpr &&) = default;

        ~RuleDefinitionExpr() = default;
    };

    struct AlternationExpr{
        static constexpr auto type = ExprType::AlternationExpr;
        std::vector<RhsExpr> exprs;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        AlternationExpr(std::vector<RhsExpr> exprs);

        AlternationExpr(AlternationExpr &&) = default;

        AlternationExpr &operator=(AlternationExpr &&) = default;

        ~AlternationExpr() = default;
    };

    struct ConcatenationExpr{
        static constexpr auto type = ExprType::ConcatenationExpr;
        std::vector<RhsExpr> exprs;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        ConcatenationExpr(std::vector<RhsExpr> expr);

        ConcatenationExpr(ConcatenationExpr &&) = default;

        ConcatenationExpr &operator=(ConcatenationExpr &&) = default;

        ~ConcatenationExpr() = default;
    };

    struct PostFixableExpr{
        static constexpr auto type = ExprType::PostFixableExpr;
        //technically Rhs expr is also allowed, but by definition, that would be any of these other expressions, which creates the indirection for us.
        using expr_variant = std::variant<
                std::unique_ptr<GroupingExpr>,
                std::unique_ptr<NonSymbolExpr>
        >;
        expr_variant expr;

        template<typename T>
        explicit PostFixableExpr(T value) : expr(std::make_unique<T>(std::move(value))) {

        }

        PostFixableExpr(PostFixableExpr &&) = default;

        PostFixableExpr &operator=(PostFixableExpr &&) = default;

        ~PostFixableExpr() = default;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }
    };


    struct OptionalExpr{
        static constexpr auto type = ExprType::OptionalExpr;
        PostFixableExpr expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        OptionalExpr(PostFixableExpr expr);

        OptionalExpr(OptionalExpr &&) = default;

        OptionalExpr &operator=(OptionalExpr &&) = default;

        ~OptionalExpr() = default;
    };

    struct AtLeastOneExpr{
        static constexpr auto type = ExprType::AtLeastOneExpr;
        PostFixableExpr expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        AtLeastOneExpr(PostFixableExpr expr);

        AtLeastOneExpr(AtLeastOneExpr &&) = default;

        AtLeastOneExpr &operator=(AtLeastOneExpr &&) = default;

        ~AtLeastOneExpr() = default;
    };

    struct RepeatExpr{
        static constexpr auto type = ExprType::RepeatExpr;
        PostFixableExpr expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        RepeatExpr(PostFixableExpr expr);

        RepeatExpr(RepeatExpr &&) = default;

        RepeatExpr &operator=(RepeatExpr &&) = default;

        ~RepeatExpr() = default;
    };


    struct GroupingExpr{
        static constexpr auto type = ExprType::GroupingExpr;
        RhsExpr expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        GroupingExpr(RhsExpr expr);

        GroupingExpr(GroupingExpr &&) = default;

        GroupingExpr &operator=(GroupingExpr &&) = default;

        ~GroupingExpr() = default;
    };

    struct NonSymbolExpr{
        static constexpr auto type = ExprType::NonSymbolExpr;
        EbnfToken token;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        NonSymbolExpr(EbnfToken token);

        NonSymbolExpr(NonSymbolExpr &&) = default;

        NonSymbolExpr &operator=(NonSymbolExpr &&) = default;

        ~NonSymbolExpr() = default;
    };
}
#endif //VULKANWORKSPACE_EBNF_EXPR_H
