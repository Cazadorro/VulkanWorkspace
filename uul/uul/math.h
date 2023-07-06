//
// Created by Shae Bolt on 12/1/2022.
//

#ifndef VULKANWORKSPACE_MATH_H
#define VULKANWORKSPACE_MATH_H
#include <concepts>
namespace uul{

    [[nodiscard]]
    constexpr std::integral auto integer_ceil(std::integral auto numerator, std::integral auto denominator){
        constexpr std::integral auto one = 1;
        return one + ((numerator - one) / denominator);
    }

    template<typename T, typename U, typename V = std::int32_t>
    [[nodiscard]]
    constexpr T clamp_subtraction(T a, U b, V lower_bound = 0){
        if((a-lower_bound) < b){
            return static_cast<T>(lower_bound);
        }else{
            return static_cast<T>(a - b);
        }
    }

    template<typename T>
    [[nodiscard]]
    constexpr T sign(T value){
        if(value > 0){
            return T{1};
        }
        if(value < 0){
            return T{-1};
        }
        if(value == 0){
            return T{0};
        }
    }
}
#endif //VULKANWORKSPACE_MATH_H
