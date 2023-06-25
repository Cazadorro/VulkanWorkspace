//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_LANG_SCAN_H
#define VULKANWORKSPACE_LANG_SCAN_H
#include "token.h"
#include "scan_state.h"
#include <fmt/core.h>
#include <vector>
#include <string_view>
#include <cstdint>

namespace hlspv::lang {
    using TokenLiteralVariant = std::variant<std::monostate, std::string, std::uint64_t, std::int64_t, double, bool>;
    using LangToken = Token<TokenType, TokenLiteralVariant>;
    ScanResult<LangToken> scan(std::string_view source_code);
}
#endif //VULKANWORKSPACE_LANG_SCAN_H
