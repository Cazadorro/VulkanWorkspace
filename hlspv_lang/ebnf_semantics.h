//
// Created by Shae Bolt on 9/27/2022.
//

#ifndef VULKANWORKSPACE_EBNF_SEMANTICS_H
#define VULKANWORKSPACE_EBNF_SEMANTICS_H

#include "ebnf_expr.h"
#include "ebnf_token.h"
#include <optional>
#include <vector>
#include <unordered_set>
#include <span>

namespace hlspv::ebnf {

    struct UnresolvedSymbolError {
        std::span<const EbnfToken> token_span;
        std::string message;
    };

    std::unordered_set<std::string> calc_symbol_table(const Expression &expr);

    std::vector<UnresolvedSymbolError>
    analyze_symbols(std::span<const EbnfToken> tokens,
                    const std::unordered_set<std::string> &symbol_table);
}

#endif //VULKANWORKSPACE_EBNF_SEMANTICS_H
