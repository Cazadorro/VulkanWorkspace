//
// Created by Shae Bolt on 9/6/2022.
//

#ifndef VULKANWORKSPACE_BIT_H
#define VULKANWORKSPACE_BIT_H

#include <concepts>

namespace uul {


    [[nodiscard]]
    constexpr bool
    bit_get(std::unsigned_integral auto uint, std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint & (static_cast<decltype(uint)>(1) << bit_index)) != static_cast<decltype(uint)>(0);
    }

    [[nodiscard]]
    constexpr std::unsigned_integral auto bit_set(
            std::unsigned_integral auto uint, std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint | (static_cast<decltype(uint)>(1) << bit_index));
    }

    [[nodiscard]]
    constexpr std::unsigned_integral auto bit_clear(std::unsigned_integral auto uint,
                             std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint & ~(static_cast<decltype(uint)>(1) << bit_index));
    }

    [[nodiscard]]
    constexpr std::unsigned_integral auto bit_assign(std::unsigned_integral auto uint,
                              std::uint8_t bit_index, bool bit_value) noexcept {
        return static_cast<decltype(uint)>((uint & ~(static_cast<decltype(uint)>(1) << bit_index)) | (static_cast<decltype(uint)>(bit_value ? 1 : 0) << bit_index));
    }

    [[nodiscard]]
    constexpr std::unsigned_integral auto bit_toggle(std::unsigned_integral auto uint,
                              std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint ^ (static_cast<decltype(uint)>(1) << bit_index));
    }

}
#endif //VULKANWORKSPACE_BIT_H
