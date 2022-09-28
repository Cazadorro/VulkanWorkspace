//
// Created by Shae Bolt on 9/28/2022.
//

#ifndef VULKANWORKSPACE_PARSE_STATE_H
#define VULKANWORKSPACE_PARSE_STATE_H

#include <fmt/core.h>
#include <string>
#include <span>
#include <optional>
#include <vector>

namespace hlspv {
    template<typename TokenT>
    struct ParseError {
        std::span<const TokenT> tokens;
        std::string error_string;

        [[nodiscard]]
        std::size_t size() const {
            return tokens.size();
        }

        [[nodiscard]]
        std::span<const TokenT> get_tokens() const {
            return tokens;
        }

        [[nodiscard]]
        std::string create_info_string() const {
            if (size() == 0) {
                return fmt::format("at line (unknown), {}", error_string);
            }
            return fmt::format("at line {}, {}:\n\t{}", tokens[0].line_number(),
                               error_string,
                               merge(tokens.front().lexeme_view(), tokens.back().lexeme_view()).to_sv());
        }
    };

    template<typename TokenT>
    struct ParseState {
        using TokenTypeT = decltype(((TokenT *) nullptr)->type());
        std::span<const TokenT> tokens;
        std::size_t start_token_index = 0;
        std::size_t current_token_index = 0;

        explicit ParseState(std::span<const TokenT> tokens) : tokens(
                tokens) {

        }

        [[nodiscard]]
        std::size_t size() const {
            return (current_token_index + 1) - start_token_index;
        }

        [[nodiscard]]
        std::size_t get_current_token_index() const {
            return current_token_index;
        }

        [[nodiscard]]
        TokenT current_token() const {
            return tokens[current_token_index];
        }

        [[nodiscard]]
        std::optional<TokenT> look_ahead(std::size_t offset = 1) const {
            auto look_ahead_index = current_token_index + offset;
            if (look_ahead_index < tokens.size()) {
                return tokens[look_ahead_index];
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]]
        bool next_matches(TokenTypeT token_type) const {
            if (auto next_token = look_ahead(1); next_token.has_value()) {
                return next_token->type() == token_type;
            } else {
                return false;
            }
        }

        [[nodiscard]]
        bool next_matches(const std::vector<TokenTypeT> &token_types) const {
            for (auto token_type: token_types) {
                if (next_matches(token_type)) {
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]]
        std::optional<TokenTypeT> look_behind(std::size_t offset = 1) const {
            if (current_token_index < offset) {
                return std::nullopt;
            }
            auto look_behind_index = current_token_index - offset;
            if (look_behind_index < tokens.size()) {
                return tokens[look_behind_index];
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]]
        bool at_end() const {
            return current_token_index >= tokens.size() ||
                   current_token().type() == TokenTypeT::EndOfFile;
        }

        std::vector<TokenT> consume_count_token(std::size_t n) {
            auto previous_index = current_token_index;
            current_token_index += n;
            return {tokens.begin() + previous_index,
                    tokens.begin() + current_token_index};
        }

        TokenT consume_current_token() {
            return consume_count_token(1)[0];
        }

        [[nodiscard]]
        std::optional<TokenT>
        consume_if_next_matches(const std::vector<TokenTypeT> &token_types) {
            if (next_matches(token_types)) {
                return consume_current_token();
            }
            return std::nullopt;
        }

        [[nodiscard]]
        ParseError<TokenT> create_error(std::string_view error_string) {
            return ParseError{tokens.subspan(start_token_index, size()),
                              std::string(error_string)};
        }

        void reset_start_index(std::size_t index) {
            start_token_index = index;
        }

        void reset_start_index() {
            reset_start_index(current_token_index);
        }
    };

}
#endif //VULKANWORKSPACE_PARSE_STATE_H
