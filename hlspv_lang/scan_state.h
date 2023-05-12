//
// Created by Shae Bolt on 9/25/2022.
//

#ifndef VULKANWORKSPACE_SCAN_STATE_H
#define VULKANWORKSPACE_SCAN_STATE_H

#include <tl/expected.hpp>
#include <string_view>
#include <string>
#include <optional>
#include <functional>
#include <cstdint>


namespace hlspv {

    class LexemeView;

    struct ScanError {
        std::string_view source_code;
        std::size_t start_char_index = 0;
        std::size_t current_char_index = 0;
        std::size_t line_index = 0;
        std::string error_string;

        [[nodiscard]]
        std::size_t size() const;

        [[nodiscard]]
        std::string_view find_substr() const;
        [[nodiscard]]
        std::string create_info_string() const;
    };
    struct ScanState {
        std::string_view source_code;
        std::size_t start_char_index = 0;
        std::size_t current_char_index = 0;
        std::size_t line_index = 0;

        explicit ScanState(std::string_view source_code);

        [[nodiscard]]
        char current_char() const;

        [[nodiscard]]
        std::optional<char> look_ahead(std::size_t offset = 1) const;

        [[nodiscard]]
        std::optional<std::string_view>
        look_head_sequence(std::size_t offset, std::size_t size) const;

        [[nodiscard]]
        bool next_char_matches(char c) const;

        bool next_sequence_matches(std::string_view sequence);


        [[nodiscard]]
        std::size_t
        count_matched_chars_between(std::size_t start, std::size_t size,
                                    char c) const;
        void increment_newlines_from_start();

        [[nodiscard]]
        bool consume_if_next_char_matches(char c);

        bool consume_until_next_char_matches(char c, char escape_char);

        bool consume_until_next_char_matches(char c);

        bool consume_until_next_sequence_matches(std::string_view sequence);

        [[nodiscard]]
        bool
        next_char_matches(
                const std::function<bool(char, char)> &predicate) const;

        [[nodiscard]]
        bool
        next_char_matches(const std::function<bool(char)> &predicate) const;

        [[nodiscard]]
        bool consume_if_next_char_matches(
                const std::function<bool(char, char)> &predicate);

        [[nodiscard]]
        bool consume_if_next_char_matches(
                const std::function<bool(char)> &predicate);
        bool consume_while_next_char_matches(
                const std::function<bool(char, char)> &predicate);

        bool consume_while_next_char_matches(
                const std::function<bool(char)> &predicate);

        bool consume_while_current_char_matches(
                const std::function<bool(char, char)> &predicate);
        bool consume_until_current_sequence_matches(std::string_view sequence);


        bool consume_until_current_char_matches(char c);

        bool consume_until_current_char_matches(char c, char escape_char);


        bool start_next_scan_after_char(char c);

        bool start_next_scan_after_char(char c, char escape_char);

        //returns number of new lines between.
        bool start_next_scan_after_sequence(std::string_view sequence);

        [[nodiscard]]
        bool at_end() const;

        [[nodiscard]]
        std::size_t size() const;

        [[nodiscard]]
        LexemeView create_lexeme_view() const;

        void start_next_scan();

        void consume_count_chars(std::size_t n);

        void consume_current_char();

        [[nodiscard]]
        ScanError create_error(std::string_view error_string);

        bool consume_while_decimal_literal();

        bool consume_while_hexidecimal_literal();

        bool consume_while_octal_literal();

        bool consume_while_binary_literal();

        bool consume_if_float_exponent();

        bool consume_while_literal_body();

        [[nodiscard]]
        tl::expected<ScanState, ScanError> consume_if_identifier() const;

        [[nodiscard]]
        tl::expected<ScanState, ScanError> consume_if_unsigned_integer() const;

        template<typename TokenEnumType>
        [[nodiscard]]
        auto consume_token(TokenEnumType token_type) {
            auto token = create_token(token_type, *this);
            start_next_scan();
            return token;
        }

    };

    template<typename TokenT>
    struct ScanResult{
        std::vector<TokenT> tokens;
        std::vector<ScanError> errors;
    };

} // hlspv

#endif //VULKANWORKSPACE_SCAN_STATE_H
