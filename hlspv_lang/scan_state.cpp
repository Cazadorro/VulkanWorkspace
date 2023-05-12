//
// Created by Shae Bolt on 9/25/2022.
//

#include "scan_state.h"
#include "lexeme_view.h"
#include "utils.h"
#include <fmt/core.h>

namespace hlspv {

    std::size_t ScanError::size() const {
        return (current_char_index + 1) - start_char_index;
    }

    std::string_view ScanError::find_substr() const {
        return source_code.substr(start_char_index, size());
    }

    std::string ScanError::create_info_string() const{
        return fmt::format("at line {}, {}: {}", line_index, error_string,
                           find_substr());
    }

    ScanState::ScanState(std::string_view source_code) : source_code(
            source_code) {

    }

    std::optional<char> ScanState::look_ahead(std::size_t offset) const {
        auto look_ahead_index = current_char_index + offset;
        if (look_ahead_index < source_code.size()) {
            return source_code[look_ahead_index];
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string_view>
    ScanState::look_head_sequence(std::size_t offset, std::size_t size) const{
        auto look_ahead_start = current_char_index + offset;
        try {
            return source_code.substr(look_ahead_start, size);
        } catch (std::exception &/*e*/) {
            return std::nullopt;
        }
    }

    bool ScanState::next_char_matches(char c) const  {
        if (auto next_char = look_ahead(1); next_char.has_value()) {
            return next_char == c;
        } else {
            return false;
        }
    }

    bool ScanState::next_sequence_matches(std::string_view sequence)  {
        if (auto look_ahead_sequence = look_head_sequence(1,
                                                          sequence.size()); look_ahead_sequence.has_value()) {
            return look_ahead_sequence == sequence;
        } else {
            return false;
        }
    }

    std::size_t
    ScanState::count_matched_chars_between(std::size_t start, std::size_t size,
                                           char c) const {
        return std::count(source_code.begin() + start,
                          source_code.begin() + start + size, c);
    }

    void ScanState::increment_newlines_from_start(){
        auto newline_count = count_matched_chars_between(start_char_index,
                                                         size(),
                                                         '\n');
        line_index += newline_count;
    }

    bool ScanState::consume_if_next_char_matches(char c) {
        if (next_char_matches(c)) {
            consume_current_char();
            return true;
        }
        return false;
    }

    bool ScanState::consume_until_next_char_matches(char c, char escape_char) {
        auto previous_char = current_char();
        bool previous_char_escaped = false;
        while (((previous_char != escape_char || previous_char_escaped)
                && !next_char_matches(c))
               && !at_end()) {
            consume_current_char();
            previous_char_escaped = (previous_char == escape_char &&
                                     !previous_char_escaped);
            previous_char = current_char();
        }
        return !at_end();
    }

    bool ScanState::consume_until_next_char_matches(char c) {
        while (!next_char_matches(c) && !at_end()) {
            consume_current_char();
        }
        return !at_end();
    }

    bool
    ScanState::consume_until_next_sequence_matches(std::string_view sequence) {
        while (!next_sequence_matches(sequence) && !at_end()) {
            consume_current_char();
        }
        return !at_end();
    }

    bool ScanState::next_char_matches(
            const std::function<bool(char, char)> &predicate) const {
        if (auto next_char = look_ahead(1); next_char.has_value()) {
            return predicate(current_char(), next_char.value());
        } else {
            return false;
        }
    }

    bool ScanState::next_char_matches(
            const std::function<bool(char)> &predicate) const {
        return next_char_matches([predicate](char/**/, char c) {
            return predicate(c);
        });
    }

    bool ScanState::consume_if_next_char_matches(
            const std::function<bool(char, char)> &predicate) {
        if (next_char_matches(predicate)) {
            consume_current_char();
            return true;
        }
        return false;
    }

    bool ScanState::consume_if_next_char_matches(
            const std::function<bool(char)> &predicate)  {
        if (next_char_matches(predicate)) {
            consume_current_char();
            return true;
        }
        return false;
    }

    bool ScanState::consume_while_next_char_matches(
            const std::function<bool(char, char)> &predicate)  {
        auto previous_current_index = current_char_index;
        while (next_char_matches(predicate) && !at_end()) {
            consume_current_char();
        }
        return previous_current_index != current_char_index;
    }

    bool ScanState::consume_while_next_char_matches(
            const std::function<bool(char)> &predicate) {
        auto previous_current_index = current_char_index;
        while (next_char_matches(predicate) && !at_end()) {
            consume_current_char();
        }
        return previous_current_index != current_char_index;
    }

    bool ScanState::consume_while_current_char_matches(
            const std::function<bool(char, char)> &predicate) {
        bool found = consume_while_next_char_matches(predicate);
        consume_current_char();
        return found;
    }

    bool ScanState::consume_until_current_sequence_matches(
            std::string_view sequence) {
        bool found = consume_until_next_sequence_matches(sequence);
        consume_count_chars(sequence.size());
        return found;
    }

    bool ScanState::consume_until_current_char_matches(char c) {
        bool found = consume_until_next_char_matches(c);
        consume_current_char();
        return found;
    }

    bool
    ScanState::consume_until_current_char_matches(char c, char escape_char) {
        bool found = consume_until_next_char_matches(c, escape_char);
        consume_current_char();
        return found;
    }

    bool ScanState::start_next_scan_after_char(char c) {
        bool found = consume_until_current_char_matches(c);
        start_next_scan();
        return found;
    }

    bool ScanState::start_next_scan_after_char(char c, char escape_char)  {
        bool found = consume_until_current_char_matches(c, escape_char);
        start_next_scan();
        return found;
    }

    bool ScanState::start_next_scan_after_sequence(std::string_view sequence) {
        bool found = consume_until_current_sequence_matches(sequence);
        start_next_scan();
        return found;
    }

    bool ScanState::at_end() const {
        return current_char_index >= source_code.size();
    }

    std::size_t ScanState::size() const {
        return (current_char_index + 1) - start_char_index;
    }

    LexemeView ScanState::create_lexeme_view() const {
        return LexemeView{source_code, start_char_index, current_char_index};
    }

    void ScanState::start_next_scan() {

        start_char_index = current_char_index += 1;
        if (start_char_index >= source_code.size()) {
            start_char_index = source_code.size();
        }
        current_char_index = start_char_index;
    }

    void ScanState::consume_count_chars(std::size_t n) {
        current_char_index += n;
        line_index += count_matched_chars_between(current_char_index, n,
                                                  '\n');
    }

    void ScanState::consume_current_char() {
        consume_count_chars(1);
    }

    bool ScanState::consume_while_decimal_literal() {
        return consume_while_next_char_matches(
                [](char previous_char, char current_char) -> bool {
                    return std::isdigit(current_char) ||
                           (current_char == '_' &&
                            std::isdigit(previous_char));
                });
    }

    bool ScanState::consume_while_hexidecimal_literal() {
        return consume_while_next_char_matches(
                [](char previous_char, char current_char) -> bool {
                    return std::isxdigit(current_char) ||
                           (current_char == '_' &&
                            std::isxdigit(previous_char));
                });
    }

    bool ScanState::consume_while_octal_literal() {
        return consume_while_next_char_matches(
                [](char previous_char, char current_char) -> bool {
                    return is_octal(current_char) ||
                           (current_char == '_' && is_octal(previous_char));
                });
    }

    bool ScanState::consume_while_binary_literal() {
        return consume_while_next_char_matches(
                [](char previous_char, char current_char) -> bool {
                    return is_binary(current_char) ||
                           (current_char == '_' &&
                            is_binary(previous_char));
                });
    }

    bool ScanState::consume_if_float_exponent() {
        auto previous_char_index = current_char_index;
        if (!consume_if_next_char_matches(
                [](char current_char) -> bool {
                    return current_char == 'E' || current_char == 'e';
                })) {
            current_char_index = previous_char_index;
            return false;
        }
        if (!consume_if_next_char_matches(
                [](char current_char) -> bool {
                    return current_char == '+' || current_char == '-';
                })) {
            current_char_index = previous_char_index;
            return false;
        }
        if (!consume_while_decimal_literal()) {
            current_char_index = previous_char_index;
            return false;
        }
        return true;
    }

    bool ScanState::consume_while_literal_body() {
        return consume_while_next_char_matches(is_valid_identifier_body_char);
    }

    ScanError ScanState::create_error(std::string_view error_string) {
        return ScanError{source_code, start_char_index, current_char_index,
                         line_index, std::string{error_string}};
    }

    tl::expected<ScanState, ScanError> ScanState::consume_if_identifier() const{
        auto current_state = *this;
        if(!is_valid_identifier_start_char(current_state.current_char())){
            return tl::make_unexpected(current_state.create_error("Invalid start char"));
        }
        current_state.consume_current_char();
        current_state.consume_while_literal_body();
        return current_state;
    }
    tl::expected<ScanState, ScanError> ScanState::consume_if_unsigned_integer() const {
        auto current_state = *this;
        if(!is_digit(current_state.current_char())){
            return tl::make_unexpected(current_state.create_error("Invalid digit character"));
        }
        current_state.consume_current_char();
        current_state.consume_while_next_char_matches(is_digit);
        return current_state;
    }
    char ScanState::current_char() const {
        if(at_end()){
            return '\0';
        }else{
            return source_code[current_char_index];
        }
    }




} // hlspv