//
// Created by Shae Bolt on 8/30/2021.
//

#ifndef VULKANWORKSPACE_TEMPARRAYPROXY_H
#define VULKANWORKSPACE_TEMPARRAYPROXY_H

#include "vul/tempvoidarrayproxy.h"
#include "vul/vkassert.h"
#include "vul/traitutils.h"
#include "vul/temparrayproxyfwd.h"
#include <gsl/span>
#include <fmt/core.h>

namespace vul {
//from https://raw.githubusercontent.com/KhronosGroup/Vulkan-Hpp/master/vulkan/vulkan.hpp
//default provided by forward declaration.
    template<typename T>
    class TempArrayProxy {
    public:
        using value_type = T;
//        using element_type = std::conditional_t<
//                is_const_only,
//                std::add_const_t<T>,
//                T
//        >;
        //want this to be implicit!

        template<
                typename B = T
                // C++11 allows the use of SFINAE right here!
                , typename = typename std::enable_if_t<!vul::is_container<B>::value>
        >
        operator TempConstVoidArrayProxy() const {
            return TempConstVoidArrayProxy(size_bytes(), m_ptr);
        }

        explicit operator TempConstVoidArrayProxy() const {
            return TempConstVoidArrayProxy(size_bytes(), m_ptr);
        }

        template<typename U>
        TempArrayProxy<U> reinterpret_to() {
            auto new_size = (m_size * sizeof(T)) / sizeof(U);
            auto old_size_bytes = (m_size * sizeof(T));
            auto new_size_bytes = (new_size * sizeof(U));
            VUL_ASSERT(old_size_bytes == new_size_bytes,
                       fmt::format(
                               "Can't reinterpret non evenly divisible "
                               "element {} bytes vs {} bytes with element "
                               "sizes {} bytes vs {} bytes",
                               old_size_bytes, new_size_bytes,
                               sizeof(T), sizeof(U)).c_str());
            return TempArrayProxy<U>{new_size, reinterpret_cast<U *>(m_ptr)};
        }

        template<typename U>
        TempArrayProxy<const U> reinterpret_to() const {
            auto new_size = (m_size * sizeof(T)) / sizeof(U);
            auto old_size_bytes = (m_size * sizeof(T));
            auto new_size_bytes = (new_size * sizeof(U));
            VUL_ASSERT(old_size_bytes == new_size_bytes,
                       fmt::format(
                               "Can't reinterpret non evenly divisible "
                               "element {} bytes vs {} bytes with element "
                               "sizes {} bytes vs {} bytes",
                               old_size_bytes, new_size_bytes,
                               sizeof(T), sizeof(U)).c_str());
            return TempArrayProxy<const U>{new_size, reinterpret_cast<const U *>(m_ptr)};
        }
//        template<
//                typename B = T
//                // C++11 allows the use of SFINAE right here!
//                , typename = typename std::enable_if_t<!vul::is_container<B>::value>
//        >
//        explicit operator TempConstVoidArrayProxy() const{
//            return TempConstVoidArrayProxy(size_bytes(), m_ptr);
//        }

        constexpr TempArrayProxy() noexcept
                : m_size(0), m_ptr(nullptr) {}

        constexpr TempArrayProxy(std::nullptr_t) noexcept
                : m_size(0), m_ptr(nullptr) {}

        TempArrayProxy(T &value) noexcept
                : m_size(1), m_ptr(&value) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(typename std::remove_const<T>::type &value) noexcept
                : m_size(1), m_ptr(&value) {}

        TempArrayProxy(std::size_t count, T *ptr) noexcept
                : m_size(count), m_ptr(ptr) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(std::size_t count,
                       typename std::remove_const<T>::type *ptr) noexcept
                : m_size(count), m_ptr(ptr) {}

#  if __GNUC__ >= 9
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winit-list-lifetime"
#  endif

        TempArrayProxy(std::initializer_list<T> const &list) noexcept
                : m_size(static_cast<std::size_t>( list.size())),
                  m_ptr(list.begin()) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::initializer_list<typename std::remove_const<T>::type> const &list) noexcept
                : m_size(static_cast<std::size_t>( list.size())),
                  m_ptr(list.begin()) {}

        TempArrayProxy(std::initializer_list<T> &list) noexcept
                : m_size(static_cast<std::size_t>( list.size())),
                  m_ptr(list.begin()) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::initializer_list<typename std::remove_const<T>::type> &list) noexcept
                : m_size(static_cast<std::size_t>( list.size())),
                  m_ptr(list.begin()) {}

#  if __GNUC__ >= 9
#    pragma GCC diagnostic pop
#  endif

        template<size_t N>
        TempArrayProxy(std::array<T, N> const &data) noexcept
                : m_size(N), m_ptr(data.data()) {}

        template<size_t N, typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::array<typename std::remove_const<T>::type, N> const &data) noexcept
                : m_size(N), m_ptr(data.data()) {}

        template<size_t N>
        TempArrayProxy(std::array<T, N> &data) noexcept
                : m_size(N), m_ptr(data.data()) {}

        template<size_t N, typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::array<typename std::remove_const<T>::type, N> &data) noexcept
                : m_size(N), m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>>
        TempArrayProxy(std::vector<T, Allocator> const &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>,
                typename B      = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::vector<typename std::remove_const<T>::type, Allocator> const &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>>
        TempArrayProxy(std::vector<T, Allocator> &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>,
                typename B      = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::vector<typename std::remove_const<T>::type, Allocator> &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}


        template<size_t N = gsl::dynamic_extent>
        TempArrayProxy(gsl::span<T, N> const &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}

        template<size_t N = gsl::dynamic_extent,
                typename B = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                gsl::span<typename std::remove_const<T>::type, N> const &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}

        template<size_t N = gsl::dynamic_extent>
        TempArrayProxy(gsl::span<T, N> &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}

        template<size_t N = gsl::dynamic_extent,
                typename B = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                gsl::span<typename std::remove_const<T>::type, N> &data) noexcept
                : m_size(static_cast<std::size_t>( data.size())),
                  m_ptr(data.data()) {}


        [[nodiscard]]
        const T *begin() const noexcept {
            return m_ptr;
        }

        [[nodiscard]]
        const T *end() const noexcept {
            return m_ptr + m_size;
        }

        [[nodiscard]]
        const T &front() const noexcept {
            VUL_ASSERT(m_size && m_ptr);
            return *m_ptr;
        }

        [[nodiscard]]
        const T &back() const noexcept {
            VUL_ASSERT(m_size && m_ptr);
            return *(m_ptr + m_size - 1);
        }

        [[nodiscard]]
        T &front() noexcept {
            VUL_ASSERT(m_size && m_ptr);
            return *m_ptr;
        }

        [[nodiscard]]
        T &back() noexcept {
            VUL_ASSERT(m_size && m_ptr);
            return *(m_ptr + m_size - 1);
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return (m_size == 0);
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return m_size;
        }

        [[nodiscard]]
        std::size_t size_bytes() const noexcept {
            return size() * sizeof(T);
        }

        [[nodiscard]]
        const T *data() const noexcept {
            return m_ptr;
        }

        [[nodiscard]]
        T *data() noexcept {
            return m_ptr;
        }

        const T &operator[](std::size_t i) const {
            return m_ptr[i];
        }

        T &operator[](std::size_t i) {
            return m_ptr[i];
        }

    private:
        std::size_t m_size;
        T *m_ptr;
    };
}
#endif //VULKANWORKSPACE_TEMPARRAYPROXY_H
