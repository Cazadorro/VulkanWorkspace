//
// Created by Shae Bolt on 9/18/2022.
//

#include "scan.h"
#include "token.h"
#include "utils.h"
#include <fmt/core.h>
#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <array>
#include <stdexcept>

namespace hlspv {

    template<typename Key, typename Value, std::size_t Size>
    struct ConstexprMap {
        std::array<std::pair<Key, Value>, Size> data;

        [[nodiscard]]
        constexpr Value at(const Key &key) const {
            const auto itr = std::find_if(begin(data), end(data),
                                          [&key](const auto &v) {
                                              return v.first == key;
                                          });
            if (itr != end(data)) {
                return itr->second;
            } else {
                throw std::range_error("Not Found");
            }
        }

        [[nodiscard]]
        constexpr std::optional<Value> get(const Key &key) const {
            const auto itr = std::find_if(begin(data), end(data),
                                          [&key](const auto &v) {
                                              return v.first == key;
                                          });
            if (itr != end(data)) {
                return itr->second;
            } else {
                return std::nullopt;
            }
        }

    };

    using KeywordMap = ConstexprMap<std::string_view, TokenType, 27>;

    using namespace std::literals::string_view_literals;
    static constexpr std::array<std::pair<std::string_view, TokenType>, 27> keyword_pairs = {
            {
                    {"true"sv, TokenType::BooleanLiteral},
                    {"false"sv, TokenType::BooleanLiteral},
                    {"let"sv, TokenType::Let},
                    {"const"sv, TokenType::Const},
                    {"fn"sv, TokenType::Function},
                    {"struct"sv, TokenType::Struct},
                    {"enum"sv, TokenType::Enum},
                    {"trait"sv, TokenType::Trait},
                    {"impl"sv, TokenType::Impl},
                    {"type"sv, TokenType::Type},
                    {"loop"sv, TokenType::Loop},
                    {"for"sv, TokenType::For},
                    {"while"sv, TokenType::While},
                    {"continue"sv, TokenType::Continue},
                    {"break"sv, TokenType::Break},
                    {"match"sv, TokenType::Match},
                    {"as"sv, TokenType::As},
                    {"if"sv, TokenType::If},
                    {"else"sv, TokenType::Else},
                    {"elif"sv, TokenType::Elif},
                    {"and"sv, TokenType::And},
                    {"or"sv, TokenType::Or},
                    {"xor"sv, TokenType::Xor},
                    {"not"sv, TokenType::Not},
                    {"pub"sv, TokenType::Pub},
                    {"mod"sv, TokenType::Mod},
                    {"return"sv, TokenType::Return}
            }
    };
    static constexpr auto keyword_map = KeywordMap{{keyword_pairs}};


    struct ScanState;



    template<typename T>
    Token create_token(TokenType token_type, const ScanState &scan_state);

    template<TokenType token_type>
    Token create_token(const ScanState &scan_state);

    Token create_token(TokenType token_type, const ScanState &scan_state);



    template<typename T = std::monostate>
    Token create_token(TokenType token_type, const ScanState &scan_state) {
        auto lexeme_view = scan_state.create_lexeme_view();
        if constexpr (std::is_same_v<T, std::monostate>) {
            return {token_type, lexeme_view, {},
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, std::string>) {
            //TODO strip escape characters!
            return {token_type, lexeme_view, std::string(trim(lexeme_view.to_sv(), 1)),
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, double>) {
            //TODO stod could fail? how would we propogate such errors here?
            std::string stripped = remove_char(lexeme_view.to_sv(), '_');

            //TODO we need to insert a dot here for C++ to properly parse this...
            try {
                double value = std::stod(std::string(stripped));
                return {token_type, lexeme_view, value,
                        scan_state.line_index};
            }catch (std::out_of_range& /*e*/){
                if(!stripped.empty()){
                    if(stripped[0] == '-'){
                        return {token_type, lexeme_view, -std::numeric_limits<double>::infinity(),
                                scan_state.line_index};
                    }
                }
                return {token_type, lexeme_view, std::numeric_limits<double>::infinity(),
                        scan_state.line_index};
            }
        } else if constexpr (std::is_same_v<T, std::uint64_t>) {
            std::string stripped = remove_char(lexeme_view.to_sv(), '_');
            std::uint64_t value;
            if (stripped.size() > 2 && stripped[1] == 'x') {
                stripped = trim_left(stripped, 2);
                value = std::stoull(stripped, nullptr, 16);
            } else if (stripped.size() > 2 && stripped[1] == 'o') {
                stripped = trim_left(stripped, 2);
                value = std::stoull(stripped, nullptr, 8);
            } else if (stripped.size() > 2 && stripped[1] == 'b') {
                stripped = trim_left(stripped, 2);
                value = std::stoull(stripped, nullptr, 2);
            } else {
                value = std::stoull(std::string(stripped));
            }
            return {token_type, lexeme_view, value,
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            std::string stripped = remove_char(lexeme_view.to_sv(), '_');
            std::int64_t value = std::stoll(std::string(stripped));
            return {token_type, lexeme_view, value,
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, bool>) {
            //TODO prepare for failure of programmer here? should only ever be true or false.
            bool value = lexeme_view.to_string() == "true";
            return {token_type, lexeme_view, value,
                    scan_state.line_index};
        } else {
            //to make static assert work
            // https://stackoverflow.com/a/64354296/
            []<bool flag = false>() { static_assert(flag, "no match"); }();
        }
    }

    template<TokenType token_type>
    Token create_token(const ScanState &scan_state) {
        switch (token_type) {
            case TokenType::String: {
                return create_token<std::string>(token_type, scan_state);
                break;
            }
            case TokenType::DecimalIntegerLiteral: {
                return create_token<std::int64_t>(token_type, scan_state);
                break;
            }
            case TokenType::DecimalUnsignedLiteral:
            case TokenType::BinaryLiteral:
            case TokenType::HexidecimalLiteral:
            case TokenType::OctalLiteral: {
                return create_token<std::uint64_t>(token_type, scan_state);
                break;
            }
            case TokenType::DecimalFloatingPointLiteral: {
                return create_token<double>(token_type, scan_state);
                break;
            }
            case TokenType::BooleanLiteral: {
                return create_token<bool>(token_type, scan_state);
                break;
            }
            default:{
                return create_token<std::monostate>(token_type, scan_state);
            }
        }
    }

    Token create_token(TokenType token_type, const ScanState &scan_state) {
        switch (token_type) {
            case TokenType::String: {
                return create_token<std::string>(token_type, scan_state);
                break;
            }
            case TokenType::DecimalIntegerLiteral: {
                return create_token<std::int64_t>(token_type, scan_state);
                break;
            }
            case TokenType::DecimalUnsignedLiteral:
            case TokenType::BinaryLiteral:
            case TokenType::HexidecimalLiteral:
            case TokenType::OctalLiteral: {
                return create_token<std::uint64_t>(token_type, scan_state);
                break;
            }
            case TokenType::DecimalFloatingPointLiteral: {
                return create_token<double>(token_type, scan_state);
                break;
            }
            case TokenType::BooleanLiteral: {
                return create_token<bool>(token_type, scan_state);
                break;
            }
            default:{
                return create_token<std::monostate>(token_type, scan_state);
            }
        }
    }


    ScanResult<Token> scan(std::string_view source_code) {
        ScanState scan_state(source_code);
        std::vector<Token> tokens;
        std::vector<ScanError> errors;
        while (!scan_state.at_end()) {
            auto last_scan_state = scan_state;
            switch (scan_state.current_char()) {
                case '(' : {
                    tokens.push_back(scan_state.consume_token(TokenType::LeftParenthesis));
                    break;
                }
                case ')' : {
                    tokens.push_back(scan_state.consume_token(TokenType::RightParenthesis));
                    break;
                }
                case '{' : {
                    tokens.push_back(scan_state.consume_token(TokenType::LeftCurlyBrace));
                    break;
                }
                case '}' : {
                    tokens.push_back(scan_state.consume_token(TokenType::RightCurlyBrace));
                    break;
                }
                case '[' : {
                    tokens.push_back(scan_state.consume_token(TokenType::LeftSquareBracket));
                    break;
                }
                case ']' : {
                    tokens.push_back(scan_state.consume_token(TokenType::RightSquareBracket));
                    break;
                }
                case ',' : {
                    tokens.push_back(scan_state.consume_token(TokenType::Comma));
                    break;
                }
                case '+' : {
                    tokens.push_back(scan_state.consume_token(TokenType::Plus));
                    break;
                }
                case ';' : {
                    tokens.push_back(
                            scan_state.consume_token(TokenType::Semicolon));
                    break;
                }
                case '*' : {
                    tokens.push_back(scan_state.consume_token(TokenType::Star));
                    break;
                }
                case '?' : {
                    tokens.push_back(
                            scan_state.consume_token(TokenType::QuestionMark));
                    break;
                }
                case '!' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(
                                scan_state.consume_token(TokenType::NotEquals));
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::ExclamationPoint));
                    }
                    break;
                }
                case '=' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(scan_state.consume_token(TokenType::DoubleEquals));
                    } else if (scan_state.consume_if_next_char_matches('>')) {
                        tokens.push_back(scan_state.consume_token(TokenType::LeftMatchArrow));
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::Equals));
                    }
                    break;
                }
                case '-' : {
                    if (scan_state.consume_if_next_char_matches('>')) {
                        tokens.push_back(
                                scan_state.consume_token(TokenType::LeftArrow));
                    } else if (scan_state.next_char_matches(is_digit)) {
                        scan_state.consume_while_decimal_literal();
                        if (scan_state.next_char_matches('.')) {
                            if (scan_state.consume_while_decimal_literal()) {
                                scan_state.consume_if_float_exponent();
                                tokens.push_back(
                                        scan_state.consume_token(TokenType::DecimalFloatingPointLiteral));
                            } else {
                                errors.push_back(scan_state.create_error(
                                        "no decimal following . in floating point"));
                                scan_state.start_next_scan();
                            }
                        } else if (scan_state.consume_if_float_exponent()) {
                            tokens.push_back(scan_state.consume_token(TokenType::DecimalFloatingPointLiteral));
                        } else {
                            tokens.push_back(
                                    scan_state.consume_token(TokenType::DecimalIntegerLiteral));
                        }

                    } else {
                        tokens.push_back(
                                scan_state.consume_token(TokenType::Minus));
                    }
                    break;
                }
                case '.' : {
                    if (scan_state.consume_if_next_char_matches('.')) {
                        tokens.push_back(
                                scan_state.consume_token(TokenType::Elipses));
                    } else {
                        tokens.push_back(
                                scan_state.consume_token(TokenType::Dot));
                    }
                    break;
                }
                case ':' : {
                    if (scan_state.consume_if_next_char_matches(':')) {
                        tokens.push_back(scan_state.consume_token(TokenType::DoubleColon));
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::Colon));
                    }
                    break;
                }
                case '<' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(scan_state.consume_token(TokenType::GreaterThanEquals));
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::GreaterThan));
                    }
                    break;
                }
                case '>' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(scan_state.consume_token(TokenType::LessThanEquals));
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::LessThan));
                    }
                    break;
                }
                case '/' : {
                    if (scan_state.consume_if_next_char_matches('/')) {
                        scan_state.start_next_scan_after_char(
                                '\n');//will auto +1 new line.
                    } else if (scan_state.consume_if_next_char_matches('*')) {
                        bool finished = scan_state.start_next_scan_after_sequence(
                                "*/");
                        if (!finished) {
                            errors.push_back(scan_state.create_error(
                                    "/* sequence not terminated"));
                            scan_state = last_scan_state;
                            scan_state.start_next_scan(); // continue to try to parse.
                        }
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::ForwardSlash));
                    }
                    break;
                }

                case ' ':
                case '\r':
                case '\t': {
                    scan_state.start_next_scan();
                    break;
                }

                case '\n': {
                    scan_state.line_index += 1;
                    scan_state.start_next_scan();
                    break;
                }
                case '"': {
                    bool finished = scan_state.consume_until_current_char_matches(
                            '"', '\\');
                    if (!finished) {
                        errors.push_back(scan_state.create_error("\" sequence not terminated"));
                        scan_state = last_scan_state;
                        scan_state.start_next_scan(); // continue to try to parse.
                    } else {
                        tokens.push_back(scan_state.consume_token(TokenType::String));
                    }
                    break;
                }

                case '0': {
                    if (scan_state.consume_if_next_char_matches('x')) {
                        if (scan_state.consume_while_hexidecimal_literal()) {
                            tokens.push_back(
                                    scan_state.consume_token(TokenType::HexidecimalLiteral));
                        } else {
                            errors.push_back(scan_state.create_error(
                                    "Started hexidecimal literal with out body [0x...]"));
                            scan_state.start_next_scan();
                        }
                        break;
                    } else if (scan_state.consume_if_next_char_matches('o')) {
                        if (scan_state.consume_while_octal_literal()) {
                            tokens.push_back(scan_state.consume_token(TokenType::OctalLiteral));
                        } else {
                            errors.push_back(scan_state.create_error(
                                    "Started octal literal with out body [0o...]"));
                            scan_state.start_next_scan();
                        }
                        break;
                    } else if (scan_state.consume_if_next_char_matches('b')) {
                        if (scan_state.consume_while_binary_literal()) {
                            tokens.push_back(scan_state.consume_token(TokenType::BinaryLiteral));
                        } else {
                            errors.push_back(scan_state.create_error(
                                    "Started binary literal with out body [0b...]"));
                            scan_state.start_next_scan();

                        }
                        break;
                    }
                    //fallthrough on purpose.
                }
                default : {
                    if (std::isdigit(scan_state.current_char())) {
                        scan_state.consume_while_decimal_literal();
                        if (scan_state.consume_if_next_char_matches('.')) {
                            if (scan_state.consume_while_decimal_literal()) {
                                scan_state.consume_if_float_exponent();
                                tokens.push_back(scan_state.consume_token(TokenType::DecimalFloatingPointLiteral));
                            } else {
                                errors.push_back(scan_state.create_error(
                                        "no decimal following . in floating point"));
                                scan_state.start_next_scan();
                            }
                        } else if (scan_state.consume_if_float_exponent()) {
                            tokens.push_back(scan_state.consume_token(TokenType::DecimalFloatingPointLiteral));
                        } else {
                            tokens.push_back(scan_state.consume_token(TokenType::DecimalIntegerLiteral));
                        }
                    } else if (is_valid_identifier_start_char(
                            scan_state.current_char())) {
                        scan_state.consume_while_literal_body();
                        if (auto keyword_result = keyword_map.get(
                                    scan_state.create_lexeme_view().to_sv()); keyword_result.has_value()) {
                            tokens.push_back(scan_state.consume_token(keyword_result.value()));
                        }else{
                            tokens.push_back(scan_state.consume_token(TokenType::Identifier));
                        }
                    }else{
                        errors.push_back(
                                scan_state.create_error("Unexpected character"));
                        scan_state.start_next_scan();
                    }
                    break;
                }
            }
        }
        if(!tokens.empty()){
            tokens.push_back(Token{TokenType::EndOfFile, tokens.back().lexeme_view(), {}, scan_state.line_index});
        }else{
            tokens.push_back(Token{TokenType::EndOfFile, {source_code, 0,0}, {}, scan_state.line_index});
        }
        return {tokens, errors};
    }
}