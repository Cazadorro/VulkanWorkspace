//
// Created by Bolt on 5/30/2023.
//

#ifndef VULKANWORKSPACE_ENUMTRAITS_H
#define VULKANWORKSPACE_ENUMTRAITS_H
#include "uul/concepts.h"
#include <ranges>
#include <utility>
namespace uul{
    constexpr inline auto enum_range = [](Enumeration auto front, Enumeration auto back) {
        using T = decltype(front);
        using U = decltype(front);
        return std::views::iota(static_cast<std::underlying_type_t<T>>(front), static_cast<std::underlying_type_t<U>>(back) + 1)
               | std::views::transform([](auto e) { return decltype(front)(e); });
    };



    template<Enumeration T>
    [[nodiscard]]
    constexpr T enum_max() noexcept = delete;
    template<Enumeration T>
    [[nodiscard]]
    constexpr T enum_min() noexcept = delete;

    template<Enumeration T>
    [[nodiscard]]
    constexpr std::underlying_type_t<T> underlying_enum_max() noexcept{
        return static_cast<std::underlying_type_t<T>>(enum_max<T>());
    }
    template<Enumeration T>
    [[nodiscard]]
    constexpr std::underlying_type_t<T> underlying_enum_min() noexcept{
        return static_cast<std::underlying_type_t<T>>(enum_min<T>());
    }
    template<typename T>
    constexpr std::underlying_type_t<T> to_underlying(T value) noexcept{
#ifdef __cpp_lib_to_underlying
        return std::to_underlying(value);
#else
        return static_cast<std::underlying_type_t<T>>(value);
#endif
    }
}
#endif //VULKANWORKSPACE_ENUMTRAITS_H
