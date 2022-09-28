//
// Created by Shae Bolt on 9/19/2022.
//

#include "parse.h"
#include <optional>
#include <vector>

namespace hlspv{

    struct ParseState {
        std::span<const Token> tokens;
        std::size_t start_token_index = 0;
        std::size_t current_token_index = 0;
        explicit ParseState( std::span<const Token> tokens) : tokens(tokens) {

        }

        [[nodiscard]]
        Token current_token() const {
            return tokens[current_token_index];
        }

        [[nodiscard]]
        std::optional<Token> look_ahead(std::size_t offset = 1) const {
            auto look_ahead_index = current_token_index + offset;
            if (look_ahead_index < tokens.size()) {
                return tokens[look_ahead_index];
            } else {
                return std::nullopt;
            }
        }
        [[nodiscard]]
        bool next_matches(TokenType token_type) const{
            if (auto next_token = look_ahead(1); next_token.has_value()) {
                return next_token->type() == token_type;
            } else {
                return false;
            }
        }
        [[nodiscard]]
        bool next_matches(const std::vector<TokenType>& token_types) const{
            for(auto token_type : token_types){
                if(next_matches(token_type)){
                    return true;
                }
            }
            return false;
        }
        [[nodiscard]]
        std::optional<Token> look_behind(std::size_t offset = 1) const {
            if(current_token_index < offset){
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
        bool at_end() const{
            return current_token_index >= tokens.size() && current_token().type() == TokenType::EndOfFile;
        }
        std::vector<Token> consume_count_token(std::size_t n) {
            auto previous_index = current_token_index;
            current_token_index += n;
            return {tokens.begin() + previous_index, tokens.begin() + current_token_index};
        }

        Token consume_current_token() {
            return consume_count_token(1)[0];
        }

        [[nodiscard]]
        std::optional<Token> consume_if_next_matches(const std::vector<TokenType>& token_types){
            if(next_matches(token_types)){
                return consume_current_token();
            }
            return std::nullopt;
        }

    };
//    std::optional<Expression> parse_unary_operator(ParseState& parse_state){
//        if(auto unary_terminal = parse_state.consume_if_next_matches({TokenType::Minus, TokenType::Plus, TokenType::Not}); unary_terminal.has_value()){
//            return UnaryOperator{unary_terminal.value()};
//        }
//        return std::nullopt;
//    }
//    std::optional<Expression> parse_unary(ParseState& parse_state){
//        auto current_parse_state = parse_state;
//        if(auto unary_operator = parse_unary_operator(current_parse_state); unary_operator.has_value()){
//            if(auto right_side = parse_unary(current_parse_state); right_side.has_value()){
//                parse_state = current_parse_state;
//                return UnaryExpression{unary_operator.value(), right_side.value()};
//            }
//        }
//        return std::nullopt;
//    }
    ParseResult parse(std::span<const Token> tokens){
        ParseState parse_state(tokens);

        while(!parse_state.at_end()){

        }
        return {};
    }

}