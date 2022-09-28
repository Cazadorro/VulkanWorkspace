//
// Created by Shae Bolt on 9/23/2022.
//

#ifndef VULKANWORKSPACE_ASTPRINTER_H
#define VULKANWORKSPACE_ASTPRINTER_H
#include <expression.h>
#include <string>

namespace hlspv{
//    struct PrimaryExpression;
//    struct BinaryOperator;
//    struct BinaryExpession;
//    struct NumericLiteral;
//    struct UnaryOperator;
//    struct UnaryExpression;
//    struct GroupingExpression;
    class AstPrinter {
    public:
        std::string print(const Expression& expr){
            return expr.accept(*this);
        }
        std::string visit(const Expression& expr){
            //need to use variant TODO~!!!
            return std::visit([=,this](auto&& field) -> std::string{
                return this->visit(*field);
            }, expr.expr);
        }
        std::string visit(const PrimaryExpression& expr){
            //need to use variant TODO~!!!
            return std::visit([=,this](auto& field) -> std::string{
                return this->visit(*field);
            }, expr.expr);
        }
        std::string visit(const BinaryExpression& expr){
            return parenthesize(expr.op.terminal.lexeme(), expr.left, expr.right);
        }
        std::string visit(const BinaryOperator& expr){
            return expr.terminal.lexeme();
        }
        std::string visit(const UnaryExpression& expr){
            return parenthesize(expr.op.terminal.lexeme(), expr.expr);
        }

        std::string visit(const UnaryOperator& expr){
            return expr.terminal.lexeme();
        }
        std::string visit(const GroupingExpression& expr){
           return parenthesize("Group", expr.expr);
        }
        std::string visit(const NumericLiteral& expr){
            return expr.terminal.get_value_string();
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
    };
}



#endif //VULKANWORKSPACE_ASTPRINTER_H
