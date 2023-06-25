//
// Created by Bolt on 5/30/2023.
//

#ifndef VULKANWORKSPACE_VARIANT_H
#define VULKANWORKSPACE_VARIANT_H
#include "uul/concepts.h"
#include <variant>
namespace uul{


    template<typename T, class ...Args>
    std::variant<T(Args...)> wrap_variant_types(std::variant<Args...> const &){}

    template<typename T, uul::Variant variant_T>
    struct wrapped_variant { typedef decltype(wrap_variant_types<T>(std::declval<variant_T>())) type; };

    template<typename T, uul::Variant variant_T>
    using wrapped_variant_t = typename wrapped_variant<T, variant_T>::type;

}
#endif //VULKANWORKSPACE_VARIANT_H
