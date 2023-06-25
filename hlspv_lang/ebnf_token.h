//
// Created by Shae Bolt on 9/26/2022.
//

#ifndef VULKANWORKSPACE_EBNF_TOKEN_H
#define VULKANWORKSPACE_EBNF_TOKEN_H
#include "ebnf_token_type.h"
#include "token.h"
#include <variant>
#include <string>
#include <cstdint>

namespace hlspv{
    //technically we could replace with std::optional, but may need numbers in future.
    using EbnfTokenLiteralVariant = std::variant<std::monostate, std::string, std::uint64_t>;

    using EbnfToken = Token<EbnfTokenType, EbnfTokenLiteralVariant>;
//    class EbnfToken {
//    public:
//        EbnfToken(EbnfTokenType type, LexemeView lexeme_view, EbnfTokenLiteralVariant literal, std::size_t line_number);
//        [[nodiscard]]
//        std::string lexeme() const;
//        [[nodiscard]]
//        hlspv::LexemeView lexeme_view() const;
//        [[nodiscard]]
//        EbnfTokenType type() const;
//        template<typename T>
//        [[nodiscard]]
//        auto value() const{
//            return std::get<T>(m_literal);
//        }
//        template<typename T>
//        [[nodiscard]]
//        bool contains_type() const{
//            return std::holds_alternative<T>(m_literal);
//        }
//        [[nodiscard]]
//        bool is_empty() const;
//        [[nodiscard]]
//        std::string get_value_string() const;
//        [[nodiscard]]
//        std::size_t line_number() const;
//    private:
//        EbnfTokenType m_type;
//        LexemeView m_lexeme_view;
//        EbnfTokenLiteralVariant m_literal;
//        std::size_t m_line_number;
//    };
}
#endif //VULKANWORKSPACE_EBNF_TOKEN_H
