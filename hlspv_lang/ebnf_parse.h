//
// Created by Shae Bolt on 9/27/2022.
//

#ifndef VULKANWORKSPACE_EBNF_PARSE_H
#define VULKANWORKSPACE_EBNF_PARSE_H
#include "ebnf_token.h"
#include "ebnf_expr.h"
#include "parse_state.h"
#include <tl/expected.hpp>
#include <vector>
#include <span>
namespace hlspv::ebnf{

    tl::expected<Expression, ParseError<EbnfToken>> parse(std::span<const EbnfToken> tokens);
}
#endif //VULKANWORKSPACE_EBNF_PARSE_H
