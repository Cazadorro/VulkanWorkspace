//
// Created by Shae Bolt on 9/26/2022.
//

#include "ebnf_token.h"

hlspv::EbnfToken::EbnfToken(hlspv::EbnfTokenType type, LexemeView lexeme_view,
                            EbnfTokenLiteralVariant literal, std::size_t line_number)  : m_type(type),m_lexeme_view(lexeme_view),
                                                                             m_literal(std::move(literal)),
                                                                             m_line_number(line_number){

}

std::string hlspv::EbnfToken::lexeme() const {
    return m_lexeme_view.to_string();
}

hlspv::EbnfTokenType hlspv::EbnfToken::type() const {
    return m_type;
}

bool hlspv::EbnfToken::is_empty() const {
    return contains_type<std::monostate>();
}

std::string hlspv::EbnfToken::get_value_string() const {
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

std::size_t hlspv::EbnfToken::line_number() const {
    return m_line_number;
}

hlspv::LexemeView hlspv::EbnfToken::lexeme_view() const {
    return m_lexeme_view;
}