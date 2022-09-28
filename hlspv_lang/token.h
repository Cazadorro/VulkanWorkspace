//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_TOKEN_H
#define VULKANWORKSPACE_TOKEN_H

#include "token_type.h"
#include "lexeme_view.h"
#include <string>
#include <string_view>
#include <variant>
#include <cstdint>

namespace hlspv {

    using TokenLiteralVariant = std::variant<std::monostate, std::string, std::uint64_t, std::int64_t, double, bool>;
    class Token {
    public:
        Token(TokenType type, LexemeView lexeme_view, TokenLiteralVariant literal, std::size_t line_number);
        [[nodiscard]]
        std::string lexeme() const;
        [[nodiscard]]
        hlspv::LexemeView lexeme_view() const;
        [[nodiscard]]
        TokenType type() const;
        template<typename T>
        [[nodiscard]]
        auto value() const{
            return std::get<T>(m_literal);
        }
        template<typename T>
        [[nodiscard]]
        bool contains_type() const{
            return std::holds_alternative<T>(m_literal);
        }
        [[nodiscard]]
        bool is_empty() const;
        [[nodiscard]]
        std::string get_value_string() const;
        [[nodiscard]]
        std::size_t line_number() const;
    private:
        TokenType m_type;
        LexemeView m_lexeme_view;
        TokenLiteralVariant m_literal;
        std::size_t m_line_number;
    };
}
#endif //VULKANWORKSPACE_TOKEN_H