//
// Created by Shae Bolt on 12/1/2022.
//

#ifndef VULKANWORKSPACE_MATH_H
#define VULKANWORKSPACE_MATH_H
#include <concepts>
namespace uul{

    constexpr std::integral auto integer_ceil(std::integral auto numerator, std::integral auto denominator){
        constexpr std::integral auto one = 1;
        return one + ((numerator - one) / denominator);
    }
}
#endif //VULKANWORKSPACE_MATH_H
