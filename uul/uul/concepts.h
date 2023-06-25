//
// Created by Bolt on 5/6/2023.
//

#ifndef VULKANWORKSPACE_CONCEPTS_H
#define VULKANWORKSPACE_CONCEPTS_H
#include <type_traits>
#include <variant>
namespace uul{
    template <typename T>
    concept ContiguousContainer = requires (T t) {
        t.data();
        t.size();
    };
    template<typename T>
    concept NestedContiguousContainer = ContiguousContainer<T> && ContiguousContainer<typename T::value_type>;

    template<typename T>
    concept Enumeration = std::is_enum_v<T>;

    //https://stackoverflow.com/a/57134598/
    template<typename T> struct is_variant : std::false_type {};

    template<typename ...Args>
    struct is_variant<std::variant<Args...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_variant_v=is_variant<T>::value;

    template<typename T>
    concept Variant = uul::is_variant_v<T>;
}
#endif //VULKANWORKSPACE_CONCEPTS_H
