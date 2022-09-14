//
// Created by Shae Bolt on 9/6/2022.
//

#ifndef VULKANWORKSPACE_ARRAY_H
#define VULKANWORKSPACE_ARRAY_H
#include <array>
namespace uul{
    template<typename T, std::size_t N>
    constexpr auto make_array(const T& init_value){
        std::array<T,N> array = {};
        for(auto& value : array){
            value = init_value;
        }
        return array;
    }
}
#endif //VULKANWORKSPACE_ARRAY_H
