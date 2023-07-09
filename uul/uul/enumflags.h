//
// Created by Bolt on 5/23/2023.
//

#ifndef VULKANWORKSPACE_ENUMFLAGS_H
#define VULKANWORKSPACE_ENUMFLAGS_H

#include "uul/concepts.h"
#include <bit>

namespace uul {
    template<uul::Enumeration T>
    class EnumFlags {
    public:
        class reference {
            friend class EnumFlags<T>;

        public:
            constexpr ~reference() noexcept = default;

            constexpr reference &operator=(const bool value) noexcept {
                m_parent_flags_ptr->set(m_position, value);
                return *this;
            }

            constexpr reference &operator=(const reference &ref) noexcept { //no need to check self assignment.
                m_parent_flags_ptr->set(m_position, static_cast<bool>(ref));
                return *this;
            }

            constexpr reference &flip() noexcept {
                m_parent_flags_ptr->flip(m_position);
                return *this;
            }

            [[nodiscard]]
            constexpr bool operator~() const noexcept {
                return !m_parent_flags_ptr->get(m_position);
            }

            [[nodiscard]]
            constexpr operator bool() const noexcept { //implicit
                return m_parent_flags_ptr->get(m_position);
            }

        private:
            constexpr reference() noexcept: m_parent_flags_ptr(nullptr), m_position({}) {}

            constexpr reference(EnumFlags<T> &parent_flags, const T position) noexcept: m_parent_flags_ptr(
                    &parent_flags), m_position(position) {}

            EnumFlags<T> *m_parent_flags_ptr;
            T m_position; // position of element in bitset
        };

        using UnderlyingType = std::underlying_type_t<T>;

        [[nodiscard]]
        explicit constexpr operator UnderlyingType() const noexcept {
            return m_bit_mask;
        }

        [[nodiscard]]
        explicit constexpr operator T() const noexcept {
            return static_cast<T>(m_bit_mask);
        }


        constexpr EnumFlags() noexcept: m_bit_mask{} {};

        constexpr EnumFlags(T bit) noexcept: m_bit_mask(static_cast<UnderlyingType>(bit)) {}; //need implicit conversion here for convienence.

        constexpr explicit EnumFlags(UnderlyingType flags) noexcept: m_bit_mask(flags) {};

        [[nodiscard]]
        constexpr bool contains(EnumFlags rhs) const noexcept {
            return (m_bit_mask & rhs.m_bit_mask) == rhs.m_bit_mask;
        }

        [[nodiscard]]
        constexpr UnderlyingType to_underlying() const noexcept {
            return static_cast<UnderlyingType>(*this);
        }

        [[nodiscard]]
        static constexpr EnumFlags full() {
            EnumFlags mask;
            mask.m_bit_mask = (~UnderlyingType{});
            return mask;
        }

        constexpr EnumFlags &operator=(T rhs) noexcept {
            m_bit_mask = static_cast<UnderlyingType>(rhs);
            return *this;
        }

        constexpr EnumFlags &operator&=(EnumFlags rhs) noexcept {
            m_bit_mask &= rhs.m_bit_mask;
            return *this;
        }

        constexpr EnumFlags &operator&=(T rhs) {
            m_bit_mask &= static_cast<UnderlyingType>(rhs);
            return *this;
        }

        constexpr EnumFlags &operator|=(EnumFlags rhs) noexcept {
            m_bit_mask |= rhs.m_bit_mask;
            return *this;
        }

        constexpr EnumFlags &operator|=(T rhs) {
            m_bit_mask |= static_cast<UnderlyingType>(rhs);
            return *this;
        }

        constexpr EnumFlags &operator^=(EnumFlags rhs) noexcept {
            m_bit_mask ^= rhs.m_bit_mask;
            return *this;
        }

        constexpr EnumFlags &operator^=(T rhs) noexcept {
            m_bit_mask ^= static_cast<UnderlyingType>(rhs);
            return *this;
        }


        constexpr EnumFlags &clear() {
            m_bit_mask = UnderlyingType{}; //0x00...;
            return *this;
        }

        constexpr EnumFlags &clear(T bit) {
            m_bit_mask &= ~static_cast<UnderlyingType>(bit);
            return *this;
        }

        constexpr EnumFlags &set() noexcept {
            m_bit_mask = ~0; //0xFFF...;
            return *this;
        }

        constexpr EnumFlags &set(T bit) noexcept {
            m_bit_mask |= static_cast<UnderlyingType>(bit);
            return *this;
        }

        constexpr EnumFlags &set(T bit, bool state) noexcept {
            if (state) {
                return set(bit);
            } else {
                return clear(bit);
            }
        }

        [[nodiscard]]
        constexpr bool get(T bit) const noexcept {
            return (m_bit_mask & static_cast<UnderlyingType>(bit)) != 0;
        }

        //to match std::bitset
        constexpr EnumFlags &reset() noexcept {
            return clear();
        }

        //to match std::bitset
        constexpr EnumFlags &reset(T bit) noexcept {
            return clear(bit);
        }

        //to match std::bitset
        [[nodiscard]]
        constexpr bool test(T bit) const noexcept {
            return get(bit);
        }

        constexpr EnumFlags &flip() noexcept {
            m_bit_mask = ~m_bit_mask;
            return *this;
        }

        constexpr EnumFlags &flip(T bit) noexcept {
            m_bit_mask ^= bit;
            return *this;
        }


        [[nodiscard]]
        constexpr bool operator[](T bit) const noexcept {
            return this->get(bit);
        }

        [[nodiscard]]
        constexpr reference operator[](T bit) noexcept {
            return reference(*this, bit);
        }

        [[nodiscard]]
        constexpr bool all() const noexcept {
            return m_bit_mask == EnumFlags::full();
        }

        [[nodiscard]]
        constexpr bool any() const noexcept {
            return m_bit_mask != UnderlyingType{};
        }

        [[nodiscard]]
        constexpr bool none() const noexcept {
            return m_bit_mask == UnderlyingType{};
        }

        [[nodiscard]]
        constexpr bool empty() const noexcept {
            return none();
        }

        [[nodiscard]]
        constexpr bool count() const noexcept{
            return std::popcount(m_bit_mask);
        }

        //No need for shift operators.
    private:
        UnderlyingType m_bit_mask;
    };

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr bool operator==(EnumFlags<T> lhs, EnumFlags<T> rhs) noexcept {
        return lhs.to_underlying() == rhs.to_underlying();
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr bool operator==(EnumFlags<T> lhs, T rhs) noexcept {
        return lhs.to_underlying() == static_cast<EnumFlags<T>::UnderlyingType>(rhs);
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr bool operator==(T lhs, EnumFlags<T> rhs) noexcept {
        return  static_cast<EnumFlags<T>::UnderlyingType>(lhs) == rhs.to_underlying();
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr bool operator!=(EnumFlags<T> lhs, EnumFlags<T> rhs) noexcept {
        return lhs.to_underlying() != rhs.to_underlying();
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr bool operator!=(EnumFlags<T> lhs, T rhs) noexcept {
        return lhs.to_underlying() != static_cast<EnumFlags<T>::UnderlyingType>(rhs);
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr bool operator!=(T lhs, EnumFlags<T> rhs) noexcept {
        return  static_cast<EnumFlags<T>::UnderlyingType>(lhs) != rhs.to_underlying();
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator&(EnumFlags<T> lhs, EnumFlags<T> rhs) noexcept {
        lhs &= rhs;
        return lhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator&(EnumFlags<T> lhs, T rhs) noexcept {
        lhs &= rhs;
        return lhs;
    }
    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator&(T lhs, EnumFlags<T> rhs) noexcept {
        return EnumFlags<T>(lhs) & rhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator|(EnumFlags<T> lhs, EnumFlags<T> rhs) noexcept {
        lhs |= rhs;
        return lhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator|(EnumFlags<T> lhs, T rhs) noexcept {
        lhs |= rhs;
        return lhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator|(T lhs, EnumFlags<T> rhs) noexcept {
        return EnumFlags<T>(lhs) | rhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator^(EnumFlags<T> lhs, EnumFlags<T> rhs) noexcept {
        lhs ^= rhs;
        return lhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator^(EnumFlags<T> lhs, T rhs) noexcept {
        lhs ^= rhs;
        return lhs;
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator^(T lhs, EnumFlags<T> rhs) noexcept {
        return EnumFlags<T>(lhs) ^ rhs;
    }


    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator~(EnumFlags<T> rhs) noexcept {
        return EnumFlags<T>(~rhs.to_underlying());
    }

    template<uul::Enumeration T>
    [[nodiscard]]
    constexpr EnumFlags<T> operator~(T rhs) noexcept {
        return EnumFlags<T>(~static_cast<EnumFlags<T>::UnderlyingType>(rhs));
    }
}
#endif //VULKANWORKSPACE_ENUMFLAGS_H
