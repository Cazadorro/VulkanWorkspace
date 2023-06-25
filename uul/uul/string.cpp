//
// Created by Bolt on 5/30/2023.
//

#include "uul/string.h"
#include <fmt/core.h>
#include <sstream>

std::string
uul::add_to_each_line(std::string_view string, std::string_view prepend, std::string_view append) {
    std::istringstream ss((std::string(string)));
    std::string line;
    std::string return_string;
    while (std::getline(ss, line)) {
        return_string += fmt::format("{}{}{}\n", prepend,
                                     line, append);
    }
    return return_string;
}

std::string uul::trim_left(std::string_view string, std::size_t n) {
    std::string trimmed_string(string);
    trimmed_string.erase(trimmed_string.begin(),
                         trimmed_string.begin() + n);
    return trimmed_string;
}

std::string uul::trim_right(std::string_view string, std::size_t n) {
    std::string trimmed_string(string);
    trimmed_string.erase(trimmed_string.end() - n, trimmed_string.end());
    return trimmed_string;
}

std::string uul::trim(std::string_view string, std::size_t left_n,
                        std::size_t right_n) {
    auto trimmed_string = trim_left(string, left_n);
    trimmed_string = trim_right(trimmed_string, right_n);
    return trimmed_string;
}

std::string uul::trim(std::string_view string, std::size_t n) {
    return trim(string, n, n);
}

std::string uul::remove_char(std::string_view string, char c) {
    std::string trimmed_string(string);
    trimmed_string.erase(
            std::remove(trimmed_string.begin(), trimmed_string.end(), c),
            trimmed_string.end());
    return trimmed_string;
}
