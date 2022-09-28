//
// Created by Shae Bolt on 9/25/2022.
//

#ifndef VULKANWORKSPACE_UTILS_H
#define VULKANWORKSPACE_UTILS_H
#include <string_view>
#include <string>
namespace hlspv{
    //https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
    [[nodiscard]]
    std::string trim_left(std::string_view string, std::size_t n);

    [[nodiscard]]
    std::string trim_right(std::string_view string, std::size_t n);

    [[nodiscard]]
    std::string
    trim(std::string_view string, std::size_t left_n, std::size_t right_n);

    [[nodiscard]]
    std::string trim(std::string_view string, std::size_t n);
    [[nodiscard]]
    std::string remove_char(std::string_view string, char c);
    [[nodiscard]]
    bool is_digit(char c);

    [[nodiscard]]
    bool is_octal(char c);
    [[nodiscard]]
    bool is_binary(char c);
    [[nodiscard]]
    bool is_valid_identifier_start_char(char c);

    [[nodiscard]]
    bool is_valid_identifier_body_char(char c);

}
#endif //VULKANWORKSPACE_UTILS_H
