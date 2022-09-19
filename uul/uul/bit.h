//
// Created by Shae Bolt on 9/6/2022.
//

#ifndef VULKANWORKSPACE_BIT_H
#define VULKANWORKSPACE_BIT_H

#include <cstdint>
#include <concepts>
#include <limits>
#include <bit>
#include <array>

namespace uul {


    [[nodiscard]]
    constexpr bool
    bit_get(std::unsigned_integral auto uint, std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint &
                                           (static_cast<decltype(uint)>(1)
                                                   << bit_index)) !=
               static_cast<decltype(uint)>(0);
    }

    [[nodiscard]]
    constexpr std::unsigned_integral

    auto bit_set(
            std::unsigned_integral auto uint, std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint |
                                           (static_cast<decltype(uint)>(1)
                                                   << bit_index));
    }

    [[nodiscard]]
    constexpr std::unsigned_integral

    auto bit_clear(std::unsigned_integral auto uint,
                   std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint &
                                           ~(static_cast<decltype(uint)>(1)
                                                   << bit_index));
    }

    [[nodiscard]]
    constexpr std::unsigned_integral

    auto bit_assign(std::unsigned_integral auto uint,
                    std::uint8_t bit_index, bool bit_value) noexcept {
        return static_cast<decltype(uint)>(
                (uint & ~(static_cast<decltype(uint)>(1) << bit_index)) |
                (static_cast<decltype(uint)>(bit_value ? 1 : 0) << bit_index));
    }

    [[nodiscard]]
    constexpr std::unsigned_integral

    auto bit_toggle(std::unsigned_integral auto uint,
                    std::uint8_t bit_index) noexcept {
        return static_cast<decltype(uint)>(uint ^
                                           (static_cast<decltype(uint)>(1)
                                                   << bit_index));
    }

    template<std::unsigned_integral UnsignedT>
    [[nodiscard]]
    constexpr UnsignedT bit_fill() noexcept{
        //inverts empty bits to all be filled.
        constexpr UnsignedT filled_bit_mask = ~static_cast<UnsignedT>(0);
        return filled_bit_mask;
    }


    template<std::unsigned_integral UnsignedT>
    [[nodiscard]]
    constexpr UnsignedT bit_fill_r(UnsignedT n) noexcept {
        constexpr auto type_bit_width = std::numeric_limits<UnsignedT>::digits;
        constexpr UnsignedT full_input_mask = uul::bit_fill<UnsignedT>(); //0x111...
        UnsignedT zero_shift_count = (type_bit_width - n);
        //shifting zeros in from the left, which leaves the right filled in with n bits
        UnsignedT filled = full_input_mask >> zero_shift_count;
        return filled;
    }

    template<std::unsigned_integral UnsignedT>
    [[nodiscard]]
    constexpr UnsignedT bit_fill_l(UnsignedT n) noexcept {
        constexpr UnsignedT type_bit_width = std::numeric_limits<UnsignedT>::digits;
        constexpr UnsignedT full_input_mask = uul::bit_fill<UnsignedT>(); //0x111...
        UnsignedT zero_shift_count = (type_bit_width - n);
        //shifting zeros in from the right, which leaves the left filled in with n bits
        UnsignedT filled = full_input_mask << zero_shift_count;
        return filled;
    }

    template<std::unsigned_integral UnsignedT>
    [[nodiscard]]
    constexpr UnsignedT bit_fill_in(UnsignedT lsb_start, UnsignedT n) noexcept {
        constexpr UnsignedT far_fill = uul::bit_fill_r(lsb_start+n);
        constexpr UnsignedT close_fill = uul::bit_fill_r(lsb_start);
        //everything filled in to the minimum should be cancelled out
        //as values overlap (eg. lsb_start=1, n=2, : 0x0111 ^ 0x0001 == 0x0110)
        constexpr UnsignedT in_fill = far_fill ^ close_fill;
        return in_fill;
    }

    namespace detail {
        template<
                std::unsigned_integral UnsignedT,
                std::size_t bit_width,
                std::size_t bit_stride
        >
        [[nodiscard]]
        constexpr auto generate_interleave_mask_array() noexcept {
            constexpr UnsignedT closest_pow2 = std::bit_ceil(bit_width);
            constexpr std::size_t iteration_count = std::countr_zero(
                    closest_pow2);
            constexpr UnsignedT initial_shift_mask = uul::bit_fill_r(closest_pow2);
            constexpr UnsignedT output_bit_count = bit_width * bit_stride;
            constexpr UnsignedT output_mask = uul::bit_fill_r(output_bit_count);
            std::array<UnsignedT, iteration_count> shift_mask_array;
            UnsignedT shift_mask = initial_shift_mask;
            for (std::size_t i = 0; i < shift_mask_array.size(); ++i) {
                UnsignedT bit_count_to_shift =
                        (1 << (shift_mask_array.size() - i)) / 2;
                // remove half the filled bits
                shift_mask &= (shift_mask >> bit_count_to_shift);
                // shift over new bits and combine with old.
                shift_mask |= shift_mask << (bit_count_to_shift * bit_stride);
                shift_mask_array[i] = shift_mask & output_mask;
            }
            return shift_mask_array;
        }

        template<
                std::unsigned_integral UnsignedT,
                std::size_t bit_width,
                std::size_t bit_stride
        >
        [[nodiscard]]
        constexpr auto generate_unravel_mask_array() noexcept {
            constexpr UnsignedT closest_pow2 = std::bit_ceil(bit_width);
            constexpr std::size_t iteration_count = std::countr_zero(
                    closest_pow2);
            constexpr UnsignedT initial_shift_mask = uul::bit_fill_r(closest_pow2);
            constexpr UnsignedT output_bit_count = bit_width * bit_stride;
            constexpr UnsignedT output_mask = uul::bit_fill_r(output_bit_count);
            std::array<UnsignedT, iteration_count> shift_mask_array;
            UnsignedT shift_mask = initial_shift_mask;
            for (std::size_t i = 0; i < shift_mask_array.size(); ++i) {
                UnsignedT bit_count_to_shift =
                        (1 << (shift_mask_array.size() - i)) / 2;
                // remove half the filled bits
                shift_mask &= (shift_mask >> bit_count_to_shift);
                // shift over new bits and combine with old.
                shift_mask |= shift_mask << (bit_count_to_shift * bit_stride);
                auto reverse_index = (shift_mask_array.size() - 1) - i;
                shift_mask_array[reverse_index] = shift_mask & output_mask;
            }
            return shift_mask_array;
        }
    }

    template<
            std::unsigned_integral UnsignedT,
            std::unsigned_integral UnsignedU,
            std::size_t input_bit_width = std::numeric_limits<UnsignedU>::digits,
            std::size_t bit_stride = std::min(std::numeric_limits<UnsignedU>::digits / input_bit_width)
            >
    [[nodiscard]]
    UnsignedT bit_interlace(UnsignedU input) noexcept {
        static constexpr auto output_type_bit_width = std::numeric_limits<UnsignedT>::digits;
        static constexpr auto input_type_bit_width = std::numeric_limits<UnsignedU>::digits;
        static_assert(input_bit_width <= input_type_bit_width,
                      "Cannot request more bits be interleaved than exist in input type");
        static_assert((input_bit_width * bit_stride) <= output_type_bit_width,
                      "Cannot have a larger stride and bitwidth that combined exceed maximum size of output type");
        static constexpr UnsignedU input_mask = uul::bit_fill_r(static_cast<UnsignedU>(input_bit_width));
        static constexpr UnsignedT closest_pow2 = std::bit_ceil(input_bit_width);
        static constexpr std::size_t iteration_count = std::countr_zero(
                closest_pow2);
        static constexpr auto shift_mask_array = detail::generate_interleave_mask_array<UnsignedT, input_bit_width, bit_stride>();
        static constexpr UnsignedT initial_input_shift = (bit_stride > 0) ? ((bit_stride - 1) * (closest_pow2/2)) : 0;
        UnsignedT input_shift = initial_input_shift;
        UnsignedT working_input = static_cast<UnsignedT>(input & input_mask);
        for (std::size_t i = 0; i < iteration_count; ++i) {
            working_input = (working_input | working_input << input_shift) &
                            shift_mask_array[i];
            input_shift /= 2;
        }
        return working_input;
    }



    template<
            std::unsigned_integral UnsignedT,
            std::unsigned_integral UnsignedU,
            std::size_t input_bit_width = std::numeric_limits<UnsignedU>::digits,
            std::size_t bit_stride = std::min(std::numeric_limits<UnsignedU>::digits / input_bit_width)
    >
    [[nodiscard]]
    UnsignedU bit_de_interlace(UnsignedT output) noexcept {
        static constexpr auto output_type_bit_width = std::numeric_limits<UnsignedT>::digits;
        static constexpr auto input_type_bit_width = std::numeric_limits<UnsignedU>::digits;
        static_assert(input_bit_width <= input_type_bit_width,
                      "Cannot request more bits be unraveled than exist in input type");
        static_assert((input_bit_width * bit_stride) <= output_type_bit_width,
                      "Cannot have a larger stride and bitwidth that combined exceed maximum size of output type");
        static constexpr UnsignedU input_mask = uul::bit_fill_r(static_cast<UnsignedU>(input_bit_width));
        static constexpr UnsignedT closest_pow2 = std::bit_ceil(input_bit_width);
        static constexpr std::size_t iteration_count = std::countr_zero(
                closest_pow2);
        static constexpr auto shift_mask_array = detail::generate_unravel_mask_array<UnsignedT, input_bit_width, bit_stride>();
        static constexpr UnsignedT initial_input_shift = (bit_stride > 0) ? ((bit_stride - 1) * (closest_pow2/2)) : 0;
        static constexpr UnsignedT initial_output_shift = initial_input_shift >> (iteration_count - 1);
        UnsignedT output_shift = initial_output_shift;
        UnsignedT working_input = static_cast<UnsignedT>(output & shift_mask_array[0]);

        for (std::size_t i = 1; i < iteration_count - 1; ++i) {
            working_input = (working_input | working_input >> output_shift) &
                            shift_mask_array[i];
            output_shift *= 2;
        }
        working_input = (working_input | working_input >> output_shift) & static_cast<UnsignedT>(input_mask);
        return static_cast<UnsignedU>(working_input);
    }
    template<std::unsigned_integral UnsignedT>
    [[nodiscard]]
    constexpr UnsignedT bit_grey_encode(UnsignedT value) noexcept{
        //casts around everything, because uint8_t and any op with uint8_t auto casts to int
        return static_cast<UnsignedT>(static_cast<UnsignedT>(value) ^ (static_cast<UnsignedT>(value) >> static_cast<UnsignedT>(value)));
    }

    template<std::unsigned_integral UnsignedT>
    [[nodiscard]]
    constexpr UnsignedT bit_grey_decode(UnsignedT value){
        constexpr UnsignedT bit_width = std::numeric_limits<UnsignedT>::digits;
        constexpr UnsignedT closest_pow2 = std::bit_ceil(bit_width);
        constexpr std::size_t iteration_count = std::countr_zero(closest_pow2);
        constexpr UnsignedT initial_bit_shift = bit_width / 2;
        for(std::size_t i = 0; i < iteration_count; ++i){
            value ^= value >> initial_bit_shift;
            initial_bit_shift /= 2;
        }
        return value;
    }

}
#endif //VULKANWORKSPACE_BIT_H
