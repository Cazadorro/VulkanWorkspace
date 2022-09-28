//
// Created by Shae Bolt on 9/25/2022.
//
#include "utils.h"
namespace hlspv{

    std::string trim_left(std::string_view string, std::size_t n) {
        std::string trimmed_string(string);
        trimmed_string.erase(trimmed_string.begin(),
                             trimmed_string.begin() + n);
        return trimmed_string;
    }

    std::string trim_right(std::string_view string, std::size_t n) {
        std::string trimmed_string(string);
        trimmed_string.erase(trimmed_string.end() - n, trimmed_string.end());
        return trimmed_string;
    }

    std::string hlspv::trim(std::string_view string, std::size_t left_n,
                            std::size_t right_n) {
        auto trimmed_string = trim_left(string, left_n);
        trimmed_string = trim_right(trimmed_string, right_n);
        return trimmed_string;
    }

    std::string hlspv::trim(std::string_view string, std::size_t n) {
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

    bool is_valid_identifier_start_char(char c) {
        return std::isalpha(c) || c == '_';
    }

    bool is_valid_identifier_body_char(char c) {
        return is_valid_identifier_start_char(c) || std::isdigit(c);
    }
}