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

    template<typename T, std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::size_bytes)>> * = nullptr>
    std::size_t size_bytes(const T& t){
        return t.size_bytes();

    }
    template<typename T>
    std::size_t size_bytes(const T& t){
        return t.size() * sizeof(typename T::value_type);
    }
}
#endif //VULKANWORKSPACE_ARRAY_H
