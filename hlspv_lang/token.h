//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_TOKEN_H
#define VULKANWORKSPACE_TOKEN_H

#include "token_type.h"
#include <string>
#include <string_view>
#include <variant>
#include <cstdint>

namespace hlspv {
    using LiteralVariant = std::variant<std::monostate, std::string, std::uint64_t, std::int64_t, double, bool>;
    class Token {
    public:
        Token(TokenType type, std::string_view lexeme, LiteralVariant literal, std::size_t line_number);
        [[nodiscard]]
        std::string lexeme() const;
        [[nodiscard]]
        TokenType type() const;
    private:
        TokenType m_type;
        std::string m_lexeme;
        LiteralVariant m_literal;
        std::size_t m_line_number;
    };
}
#endif //VULKANWORKSPACE_TOKEN_H
