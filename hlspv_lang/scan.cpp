//
// Created by Shae Bolt on 9/18/2022.
//

#include "scan.h"
#include "token.h"
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

    //https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
    [[nodiscard]]
    std::string trim_left(std::string_view string, std::size_t n) {
        std::string trimmed_string(string);
        trimmed_string.erase(trimmed_string.begin(),
                             trimmed_string.begin() + n);
        return trimmed_string;
    }

    [[nodiscard]]
    std::string trim_right(std::string_view string, std::size_t n) {
        std::string trimmed_string(string);
        trimmed_string.erase(trimmed_string.end() - n, trimmed_string.end());
        return trimmed_string;
    }

    [[nodiscard]]
    std::string
    trim(std::string_view string, std::size_t left_n, std::size_t right_n) {
        auto trimmed_string = trim_left(string, left_n);
        trimmed_string = trim_right(trimmed_string, right_n);
        return trimmed_string;
    }

    [[nodiscard]]
    std::string trim(std::string_view string, std::size_t n) {
        return trim(string, n, n);
    }

    std::string remove_char(std::string_view string, char c) {
        std::string trimmed_string(string);
        trimmed_string.erase(
                std::remove(trimmed_string.begin(), trimmed_string.end(), c),
                trimmed_string.end());
        return trimmed_string;
    }

    bool is_digit(char c) {
        return std::isdigit(c);
    }

    bool is_octal(char c) {
        return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
               c == '5' || c == '6' || c == '7';
    }

    bool is_binary(char c) {
        return c == '0' || c == '1';
    }

    bool is_valid_literal_start_char(char c) {
        return std::isalpha(c) || c == '_';
    }

    bool is_valid_literal_body_char(char c) {
        return is_valid_literal_start_char(c) || std::isdigit(c);
    }

    struct ScanState;



    template<typename T>
    Token create_token(TokenType token_type, const ScanState &scan_state);

    template<TokenType token_type>
    Token create_token(const ScanState &scan_state);

    Token create_token(TokenType token_type, const ScanState &scan_state);

    struct ScanState {
        std::string_view source_code;
        std::size_t start_char_index = 0;
        std::size_t current_char_index = 0;
        std::size_t line_index = 0;

        explicit ScanState(std::string_view source_code) : source_code(
                source_code) {

        }

        [[nodiscard]]
        char current_char() const {
            return source_code[current_char_index];
        }

        [[nodiscard]]
        std::optional<char> look_ahead(std::size_t offset = 1) const {
            auto look_ahead_index = current_char_index + offset;
            if (look_ahead_index < source_code.size()) {
                return source_code[look_ahead_index];
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]]
        std::optional<std::string_view>
        look_head_sequence(std::size_t offset, std::size_t size) const{
            auto look_ahead_start = current_char_index + offset;
            try {
                return source_code.substr(look_ahead_start, size);
            } catch (std::exception &/*e*/) {
                return std::nullopt;
            }
        }

        [[nodiscard]]
        bool next_char_matches(char c) const {
            if (auto next_char = look_ahead(1); next_char.has_value()) {
                return next_char == c;
            } else {
                return false;
            }
        }

        bool next_sequence_matches(std::string_view sequence) {
            if (auto look_ahead_sequence = look_head_sequence(1,
                                                              sequence.size()); look_ahead_sequence.has_value()) {
                return look_ahead_sequence == sequence;
            } else {
                return false;
            }
        }


        [[nodiscard]]
        std::size_t
        count_matched_chars_between(std::size_t start, std::size_t size,
                                    char c) const {
            return std::count(source_code.begin() + start,
                              source_code.begin() + start + size, c);
        }

        void increment_newlines_from_start() {
            auto newline_count = count_matched_chars_between(start_char_index,
                                                             size(),
                                                             '\n');
            line_index += newline_count;
        }

        [[nodiscard]]
        bool consume_if_next_char_matches(char c) {
            if (next_char_matches(c)) {
                consume_current_char();
                return true;
            }
            return false;
        }

        bool consume_until_next_char_matches(char c, char escape_char) {
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

        bool consume_until_next_char_matches(char c) {
            while (!next_char_matches(c) && !at_end()) {
                consume_current_char();
            }
            return !at_end();
        }

        bool consume_until_next_sequence_matches(std::string_view sequence) {
            while (!next_sequence_matches(sequence) && !at_end()) {
                consume_current_char();
            }
            return !at_end();
        }

        [[nodiscard]]
        bool
        next_char_matches(
                const std::function<bool(char, char)> &predicate) const {
            if (auto next_char = look_ahead(1); next_char.has_value()) {
                return predicate(current_char(), next_char.value());
            } else {
                return false;
            }
        }

        [[nodiscard]]
        bool
        next_char_matches(const std::function<bool(char)> &predicate) const {
            return next_char_matches([predicate](char/**/, char c) {
                return predicate(c);
            });
        }

        [[nodiscard]]
        bool consume_if_next_char_matches(
                const std::function<bool(char, char)> &predicate) {
            if (next_char_matches(predicate)) {
                consume_current_char();
                return true;
            }
            return false;
        }

        [[nodiscard]]
        bool consume_if_next_char_matches(
                const std::function<bool(char)> &predicate) {
            if (next_char_matches(predicate)) {
                consume_current_char();
                return true;
            }
            return false;
        }

        bool consume_while_next_char_matches(
                const std::function<bool(char, char)> &predicate) {
            auto previous_current_index = current_char_index;
            while (next_char_matches(predicate) && !at_end()) {
                consume_current_char();
            }
            return previous_current_index != current_char_index;
        }

        bool consume_while_next_char_matches(
                const std::function<bool(char)> &predicate) {
            auto previous_current_index = current_char_index;
            while (next_char_matches(predicate) && !at_end()) {
                consume_current_char();
            }
            return previous_current_index != current_char_index;
        }

        bool consume_while_current_char_matches(
                const std::function<bool(char, char)> &predicate) {
            bool found = consume_while_next_char_matches(predicate);
            consume_current_char();
            return found;
        }

        bool consume_until_current_sequence_matches(std::string_view sequence) {
            bool found = consume_until_next_sequence_matches(sequence);
            consume_count_chars(sequence.size());
            return found;
        }


        bool consume_until_current_char_matches(char c) {
            bool found = consume_until_next_char_matches(c);
            consume_current_char();
            return found;
        }

        bool consume_until_current_char_matches(char c, char escape_char) {
            bool found = consume_until_next_char_matches(c, escape_char);
            consume_current_char();
            return found;
        }


        bool start_next_scan_after_char(char c) {
            bool found = consume_until_current_char_matches(c);
            start_next_scan();
            return found;
        }

        bool start_next_scan_after_char(char c, char escape_char) {
            bool found = consume_until_current_char_matches(c, escape_char);
            start_next_scan();
            return found;
        }

        //returns number of new lines between.
        bool start_next_scan_after_sequence(std::string_view sequence) {
            bool found = consume_until_current_sequence_matches(sequence);
            start_next_scan();
            return found;
        }

        [[nodiscard]]
        bool at_end() const {
            return current_char_index >= source_code.size();
        }

        [[nodiscard]]
        std::size_t size() const {
            return (current_char_index + 1) - start_char_index;
        }

        [[nodiscard]]
        std::string_view create_lexeme_view() const {
            return source_code.substr(start_char_index, size());
        }

        void start_next_scan() {

            start_char_index = current_char_index += 1;
            if (start_char_index >= source_code.size()) {
                start_char_index = source_code.size();
            }
            current_char_index = start_char_index;
        }

        void consume_count_chars(std::size_t n) {
            current_char_index += n;
            line_index += count_matched_chars_between(current_char_index, n,
                                                      '\n');
        }

        void consume_current_char() {
            consume_count_chars(1);
        }

        template<TokenType token_type>
        Token consume_token() {
            Token token = create_token<token_type>(*this);
            start_next_scan();
            return token;
        }
        Token consume_token(TokenType token_type) {
            Token token = create_token(token_type, *this);
            start_next_scan();
            return token;
        }

        ScanError create_error(std::string_view error_string) {
            return ScanError{source_code, start_char_index, current_char_index,
                             line_index, std::string{error_string}};
        }

        bool consume_decimal_literal() {
            return consume_while_next_char_matches(
                    [](char previous_char, char current_char) -> bool {
                        return std::isdigit(current_char) ||
                               (current_char == '_' &&
                                std::isdigit(previous_char));
                    });
        }

        bool consume_hexidecimal_literal() {
            return consume_while_next_char_matches(
                    [](char previous_char, char current_char) -> bool {
                        return std::isxdigit(current_char) ||
                               (current_char == '_' &&
                                std::isxdigit(previous_char));
                    });
        }

        bool consume_octal_literal() {
            return consume_while_next_char_matches(
                    [](char previous_char, char current_char) -> bool {
                        return is_octal(current_char) ||
                               (current_char == '_' && is_octal(previous_char));
                    });
        }

        bool consume_binary_literal() {
            return consume_while_next_char_matches(
                    [](char previous_char, char current_char) -> bool {
                        return is_binary(current_char) ||
                               (current_char == '_' &&
                                is_binary(previous_char));
                    });
        }

        bool consume_if_float_exponent() {
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
            if (!consume_decimal_literal()) {
                current_char_index = previous_char_index;
                return false;
            }
            return true;
        }

        bool consume_literal_body() {
            return consume_while_next_char_matches(is_valid_literal_body_char);
        }
    };


    template<typename T = std::monostate>
    Token create_token(TokenType token_type, const ScanState &scan_state) {
        auto lexeme = scan_state.create_lexeme_view();
        if constexpr (std::is_same_v<T, std::monostate>) {
            return {token_type, lexeme, {},
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, std::string>) {
            //TODO strip escape characters!
            return {token_type, lexeme, std::string(trim(lexeme, 1)),
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, double>) {
            //TODO stod could fail? how would we propogate such errors here?
            std::string stripped = remove_char(lexeme, '_');

            //TODO we need to insert a dot here for C++ to properly parse this...
            try {
                double value = std::stod(std::string(stripped));
                return {token_type, lexeme, value,
                        scan_state.line_index};
            }catch (std::out_of_range& /*e*/){
                if(!stripped.empty()){
                    if(stripped[0] == '-'){
                        return {token_type, lexeme, -std::numeric_limits<double>::infinity(),
                                scan_state.line_index};
                    }
                }
                return {token_type, lexeme, std::numeric_limits<double>::infinity(),
                        scan_state.line_index};
            }
        } else if constexpr (std::is_same_v<T, std::uint64_t>) {
            std::string stripped = remove_char(lexeme, '_');
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
            return {token_type, lexeme, value,
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            std::string stripped = remove_char(lexeme, '_');
            std::int64_t value = std::stoll(std::string(stripped));
            return {token_type, lexeme, value,
                    scan_state.line_index};
        } else if constexpr (std::is_same_v<T, bool>) {
            //TODO prepare for failure of programmer here? should only ever be true or false.
            bool value = std::string(lexeme) == "true";
            return {token_type, lexeme, value,
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


    ScanResult scan(std::string_view source_code) {
        ScanState scan_state(source_code);
        std::vector<Token> tokens;
        std::vector<ScanError> errors;
        while (!scan_state.at_end()) {
            auto last_scan_state = scan_state;
            switch (scan_state.current_char()) {
                case '(' : {
                    tokens.push_back(scan_state.consume_token<TokenType::LeftParenthesis>());
                    break;
                }
                case ')' : {
                    tokens.push_back(scan_state.consume_token<TokenType::RightParenthesis>());
                    break;
                }
                case '{' : {
                    tokens.push_back(scan_state.consume_token<TokenType::LeftCurlyBrace>());
                    break;
                }
                case '}' : {
                    tokens.push_back(scan_state.consume_token<TokenType::RightCurlyBrace>());
                    break;
                }
                case '[' : {
                    tokens.push_back(scan_state.consume_token<TokenType::LeftSquareBracket>());
                    break;
                }
                case ']' : {
                    tokens.push_back(scan_state.consume_token<TokenType::RightSquareBracket>());
                    break;
                }
                case ',' : {
                    tokens.push_back(scan_state.consume_token<TokenType::Comma>());
                    break;
                }
                case '+' : {
                    tokens.push_back(scan_state.consume_token<TokenType::Plus>());
                    break;
                }
                case ';' : {
                    tokens.push_back(
                            scan_state.consume_token<TokenType::Semicolon>());
                    break;
                }
                case '*' : {
                    tokens.push_back(scan_state.consume_token<TokenType::Star>());
                    break;
                }
                case '?' : {
                    tokens.push_back(
                            scan_state.consume_token<TokenType::QuestionMark>());
                    break;
                }
                case '!' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(
                                scan_state.consume_token<TokenType::NotEquals>());
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::ExclamationPoint>());
                    }
                    break;
                }
                case '=' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(scan_state.consume_token<TokenType::DoubleEquals>());
                    } else if (scan_state.consume_if_next_char_matches('>')) {
                        tokens.push_back(scan_state.consume_token<TokenType::LeftMatchArrow>());
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::Equals>());
                    }
                    break;
                }
                case '-' : {
                    if (scan_state.consume_if_next_char_matches('>')) {
                        tokens.push_back(
                                scan_state.consume_token<TokenType::LeftArrow>());
                    } else if (scan_state.next_char_matches(is_digit)) {
                        scan_state.consume_decimal_literal();
                        if (scan_state.next_char_matches('.')) {
                            if (scan_state.consume_decimal_literal()) {
                                scan_state.consume_if_float_exponent();
                                tokens.push_back(
                                        scan_state.consume_token<TokenType::DecimalFloatingPointLiteral>());
                            } else {
                                scan_state.create_error(
                                        "no decimal following . in floating point");
                                scan_state.start_next_scan();
                            }
                        } else if (scan_state.consume_if_float_exponent()) {
                            tokens.push_back(scan_state.consume_token<TokenType::DecimalFloatingPointLiteral>());
                        } else {
                            tokens.push_back(
                                    scan_state.consume_token<TokenType::DecimalIntegerLiteral>());
                        }

                    } else {
                        tokens.push_back(
                                scan_state.consume_token<TokenType::Minus>());
                    }
                    break;
                }
                case '.' : {
                    if (scan_state.consume_if_next_char_matches('.')) {
                        tokens.push_back(
                                scan_state.consume_token<TokenType::Elipses>());
                    } else {
                        tokens.push_back(
                                scan_state.consume_token<TokenType::Dot>());
                    }
                    break;
                }
                case ':' : {
                    if (scan_state.consume_if_next_char_matches(':')) {
                        tokens.push_back(scan_state.consume_token<TokenType::DoubleColon>());
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::Colon>());
                    }
                    break;
                }
                case '<' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(scan_state.consume_token<TokenType::GreaterThanEquals>());
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::GreaterThan>());
                    }
                    break;
                }
                case '>' : {
                    if (scan_state.consume_if_next_char_matches('=')) {
                        tokens.push_back(scan_state.consume_token<TokenType::LessThanEquals>());
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::LessThan>());
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
                            scan_state.create_error(
                                    "/* sequence not terminated");
                            scan_state = last_scan_state;
                            scan_state.start_next_scan(); // continue to try to parse.
                        }
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::ForwardSlash>());
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
                        scan_state.create_error("\" sequence not terminated");
                        scan_state = last_scan_state;
                        scan_state.start_next_scan(); // continue to try to parse.
                    } else {
                        tokens.push_back(scan_state.consume_token<TokenType::String>());
                    }
                    break;
                }

                case '0': {
                    if (scan_state.consume_if_next_char_matches('x')) {
                        if (scan_state.consume_hexidecimal_literal()) {
                            tokens.push_back(
                                    scan_state.consume_token<TokenType::HexidecimalLiteral>());
                        } else {
                            scan_state.create_error(
                                    "Started hexidecimal literal with out body [0x...]");
                            scan_state.start_next_scan();
                        }
                        break;
                    } else if (scan_state.consume_if_next_char_matches('o')) {
                        if (scan_state.consume_octal_literal()) {
                            tokens.push_back(scan_state.consume_token<TokenType::OctalLiteral>());
                        } else {
                            scan_state.create_error(
                                    "Started octal literal with out body [0o...]");
                            scan_state.start_next_scan();
                        }
                        break;
                    } else if (scan_state.consume_if_next_char_matches('b')) {
                        if (scan_state.consume_binary_literal()) {
                            tokens.push_back(scan_state.consume_token<TokenType::BinaryLiteral>());
                        } else {
                            scan_state.create_error(
                                    "Started binary literal with out body [0b...]");
                            scan_state.start_next_scan();

                        }
                        break;
                    }
                    //fallthrough on purpose.
                }
                default : {
                    if (std::isdigit(scan_state.current_char())) {
                        scan_state.consume_decimal_literal();
                        if (scan_state.consume_if_next_char_matches('.')) {
                            if (scan_state.consume_decimal_literal()) {
                                scan_state.consume_if_float_exponent();
                                tokens.push_back(scan_state.consume_token<TokenType::DecimalFloatingPointLiteral>());
                            } else {
                                scan_state.create_error(
                                        "no decimal following . in floating point");
                                scan_state.start_next_scan();
                            }
                        } else if (scan_state.consume_if_float_exponent()) {
                            tokens.push_back(scan_state.consume_token<TokenType::DecimalFloatingPointLiteral>());
                        } else {
                            tokens.push_back(scan_state.consume_token<TokenType::DecimalIntegerLiteral>());
                        }
                    } else if (is_valid_literal_start_char(
                            scan_state.current_char())) {
                        scan_state.consume_literal_body();
                        if (auto keyword_result = keyword_map.get(
                                    scan_state.create_lexeme_view()); keyword_result.has_value()) {
                            tokens.push_back(scan_state.consume_token(keyword_result.value()));
                        }else{
                            tokens.push_back(scan_state.consume_token<TokenType::Identifier>());
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
        tokens.push_back(Token{TokenType::EndOfFile, "\0", {}, scan_state.line_index});
        return ScanResult{tokens, errors};
    }
}