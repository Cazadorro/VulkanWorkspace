//
// Created by Shae Bolt on 8/24/2021.
//

#ifndef VULKANWORKSPACE_TUPLE_TO_ARRAY_H
#define VULKANWORKSPACE_TUPLE_TO_ARRAY_H
#include <tuple>
#include <array>
#include <utility>
namespace vul{
    template<typename tuple_t>
    constexpr auto array_from_tuple(tuple_t&& tuple){
        constexpr auto get_array = [](auto&& ... x){ return std::array{std::forward<decltype(x)>(x) ... }; };
        return std::apply(get_array, std::forward<tuple_t>(tuple));
    }
}
#endif //VULKANWORKSPACE_TUPLE_TO_ARRAY_H
