//
// Created by Shae Bolt on 9/27/2022.
//

#ifndef VULKANWORKSPACE_EBNF_SCAN_H
#define VULKANWORKSPACE_EBNF_SCAN_H
#include "ebnf_token.h"
#include "scan_state.h"
#include <string_view>
namespace hlspv::ebnf{
    ScanResult<EbnfToken> scan(std::string_view source_code);
}
#endif //VULKANWORKSPACE_EBNF_SCAN_H
