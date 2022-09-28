//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_expr.h"

hlspv::ebnf::Expression::Expression(std::vector<RuleDefinitionExpr> exprs) : exprs(std::move(exprs)) {

}

hlspv::ebnf::RuleDefinitionExpr::RuleDefinitionExpr(
        hlspv::EbnfToken non_terminal, hlspv::ebnf::RhsExpr right) : non_terminal(std::move(non_terminal)), right(std::move(right)) {

}

hlspv::ebnf::AlternationExpr::AlternationExpr(std::vector<RhsExpr> exprs): exprs(std::move(exprs)) {

}

hlspv::ebnf::OptionalExpr::OptionalExpr(hlspv::ebnf::PostFixableExpr expr): expr(std::move(expr)) {

}

hlspv::ebnf::AtLeastOneExpr::AtLeastOneExpr(hlspv::ebnf::PostFixableExpr expr): expr(std::move(expr)) {

}

hlspv::ebnf::RepeatExpr::RepeatExpr(hlspv::ebnf::PostFixableExpr expr): expr(std::move(expr)) {

}

hlspv::ebnf::ConcatenationExpr::ConcatenationExpr(std::vector<RhsExpr> exprs): exprs(std::move(exprs)) {

}

hlspv::ebnf::GroupingExpr::GroupingExpr(hlspv::ebnf::RhsExpr expr): expr(std::move(expr)) {

}

hlspv::ebnf::NonSymbolExpr::NonSymbolExpr(hlspv::EbnfToken token): token(std::move(token)){

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
        case ExprType::PostFixableExpr:
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

        case ExprType::GroupingExpr:
            return "GroupingExpr";
            break;
        case ExprType::NonSymbolExpr:
            return "NonSymbolExpr";
            break;
    }
}
