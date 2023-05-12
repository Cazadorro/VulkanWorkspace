//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_scan.h"
#include "scan_state.h"
#include "ebnf_token.h"
#include "ebnf_token_type.h"
#include "lexeme_view.h"
#include "utils.h"
namespace hlspv{
    EbnfToken
    create_token(EbnfTokenType token_type, const ScanState &scan_state) {
        auto lexeme_view = scan_state.create_lexeme_view();
        switch (token_type) {
            using enum EbnfTokenType;
            case TerminalString:
            case TerminalUserCode: {
                return {token_type, lexeme_view, std::string(trim(lexeme_view.to_sv(), 1)),
                        scan_state.line_index};
            }
            case UnsignedInteger: {
                return {token_type, lexeme_view, static_cast<std::uint64_t>(std::stoi(std::string(lexeme_view.to_sv()))),
                        scan_state.line_index};
            }
            default: {
                return {token_type, lexeme_view, {}, scan_state.line_index};
            }
        }
    }
}
namespace hlspv::ebnf{


    ScanResult<EbnfToken> scan(std::string_view source_code) {
        ScanState scan_state(source_code);
        std::vector<EbnfToken> tokens;
        std::vector<ScanError> errors;
        while (!scan_state.at_end()) {
            auto last_scan_state = scan_state;
            switch (scan_state.current_char()) {
                case ':' : {
                    tokens.push_back(scan_state.consume_token(
                            EbnfTokenType::Colon));
                    break;
                }
                case ';' : {
                    tokens.push_back(scan_state.consume_token(
                            EbnfTokenType::SemiColon));
                    break;
                }
                case '[' : {
                    tokens.push_back(scan_state.consume_token(
                            EbnfTokenType::LeftSquareBracket));
                    break;
                }
                case ']' : {
                    tokens.push_back(scan_state.consume_token(
                            EbnfTokenType::RightSquareBracket));
                    break;
                }
                case '(' : {
                    tokens.push_back(scan_state.consume_token(
                            EbnfTokenType::LeftParenthesis));
                    break;
                }
                case ')' : {
                    tokens.push_back(scan_state.consume_token(
                            EbnfTokenType::RightParenthesis));
                    break;
                }
                case ',' : {
                    tokens.push_back(
                            scan_state.consume_token(EbnfTokenType::Comma));
                    break;
                }
                case '+' : {
                    tokens.push_back(
                            scan_state.consume_token(EbnfTokenType::Plus));
                    break;
                }
                case '*' : {
                    tokens.push_back(
                            scan_state.consume_token(EbnfTokenType::Star));
                    break;
                }
                case '?' : {
                    tokens.push_back(
                            scan_state.consume_token(
                                    EbnfTokenType::QuestionMark));
                    break;
                }
                case '!' : {
                    tokens.push_back(
                            scan_state.consume_token(
                                    EbnfTokenType::ExclamationPoint));
                    break;
                }
                case '&' : {
                    tokens.push_back(
                            scan_state.consume_token(
                                    EbnfTokenType::Ampersand));
                    break;
                }
                case '|' : {
                    tokens.push_back(
                            scan_state.consume_token(
                                    EbnfTokenType::VerticalLine));
                    break;
                }
                case '=' : {
                    tokens.push_back(
                            scan_state.consume_token(EbnfTokenType::Equals));

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
                        errors.push_back(scan_state.create_error(
                                "invalid token"));
                        scan_state = last_scan_state;
                        scan_state.start_next_scan(); // continue to try to parse.
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
                        errors.push_back(scan_state.create_error(
                                "\" sequence not terminated"));
                        scan_state = last_scan_state;
                        scan_state.start_next_scan(); // continue to try to parse.
                    } else {
                        tokens.push_back(scan_state.consume_token(
                                EbnfTokenType::TerminalString));
                    }
                    break;
                }
                case '\'': {
                    bool finished = scan_state.consume_until_current_char_matches(
                            '\'', '\\');
                    if (!finished) {
                        errors.push_back(scan_state.create_error(
                                "\' sequence not terminated"));
                        scan_state = last_scan_state;
                        scan_state.start_next_scan(); // continue to try to parse.
                    } else {
                        tokens.push_back(scan_state.consume_token(
                                EbnfTokenType::TerminalUserCode));
                    }
                    break;
                }
                default : {
                    if(auto new_scan_state = scan_state.consume_if_unsigned_integer(); new_scan_state.has_value()){
                        scan_state = new_scan_state.value();
                        tokens.push_back(scan_state.consume_token(
                                EbnfTokenType::UnsignedInteger));
                    }
                    if (auto new_scan_state = scan_state.consume_if_identifier(); new_scan_state.has_value()) {
                        scan_state = new_scan_state.value();
                        tokens.push_back(scan_state.consume_token(
                                EbnfTokenType::NonTerminalIdentifier));
                    } else {
                        errors.push_back(
                                scan_state.create_error(
                                        "Unexpected character"));
                        scan_state.start_next_scan();
                    }
                }
            }
        }
        if(!tokens.empty()){
            tokens.push_back(EbnfToken{EbnfTokenType::EndOfFile, tokens.back().lexeme_view(), {}, scan_state.line_index});
        }else{
            tokens.push_back(EbnfToken{EbnfTokenType::EndOfFile, {source_code, 0,0}, {}, scan_state.line_index});
        }
        return {tokens, errors};
    }
}