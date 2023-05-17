//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_expr.h"

hlspv::ebnf::Expression::Expression(std::vector<RuleDefinitionExpr*> exprs) : exprs(std::move(exprs)) {

}

hlspv::ebnf::RuleDefinitionExpr::RuleDefinitionExpr(
        hlspv::EbnfToken non_terminal, hlspv::ebnf::RhsExpr* right) : non_terminal(std::move(non_terminal)), right(right) {

}

hlspv::ebnf::AlternationExpr::AlternationExpr(std::vector<RhsExpr*> exprs): exprs(std::move(exprs)) {

}

hlspv::ebnf::OptionalExpr::OptionalExpr(hlspv::ebnf::PreAndPostFixableExpr* expr): expr(expr) {

}

hlspv::ebnf::AtLeastOneExpr::AtLeastOneExpr(hlspv::ebnf::PreAndPostFixableExpr* expr): expr(expr) {

}

hlspv::ebnf::RepeatExpr::RepeatExpr(hlspv::ebnf::PreAndPostFixableExpr* expr): expr(expr) {

}

hlspv::ebnf::BracketExpr::BracketExpr(hlspv::ebnf::PreAndPostFixableExpr* expr, std::uint64_t min, std::uint64_t max) : expr(expr), min(min), max(max){

}

hlspv::ebnf::ConcatenationExpr::ConcatenationExpr(std::vector<RhsExpr*> exprs): exprs(std::move(exprs)) {

}

hlspv::ebnf::AndPredicateExpr::AndPredicateExpr(hlspv::ebnf::PreAndPostFixableExpr* expr) : expr(expr){

}

hlspv::ebnf::NotPredicateExpr::NotPredicateExpr(hlspv::ebnf::PreAndPostFixableExpr* expr) : expr(expr){

}

hlspv::ebnf::GroupingExpr::GroupingExpr(hlspv::ebnf::RhsExpr* expr): expr(expr) {

}

hlspv::ebnf::NonSymbolExpr::NonSymbolExpr(hlspv::EbnfToken token): token(token){

}

std::string hlspv::ebnf::to_string(hlspv::ebnf::ExprType type) {
    switch(type){
        case ExprType::Expression:
            return "Expression";
            break;
        case ExprType::RuleDefinitionExpr:
            return "RuleDefinitionExpr";
            break;
        case ExprType::RhsExpr:
            return "RhsExpr";
            break;
        case ExprType::AlternationExpr:
            return "AlternationExpr";
            break;
        case ExprType::ConcatenationExpr:
            return "ConcatenationExpr";
            break;
        case ExprType::PreAndPostFixableExpr:
            return "PostFixableExpr";
            break;
        case ExprType::OptionalExpr:
            return "OptionalExpr";
            break;
        case ExprType::AtLeastOneExpr:
            return "AtLeastOneExpr";
            break;
        case ExprType::RepeatExpr:
            return "RepeatExpr";
            break;
        case ExprType::BracketExpr:
            return "BracketExpr";
            break;
        case ExprType::AndPredicateExpr:
            return "AndPredicate";
            break;
        case ExprType::NotPredicateExpr:
            return "NotPredicate";
            break;
        case ExprType::GroupingExpr:
            return "GroupingExpr";
            break;
        case ExprType::NonSymbolExpr:
            return "NonSymbolExpr";
            break;
        default:
            return "Unknown";
    }

}
namespace hlspv::ebnf {
    template<>
    ExprType get_expression_type<Expression>(){
        return ExprType::Expression;
    }
    template<>
    ExprType get_expression_type<RuleDefinitionExpr>(){
        return ExprType::RuleDefinitionExpr;
    }
    template<>
    ExprType get_expression_type<RhsExpr>() {
        return ExprType::RhsExpr;
    }

    template<>
    ExprType get_expression_type<AlternationExpr>() {
        return ExprType::AlternationExpr;
    }

    template<>
    ExprType get_expression_type<ConcatenationExpr>() {
        return ExprType::ConcatenationExpr;
    }

    template<>
    ExprType get_expression_type<PreAndPostFixableExpr>() {
        return ExprType::PreAndPostFixableExpr;
    }

    template<>
    ExprType get_expression_type<OptionalExpr>() {
        return ExprType::OptionalExpr;
    }

    template<>
    ExprType get_expression_type<AtLeastOneExpr>() {
        return ExprType::AtLeastOneExpr;
    }

    template<>
    ExprType get_expression_type<RepeatExpr>() {
        return ExprType::RepeatExpr;
    }

    template<>
    ExprType get_expression_type<BracketExpr>() {
        return ExprType::BracketExpr;
    }

    template<>
    ExprType get_expression_type<AndPredicateExpr>() {
        return ExprType::AndPredicateExpr;
    }

    template<>
    ExprType get_expression_type<NotPredicateExpr>() {
        return ExprType::NotPredicateExpr;
    }

    template<>
    ExprType get_expression_type<GroupingExpr>() {
        return ExprType::GroupingExpr;
    }

    template<>
    ExprType get_expression_type<NonSymbolExpr>() {
        return ExprType::NonSymbolExpr;
    }
}

