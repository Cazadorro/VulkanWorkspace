//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_SCAN_H
#define VULKANWORKSPACE_SCAN_H
#include "token.h"
#include "fmt/core.h"
#include <vector>
#include <string_view>
#include <cstdint>

namespace hlspv {
    struct ScanError {
        std::string_view source_code;
        std::size_t start_char_index = 0;
        std::size_t current_char_index = 0;
        std::size_t line_index = 0;
        std::string error_string;

        [[nodiscard]]
        std::size_t size() const {
            return (current_char_index + 1) - start_char_index;
        }

        [[nodiscard]]
        std::string_view find_substr() const {
            return source_code.substr(start_char_index, size());
        }

        [[nodiscard]]
        std::string create_info_string() const{
            return fmt::format("at line {}, {}: {}", line_index, error_string,
                               find_substr());
        }
    };
    struct ScanResult{
        std::vector<Token> tokens;
        std::vector<ScanError> errors;
    };
    ScanResult scan(std::string_view source_code);
}
#endif //VULKANWORKSPACE_SCAN_H
