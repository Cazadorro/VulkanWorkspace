//
// Created by Bolt on 5/31/2023.
//

#ifndef VULKANWORKSPACE_CHAR_H
#define VULKANWORKSPACE_CHAR_H
namespace uul{
    [[nodiscard]]
    constexpr bool is_alpha_lower(char c) noexcept{
        return c >= 'a' && c <= 'z';
    }

    [[nodiscard]]
    constexpr bool is_alpha_upper(char c) noexcept{
        return c >= 'A' && c <= 'Z';
    }
    [[nodiscard]]
    constexpr bool is_alpha(char c) noexcept{
        return is_alpha_lower(c) || is_alpha_upper(c);
    }

    [[nodiscard]]
    constexpr bool is_digit(char c) noexcept{
        return c >= '0' && c <= '9';
    }

    [[nodiscard]]
    constexpr bool is_octal(char c) noexcept{
        return c >= '0' && c <= '7';
    }

    [[nodiscard]]
    constexpr bool is_binary(char c) noexcept{
        return c == '0' || c == '1';
    }
    [[nodiscard]]
    constexpr bool is_valid_identifier_start_char(char c) noexcept{
        return is_alpha(c) || c == '_';
    }
    [[nodiscard]]
    constexpr bool is_valid_identifier_body_char(char c) noexcept{
        return is_valid_identifier_start_char(c) || is_digit(c);
    }
}
#endif //VULKANWORKSPACE_CHAR_H
