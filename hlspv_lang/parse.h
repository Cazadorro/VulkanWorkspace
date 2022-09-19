//
// Created by Shae Bolt on 9/19/2022.
//

#ifndef VULKANWORKSPACE_PARSE_H
#define VULKANWORKSPACE_PARSE_H
#include "token.h"
#include "expression.h"
#include <vector>
#include <cstdint>
#include <span>
namespace hlspv{
    struct ParseError{

    };

    struct ParseResult{
        std::vector<Expression> expressions;
        std::vector<ParseError> errors;
    };
    ParseResult parse(std::span<const Token> tokens);
}
#endif //VULKANWORKSPACE_PARSE_H
