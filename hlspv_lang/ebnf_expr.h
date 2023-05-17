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
    struct RuleDefinitionExpr;      // A = a | (b | B,c)...
    struct RhsExpr;                 // a | (b | B,c)...
    struct AlternationExpr;         // a|b
    struct ConcatenationExpr;       // a,b
    struct PreAndPostFixableExpr;   // anything that you can put +/?/*/[] and prefixes.
    struct OptionalExpr;            // a?
    struct AtLeastOneExpr;          // a+
    struct RepeatExpr;              // a*
    struct BracketExpr;             // a[x:y]
    struct AndPredicateExpr;            // &a
    struct NotPredicateExpr;            // !a
    struct GroupingExpr;            // (..)
    struct NonSymbolExpr;           // terminals and expression



    enum class ExprType {
        Expression,
        RuleDefinitionExpr,
        RhsExpr,
        AlternationExpr,
        ConcatenationExpr,
        PreAndPostFixableExpr,
        OptionalExpr,
        AtLeastOneExpr,
        RepeatExpr,
        BracketExpr,
        AndPredicateExpr,
        NotPredicateExpr,
        GroupingExpr,
        NonSymbolExpr
    };

    [[nodiscard]]
    std::string to_string(ExprType type);


    struct Expression {
        static constexpr auto type = ExprType::Expression;
        std::vector<RuleDefinitionExpr*> exprs;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        Expression(std::vector<RuleDefinitionExpr*> exprs);

        Expression(Expression &&) = default;

        Expression &operator=(Expression &&) = default;

        ~Expression() = default;
    };

    //TODO handle recursive grammar rule(fully recursive?, should just need to parse in order?)
    struct RhsExpr {
        static constexpr auto type = ExprType::RhsExpr;
        //technically Rhs expr is also allowed, but by definition, that would be any of these other expressions, which creates the indirection for us.
        using expr_variant = std::variant<
                AlternationExpr*,
                ConcatenationExpr*,
                OptionalExpr*,
                AtLeastOneExpr*,
                RepeatExpr*,
                BracketExpr*,
                AndPredicateExpr*,
                NotPredicateExpr*,
                GroupingExpr*,
                NonSymbolExpr*
        >;
        expr_variant expr;

        template<typename T>
        explicit RhsExpr(T value) : expr(value) {}

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
        RhsExpr* right;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        RuleDefinitionExpr(EbnfToken non_terminal, RhsExpr* right);

        RuleDefinitionExpr(RuleDefinitionExpr &&) = default;

        RuleDefinitionExpr &operator=(RuleDefinitionExpr &&) = default;

        ~RuleDefinitionExpr() = default;
    };

    struct AlternationExpr {
        static constexpr auto type = ExprType::AlternationExpr;
        std::vector<RhsExpr*> exprs;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        AlternationExpr(std::vector<RhsExpr*> exprs);

        AlternationExpr(AlternationExpr &&) = default;

        AlternationExpr &operator=(AlternationExpr &&) = default;

        ~AlternationExpr() = default;
    };

    struct ConcatenationExpr {
        static constexpr auto type = ExprType::ConcatenationExpr;
        std::vector<RhsExpr*> exprs;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        ConcatenationExpr(std::vector<RhsExpr*> expr);

        ConcatenationExpr(ConcatenationExpr &&) = default;

        ConcatenationExpr &operator=(ConcatenationExpr &&) = default;

        ~ConcatenationExpr() = default;
    };

    struct PreAndPostFixableExpr {
        static constexpr auto type = ExprType::PreAndPostFixableExpr;
        //technically Rhs expr is also allowed, but by definition, that would be any of these other expressions, which creates the indirection for us.
        using expr_variant = std::variant<
                GroupingExpr*,
                NonSymbolExpr*
        >;
        expr_variant expr;

        template<typename T>
        explicit PreAndPostFixableExpr(T* value) : expr(value) {

        }

        PreAndPostFixableExpr(PreAndPostFixableExpr &&) = default;

        PreAndPostFixableExpr &operator=(PreAndPostFixableExpr &&) = default;

        ~PreAndPostFixableExpr() = default;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }
    };


    struct OptionalExpr {
        static constexpr auto type = ExprType::OptionalExpr;
        PreAndPostFixableExpr* expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        OptionalExpr(PreAndPostFixableExpr* expr);

        OptionalExpr(OptionalExpr &&) = default;

        OptionalExpr &operator=(OptionalExpr &&) = default;

        ~OptionalExpr() = default;
    };

    struct AtLeastOneExpr {
        static constexpr auto type = ExprType::AtLeastOneExpr;
        PreAndPostFixableExpr* expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        AtLeastOneExpr(PreAndPostFixableExpr* expr);

        AtLeastOneExpr(AtLeastOneExpr &&) = default;

        AtLeastOneExpr &operator=(AtLeastOneExpr &&) = default;

        ~AtLeastOneExpr() = default;
    };

    struct RepeatExpr {
        static constexpr auto type = ExprType::RepeatExpr;
        PreAndPostFixableExpr* expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        RepeatExpr(PreAndPostFixableExpr* expr);

        RepeatExpr(RepeatExpr &&) = default;

        RepeatExpr &operator=(RepeatExpr &&) = default;

        ~RepeatExpr() = default;
    };

    struct BracketExpr {
        static constexpr auto type = ExprType::BracketExpr;
        PreAndPostFixableExpr* expr;
        std::uint64_t min;
        std::uint64_t max;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        BracketExpr(PreAndPostFixableExpr* expr, std::uint64_t min, std::uint64_t max);

        BracketExpr(BracketExpr &&) = default;

        BracketExpr &operator=(BracketExpr &&) = default;

        ~BracketExpr() = default;
    };

    struct AndPredicateExpr {
        static constexpr auto type = ExprType::AndPredicateExpr;
        PreAndPostFixableExpr* expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        AndPredicateExpr(PreAndPostFixableExpr* expr);

        AndPredicateExpr(AndPredicateExpr &&) = default;

        AndPredicateExpr &operator=(AndPredicateExpr &&) = default;

        ~AndPredicateExpr() = default;
    };

    struct NotPredicateExpr {
        static constexpr auto type = ExprType::NotPredicateExpr;
        PreAndPostFixableExpr* expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        NotPredicateExpr(PreAndPostFixableExpr* expr);

        NotPredicateExpr(NotPredicateExpr &&) = default;

        NotPredicateExpr &operator=(NotPredicateExpr &&) = default;

        ~NotPredicateExpr() = default;
    };


    struct GroupingExpr {
        static constexpr auto type = ExprType::GroupingExpr;
        RhsExpr* expr;

        template<typename Visitor>
        auto accept(Visitor &visitor) {
            return visitor.visit(*this);
        }

        template<typename Visitor>
        auto accept(Visitor &visitor) const {
            return visitor.visit(*this);
        }

        GroupingExpr(RhsExpr* expr);

        GroupingExpr(GroupingExpr &&) = default;

        GroupingExpr &operator=(GroupingExpr &&) = default;

        ~GroupingExpr() = default;
    };

    struct NonSymbolExpr {
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

    using AnyExpression = std::variant<
            Expression,
            RuleDefinitionExpr,
            RhsExpr,
            AlternationExpr,
            ConcatenationExpr,
            PreAndPostFixableExpr,
            OptionalExpr,
            AtLeastOneExpr,
            RepeatExpr,
            BracketExpr,
            AndPredicateExpr,
            NotPredicateExpr,
            GroupingExpr,
            NonSymbolExpr
    >;

    using AnyExpressionPtr = std::variant<
            Expression*,
            RuleDefinitionExpr*,
            RhsExpr*,
            AlternationExpr*,
            ConcatenationExpr*,
            PreAndPostFixableExpr*,
            OptionalExpr*,
            AtLeastOneExpr*,
            RepeatExpr*,
            BracketExpr*,
            AndPredicateExpr*,
            NotPredicateExpr*,
            GroupingExpr*,
            NonSymbolExpr*
    >;

    using AnyExpressionUniquePtr = std::variant<
            std::unique_ptr<Expression>,
            std::unique_ptr<RuleDefinitionExpr>,
            std::unique_ptr<RhsExpr>,
            std::unique_ptr<AlternationExpr>,
            std::unique_ptr<ConcatenationExpr>,
            std::unique_ptr<PreAndPostFixableExpr>,
            std::unique_ptr<OptionalExpr>,
            std::unique_ptr<AtLeastOneExpr>,
            std::unique_ptr<RepeatExpr>,
            std::unique_ptr<BracketExpr>,
            std::unique_ptr<AndPredicateExpr>,
            std::unique_ptr<NotPredicateExpr>,
            std::unique_ptr<GroupingExpr>,
            std::unique_ptr<NonSymbolExpr>
    >;



    template<typename T>
    ExprType get_expression_type() = delete;
    template<>
    ExprType get_expression_type<Expression>();
    template<>
    ExprType get_expression_type<RuleDefinitionExpr>();
    template<>
    ExprType get_expression_type<RhsExpr>();
    template<>
    ExprType get_expression_type<AlternationExpr>();
    template<>
    ExprType get_expression_type<ConcatenationExpr>();
    template<>
    ExprType get_expression_type<PreAndPostFixableExpr>();
    template<>
    ExprType get_expression_type<OptionalExpr>();
    template<>
    ExprType get_expression_type<AtLeastOneExpr>();
    template<>
    ExprType get_expression_type<RepeatExpr>();
    template<>
    ExprType get_expression_type<BracketExpr>();
    template<>
    ExprType get_expression_type<AndPredicateExpr>();
    template<>
    ExprType get_expression_type<NotPredicateExpr>();
    template<>
    ExprType get_expression_type<GroupingExpr>();
    template<>
    ExprType get_expression_type<NonSymbolExpr>();



}
#endif //VULKANWORKSPACE_EBNF_EXPR_H
