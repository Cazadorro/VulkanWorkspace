//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_SCAN_H
#define VULKANWORKSPACE_SCAN_H
#include "token.h"
#include "scan_state.h"
#include <fmt/core.h>
#include <vector>
#include <string_view>
#include <cstdint>

namespace hlspv {

    ScanResult<Token> scan(std::string_view source_code);
}
#endif //VULKANWORKSPACE_SCAN_H
