//
// Created by Shae Bolt on 9/27/2022.
//

#ifndef VULKANWORKSPACE_EBNF_AST_PRINTER_H
#define VULKANWORKSPACE_EBNF_AST_PRINTER_H
#include "ebnf_expr.h"
#include "fmt/core.h"
#include <string>
#include <variant>
namespace hlspv::ebnf{
    class AstPrinter {
    public:
        std::string print(const Expression& expr){
            return expr.accept(*this);
        }
        std::string visit(const Expression& expr){
            return parenthesize_list("Expression", expr.exprs);
        }
        std::string visit(const RuleDefinitionExpr& expr){
            return parenthesize(fmt::format("RuleDefinitionExpr: {} =", expr.non_terminal.lexeme()), expr.right);
        }
        std::string visit(const RhsExpr& expr){
            //need to use variant TODO~!!!
            return std::visit([=,this](auto& field) -> std::string{
                return this->visit(*field);
            }, expr.expr);
        }
        std::string visit(const AlternationExpr& expr){
            return parenthesize_list("AlternationExpr", expr.exprs);
        }
        std::string visit(const ConcatenationExpr& expr){
            return parenthesize_list("AlternationExpr", expr.exprs);
        }
        std::string visit(const PostFixableExpr& expr){
            return std::visit([=,this](auto& field) -> std::string{
                return this->visit(*field);
            }, expr.expr);
        }

        std::string visit(const OptionalExpr& expr){
            return parenthesize("Optional", expr) + "?";
        }
        std::string visit(const AtLeastOneExpr& expr){
            return parenthesize("Optional", expr) + "+";
        }
        std::string visit(const RepeatExpr& expr){
            return parenthesize("Optional", expr) + "*";
        }
        std::string visit(const GroupingExpr& expr){
            return parenthesize("GroupingExpr", expr.expr);
        }
        std::string visit(const NonSymbolExpr& expr){
            return expr.token.lexeme();
        }

    private:
        template<typename ...Args>
        std::string parenthesize(std::string name, Args&&... exprs) {
            std::string builder;
            builder.append("(").append(name);
            builder.append(" ");
            ([&]{
                builder.append(exprs.accept(*this));
            }(), ...);
            builder.append(")");
            return builder;
        }

        template<typename T>
        std::string parenthesize_list(std::string name, const std::vector<T>& exprs) {
            std::string builder;
            builder.append("(").append(name);
            builder.append(" ");
            for(const auto& expr : exprs){
                builder.append(expr.accept(*this));
            }
            builder.append(")");
            return builder;
        }
    };
}
#endif //VULKANWORKSPACE_EBNF_AST_PRINTER_H
