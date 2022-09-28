//
// Created by Shae Bolt on 9/19/2022.
//

#include "expression.h"


hlspv::BinaryOperator::BinaryOperator(hlspv::Token terminal) : terminal(std::move(terminal)) {

}

hlspv::BinaryExpression::BinaryExpression(hlspv::Expression left,
                                          hlspv::BinaryOperator op,
                                          hlspv::Expression right) :left(std::move(left)), op(std::move(op)), right(std::move(right)) {

}

hlspv::NumericLiteral::NumericLiteral(hlspv::Token terminal) : terminal(std::move(terminal)){

}

hlspv::UnaryOperator::UnaryOperator(hlspv::Token terminal): terminal(std::move(terminal)) {

}

hlspv::UnaryExpression::UnaryExpression(hlspv::UnaryOperator op,
                                        hlspv::PrimaryExpression expr): op(std::move(op)), expr(std::move(expr)){

}

hlspv::GroupingExpression::GroupingExpression(hlspv::Expression expr):expr(std::move(expr)) {

}
