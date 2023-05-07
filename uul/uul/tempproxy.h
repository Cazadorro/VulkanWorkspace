//
// Created by Bolt on 5/5/2023.
//

#ifndef VULKANWORKSPACE_TEMPPROXY_H
#define VULKANWORKSPACE_TEMPPROXY_H
#include "uul/assert.h"
#include <fmt/core.h>
#include <type_traits>
#include <cstdint>


namespace uul{
    template<typename T>
    class temp_proxy {
    public:
        using value_type = T;
        constexpr temp_proxy() noexcept
                : m_count(0), m_ptr(nullptr) {
        }

        constexpr temp_proxy(std::nullptr_t) noexcept
                : m_count(0), m_ptr(nullptr) {
        }

//        template<typename B = T, std::enable_if_t<!vul::is_contiguous<B>::value>* = nullptr>
        temp_proxy(T const &value) noexcept
                : m_count(1), m_ptr(&value) {
        }

        temp_proxy(uint32_t count, T const *ptr) noexcept
                : m_count(count), m_ptr(ptr) {
        }

        template<std::size_t C>
        temp_proxy(T const ( &ptr )[C]) noexcept
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
        temp_proxy(V const &v) noexcept
                : m_count(static_cast<uint32_t>( v.size())), m_ptr(v.data()) {
        }

        temp_proxy(std::initializer_list<T> const &list) noexcept
                : m_count(static_cast<uint32_t>( list.size())), m_ptr(list.begin()) {
        }

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        temp_proxy(std::initializer_list<typename std::remove_const<T>::type> const &list) noexcept
                : m_count(static_cast<uint32_t>( list.size())), m_ptr(list.begin()) {
        }

#  if __GNUC__ >= 9
#    pragma GCC diagnostic pop
#  endif


        const T *begin() const noexcept {
            return m_ptr;
        }

        const T *end() const noexcept {
            return m_ptr + m_count;
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
        temp_proxy<U> reinterpret_to() {
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
            return temp_proxy<U>{new_size, reinterpret_cast<U *>(m_ptr)};
        }

        template<typename U>
        [[nodiscard]]
        temp_proxy<const U> reinterpret_to() const {
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
            return temp_proxy<const U>{static_cast<std::uint32_t>(new_size), reinterpret_cast<const U *>(m_ptr)};
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

//        explicit operator TempConstVoidArrayProxy() const {
//            return TempConstVoidTempArrayProxy(size_bytes(), m_ptr);
//        }

        const T &operator[](std::size_t i) const {
            return m_ptr[i];
        }

        T &operator[](std::size_t i) {
            return m_ptr[i];
        }

        [[nodiscard]]
        std::size_t size_bytes() const noexcept {
            return size() * sizeof(T);
        }

    private:
        uint32_t m_count;
        T const *m_ptr;
    };


    template<typename V, typename T = typename V::value_type,
            typename std::enable_if<
                    std::is_convertible<decltype(std::declval<V>().size()), std::size_t>::value>::type * = nullptr>
    auto make_proxy(const V& v) {
        return TempArrayProxy<T>(static_cast<std::uint32_t>(v.size()), v.data());
    }


    template<typename T>
    auto make_proxy(uint32_t count, T const *ptr) noexcept{
        return TempArrayProxy<T>(count, ptr);
    }
}
#endif //VULKANWORKSPACE_TEMPPROXY_H
