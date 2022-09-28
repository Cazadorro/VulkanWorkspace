//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_semantics.h"
#include <range/v3/view/enumerate.hpp>
#include <fmt/core.h>

std::unordered_set<std::string>
hlspv::ebnf::calc_symbol_table(const hlspv::ebnf::Expression &expr) {
    std::unordered_set<std::string> symbol_table;
    for (const auto &rule: expr.exprs) {
        symbol_table.insert(rule.non_terminal.lexeme());
    }
    return symbol_table;
}

std::vector<hlspv::ebnf::UnresolvedSymbolError>
hlspv::ebnf::analyze_symbols(std::span<const EbnfToken> tokens,
                             const std::unordered_set<std::string> &symbol_table) {
    std::vector<UnresolvedSymbolError> errors;
    for(const auto [index, token] : tokens | ranges::views::enumerate ){
        if(token.type() == EbnfTokenType::NonTerminalIdentifier){
            if(symbol_table.count(token.lexeme()) == 0){
                errors.emplace_back(tokens.subspan(index,1), fmt::format("can't find {}", token.lexeme()));
            }
        }
    }
    return errors;
}
