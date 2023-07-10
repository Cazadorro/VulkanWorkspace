//
// Created by Shae Bolt on 8/30/2021.
//

#ifndef VULKANWORKSPACE_TEMPARRAYPROXY_H
#define VULKANWORKSPACE_TEMPARRAYPROXY_H

#include "vul/tempvoidarrayproxy.h"
#include <uul/assert.h>
#include "vul/traitutils.h"
#include "vul/temparrayproxyfwd.h"
#include <span>
#include <gsl/span>
#include <fmt/core.h>
#include <range/v3/view/facade.hpp>

namespace vul {

// : public ranges::view_facade<TempArrayProxy<T>>
    template<typename T>
    class TempArrayProxy{
    public:
        class iterator {
        public:
            using value_type = T;
            using reference = value_type &;
            using pointer = value_type *;
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using iterator_concept = std::contiguous_iterator_tag;

            explicit constexpr iterator(T *iter = nullptr) : m_ptr{iter} {}

            [[nodiscard]]
            constexpr bool operator==(const iterator &other) const noexcept {
                return m_ptr == other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator!=(const iterator &other) const noexcept {
                return m_ptr != other.m_ptr;
            }

            [[nodiscard]]
            constexpr reference operator*() const noexcept {
                return *m_ptr;
            }

            constexpr pointer operator->() const noexcept {
                return m_ptr;
            }

            constexpr iterator &operator++() noexcept {
                ++m_ptr;
                return *this;
            }

            constexpr iterator operator++(int) noexcept {
                iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            constexpr iterator &operator--() noexcept {
                --m_ptr;
                return *this;
            }

            constexpr iterator operator--(int) noexcept {
                iterator tmp(*this);
                --(*this);
                return tmp;
            }

            constexpr iterator &operator+=(const difference_type other) noexcept {
                m_ptr += other;
                return *this;
            }

            constexpr iterator &operator-=(const difference_type other) noexcept {
                m_ptr -= other;
                return *this;
            }

            [[nodiscard]]
            constexpr iterator operator+(const difference_type other) const noexcept {
                return iterator(m_ptr + other);
            }

            [[nodiscard]]
            constexpr iterator operator-(const difference_type other) const noexcept {
                return iterator(m_ptr - other);
            }

            [[nodiscard]]
            constexpr iterator operator+(const iterator &other) const noexcept {
                return iterator(*this + other.m_ptr);
            }

            [[nodiscard]]
            constexpr difference_type operator-(const iterator &other) const noexcept {
                return std::distance(m_ptr, other.m_ptr);
            }

            constexpr reference operator[](std::size_t index) const {
                return m_ptr[index];
            }

            [[nodiscard]]
            constexpr bool operator<(const iterator &other) const noexcept {
                return m_ptr < other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator>(const iterator &other) const noexcept {
                return m_ptr > other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator<=(const iterator &other) const noexcept {
                return m_ptr <= other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator>=(const iterator &other) const noexcept {
                return m_ptr >= other.m_ptr;
            }

        private:
            T *m_ptr;
        };


        class const_iterator {
        public:
            using value_type = T;
            using reference = const value_type &;
            using pointer = const value_type *;
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using iterator_concept = std::contiguous_iterator_tag;

            explicit constexpr const_iterator(const T *iter = nullptr) : m_ptr{iter} {}

            [[nodiscard]]
            constexpr bool operator==(const const_iterator &other) const noexcept {
                return m_ptr == other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator!=(const const_iterator &other) const noexcept {
                return m_ptr != other.m_ptr;
            }

            [[nodiscard]]
            constexpr reference operator*() const noexcept {
                return *m_ptr;
            }

            constexpr pointer operator->() const noexcept {
                return m_ptr;
            }

            constexpr const_iterator &operator++() noexcept {
                ++m_ptr;
                return *this;
            }

            constexpr const_iterator operator++(int) noexcept {
                const_iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            constexpr const_iterator &operator--() noexcept {
                --m_ptr;
                return *this;
            }

            constexpr const_iterator operator--(int) noexcept {
                const_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            constexpr const_iterator &operator+=(const difference_type other) noexcept {
                m_ptr += other;
                return *this;
            }

            constexpr const_iterator &operator-=(const difference_type other) noexcept {
                m_ptr -= other;
                return *this;
            }

            [[nodiscard]]
            constexpr const_iterator operator+(const difference_type other) const noexcept {
                return const_iterator(m_ptr + other);
            }

            [[nodiscard]]
            constexpr const_iterator operator-(const difference_type other) const noexcept {
                return const_iterator(m_ptr - other);
            }

            [[nodiscard]]
            constexpr const_iterator operator+(const const_iterator &other) const noexcept {
                return const_iterator(*this + other.m_ptr);
            }

            [[nodiscard]]
            constexpr difference_type operator-(const const_iterator &other) const noexcept {
//                auto temp =std::distance(m_ptr, other.m_ptr);
                return m_ptr - other.m_ptr;
            }

            constexpr reference operator[](std::size_t index) const {
                return m_ptr[index];
            }

            [[nodiscard]]
            constexpr bool operator<(const const_iterator &other) const noexcept {
                return m_ptr < other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator>(const const_iterator &other) const noexcept {
                return m_ptr > other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator<=(const const_iterator &other) const noexcept {
                return m_ptr <= other.m_ptr;
            }

            [[nodiscard]]
            constexpr bool operator>=(const const_iterator &other) const noexcept {
                return m_ptr >= other.m_ptr;
            }

        private:
            const T *m_ptr;
        };

        using value_type = T;

        constexpr TempArrayProxy() noexcept
                : m_count(0), m_ptr(nullptr) {
        }

        constexpr TempArrayProxy(std::nullptr_t) noexcept
                : m_count(0), m_ptr(nullptr) {
        }

//        template<typename B = T, std::enable_if_t<!vul::is_contiguous<B>::value>* = nullptr>
        TempArrayProxy(T const &value) noexcept
                : m_count(1), m_ptr(&value) {
        }

        TempArrayProxy(uint32_t count, T const *ptr) noexcept
                : m_count(count), m_ptr(ptr) {
        }

        template<std::size_t C>
        TempArrayProxy(T const ( &ptr )[C]) noexcept
                : m_count(C), m_ptr(ptr) {
        }

#  if __GNUC__ >= 9
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winit-list-lifetime"
#  endif


        // Any type with a .data() return type implicitly convertible to T*, and a .size() return type implicitly
        // convertible to size_t. The const version can capture temporaries, with lifetime ending at end of statement.
        template<typename V,
                typename std::enable_if<std::is_convertible<decltype(std::declval<V>().data()), T *>::value &&
                                        std::is_convertible<decltype(std::declval<V>().size()), std::size_t>::value>::type * = nullptr>
        TempArrayProxy(V const &v) noexcept
                : m_count(static_cast<uint32_t>( v.size())), m_ptr(v.data()) {
        }

        TempArrayProxy(std::initializer_list<T> const &list) noexcept
                : m_count(static_cast<uint32_t>( list.size())), m_ptr(list.begin()) {
        }

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(std::initializer_list<typename std::remove_const<T>::type> const &list) noexcept
                : m_count(static_cast<uint32_t>( list.size())), m_ptr(list.begin()) {
        }

#  if __GNUC__ >= 9
#    pragma GCC diagnostic pop
#  endif


        const_iterator begin() const noexcept {
            return const_iterator{m_ptr};
        }

        const_iterator end() const noexcept {
            return const_iterator{m_ptr + m_count};
        }

        const T &front() const noexcept {
            VULKAN_HPP_ASSERT(m_count && m_ptr);
            return *m_ptr;
        }

        const T &back() const noexcept {
            VULKAN_HPP_ASSERT(m_count && m_ptr);
            return *(m_ptr + m_count - 1);
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return (m_count == 0);
        }

        [[nodiscard]]
        uint32_t size() const noexcept {
            return m_count;
        }

        T const *data() const noexcept {
            return m_ptr;
        }

        template<typename U>
        [[nodiscard]]
        TempArrayProxy<U> reinterpret_to() {
            auto new_size = (m_count * sizeof(T)) / sizeof(U);
            auto old_size_bytes = (m_count * sizeof(T));
            auto new_size_bytes = (new_size * sizeof(U));
            UUL_ASSERT(old_size_bytes == new_size_bytes,
                       fmt::format(
                               "Can't reinterpret non evenly divisible "
                               "element {} bytes vs {} bytes with element "
                               "sizes {} bytes vs {} bytes",
                               old_size_bytes, new_size_bytes,
                               sizeof(T), sizeof(U)).c_str());
            return TempArrayProxy<U>{new_size, reinterpret_cast<U *>(m_ptr)};
        }

        template<typename U>
        [[nodiscard]]
        TempArrayProxy<const U> reinterpret_to() const {
            auto new_size = (m_count * sizeof(T)) / sizeof(U);
            auto old_size_bytes = (m_count * sizeof(T));
            auto new_size_bytes = (new_size * sizeof(U));
            UUL_ASSERT(old_size_bytes == new_size_bytes,
                       fmt::format(
                               "Can't reinterpret non evenly divisible "
                               "element {} bytes vs {} bytes with element "
                               "sizes {} bytes vs {} bytes",
                               old_size_bytes, new_size_bytes,
                               sizeof(T), sizeof(U)).c_str());
            return TempArrayProxy<const U>{static_cast<std::uint32_t>(new_size), reinterpret_cast<const U *>(m_ptr)};
        }

        template<typename U>
        [[nodiscard]]
        U *reinterpret_data() noexcept {
            auto new_size = (m_count * sizeof(T)) / sizeof(U);
            auto old_size_bytes = (m_count * sizeof(T));
            auto new_size_bytes = (new_size * sizeof(U));
            UUL_ASSERT(old_size_bytes == new_size_bytes,
                       fmt::format(
                               "Can't reinterpret non evenly divisible "
                               "element {} bytes vs {} bytes with element "
                               "sizes {} bytes vs {} bytes",
                               old_size_bytes, new_size_bytes,
                               sizeof(T), sizeof(U)).c_str());
            return reinterpret_cast<U *>(m_ptr);
        }

        template<typename U>
        [[nodiscard]]
        const U *reinterpret_data() const noexcept {
            auto new_size = (m_count * sizeof(T)) / sizeof(U);
            auto old_size_bytes = (m_count * sizeof(T));
            auto new_size_bytes = (new_size * sizeof(U));
            UUL_ASSERT(old_size_bytes == new_size_bytes,
                       fmt::format(
                               "Can't reinterpret non evenly divisible "
                               "element {} bytes vs {} bytes with element "
                               "sizes {} bytes vs {} bytes",
                               old_size_bytes, new_size_bytes,
                               sizeof(T), sizeof(U)).c_str());
            return reinterpret_cast<const U *>(m_ptr);
        }

        template<
                typename B = T
                // C++11 allows the use of SFINAE right here!
                , typename = typename std::enable_if_t<!vul::is_container<B>::value>
        >
        explicit operator TempConstVoidArrayProxy() const {
            return TempConstVoidTempArrayProxy(size_bytes(), m_ptr);
        }

        explicit operator TempConstVoidArrayProxy() const {
            return TempConstVoidTempArrayProxy(size_bytes(), m_ptr);
        }

        const T &operator[](std::size_t i) const {
            return m_ptr[i];
        }
//TODO currently can't modify internally, so this is useless
//        T &operator[](std::size_t i) {
//            return m_ptr[i];
//        }

        [[nodiscard]]
        std::size_t size_bytes() const noexcept {
            return size() * sizeof(T);
        }

    private:
        uint32_t m_count;
        T const *m_ptr;
    };

    template<typename V>
    TempArrayProxy(V const &v) noexcept -> TempArrayProxy<typename V::value_type>;

    template<typename V, typename T = typename V::value_type,
            typename std::enable_if<
                    std::is_convertible<decltype(std::declval<V>().size()), std::size_t>::value>::type * = nullptr>
    auto make_proxy(const V &v) {
        return TempArrayProxy<T>(static_cast<std::uint32_t>(v.size()), v.data());
    }


    template<typename T>
    auto make_proxy(uint32_t count, T const *ptr) noexcept {
        return TempArrayProxy<T>(count, ptr);
    }

    template<typename T>
    std::size_t size_bytes(const T &t) {
        if constexpr (std::is_member_function_pointer_v<decltype(&T::size_bytes)>) {
            return t.size_bytes();
        } else {
            return t.size() * sizeof(T::value_type);
        }
    }
}
#endif //VULKANWORKSPACE_TEMPARRAYPROXY_H
