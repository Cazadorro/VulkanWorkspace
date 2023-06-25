//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_TOKEN_H
#define VULKANWORKSPACE_TOKEN_H

#include "lang_token_type.h"
#include "lexeme_view.h"
#include <uul/concepts.h>
#include <string>
#include <string_view>
#include <variant>
#include <cstdint>
#include <type_traits>

namespace hlspv {

    template<typename TokenType_T, uul::Variant TokenLiteralVariant_T>
    class Token {
    public:
        using token_type = TokenType_T;

        Token(TokenType_T type, LexemeView lexeme_view, TokenLiteralVariant_T literal, std::size_t line_number): m_type(type), m_lexeme_view(lexeme_view),
                                                                                                                 m_literal(std::move(literal)),
                                                                                                                 m_line_number(line_number) {

        };
        [[nodiscard]]
        std::string lexeme() const {
            return m_lexeme_view.to_string();
        }
        [[nodiscard]]
        hlspv::LexemeView lexeme_view() const{
            return m_lexeme_view;
        }
        [[nodiscard]]
        TokenType_T type() const{
            return m_type;
        }
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
        bool is_empty() const{
            return contains_type<std::monostate>();
        }
        [[nodiscard]]
        std::string get_value_string() const{
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
        [[nodiscard]]
        std::size_t line_number() const{
            return m_line_number;
        }
    private:
        TokenType_T m_type;
        LexemeView m_lexeme_view;
        TokenLiteralVariant_T m_literal;
        std::size_t m_line_number;
    };
}
#endif //VULKANWORKSPACE_TOKEN_H
