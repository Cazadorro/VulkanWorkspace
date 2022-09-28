//
// Created by Shae Bolt on 9/18/2022.
//

#include "token.h"
#include <type_traits>


hlspv::Token::Token(hlspv::TokenType type, LexemeView lexeme_view,
                    TokenLiteralVariant literal,
                    std::size_t line_number) : m_type(type), m_lexeme_view(lexeme_view),
                                               m_literal(std::move(literal)),
                                               m_line_number(line_number) {

}

std::string hlspv::Token::lexeme() const {
    return m_lexeme_view.to_string();
}

hlspv::TokenType hlspv::Token::type() const {
    return m_type;
}

bool hlspv::Token::is_empty() const {
    return contains_type<std::monostate>();
}
//        if constexpr (std::is_same_v<decltype(value), std::monostate>){
//
//        }else{

std::string hlspv::Token::get_value_string() const {
    return std::visit(
                      [](auto value) -> std::string {
                             if constexpr (std::is_same_v<decltype(value), std::monostate>){
                                  return "monostate";
                            }
                             if constexpr (std::is_same_v<decltype(value), std::string>){
                                 return value;
                             }
                             else if constexpr(!std::is_same_v<decltype(value), std::monostate>){
                                 return std::to_string(value);
                             }
                      }
                      , m_literal);
}

std::size_t hlspv::Token::line_number() const {
    return m_line_number;
}

hlspv::LexemeView hlspv::Token::lexeme_view() const {
    return m_lexeme_view;
}
