//
// Created by Bolt on 5/30/2023.
//

#ifndef VULKANWORKSPACE_STRING_H
#define VULKANWORKSPACE_STRING_H
#include <string>
#include <string_view>


namespace uul{
    [[nodiscard]]
    std::string add_to_each_line(std::string_view string, std::string_view prepend, std::string_view append = {});

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
    template<typename T>
    [[nodiscard]]
    T from_string(std::string_view value) = delete;
}
#endif //VULKANWORKSPACE_STRING_H
