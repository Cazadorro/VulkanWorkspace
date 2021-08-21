//
// Created by Shae Bolt on 8/18/2021.
//

#include "vkassert.h"
#include <iostream>

void vul::detail::handle_assert(const char *message, const char *condition,
                                const char *file_name,
                                const char *function_name,
                                unsigned long long int line_number) {
    std::cerr << "message: \"" << message << "\"\n";
    std::cerr << "condition: " << condition << "\n";
    std::cerr << "file_name: " << file_name << "\n";
    std::cerr << "function_name: " << function_name << "\n";
    std::cerr << "line_number: " << line_number << std::endl;
}
