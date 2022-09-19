//
// Created by Shae Bolt on 9/18/2022.
//

#include "token.h"

hlspv::Token::Token(hlspv::TokenType type, std::string_view lexeme,
                    LiteralVariant literal,
                    std::size_t line_number) : m_type(type), m_lexeme(lexeme),
                                               m_literal(std::move(literal)),
                                               m_line_number(line_number) {

}

std::string hlspv::Token::lexeme() const {
    return m_lexeme;
}

hlspv::TokenType hlspv::Token::type() const {
    return m_type;
}
