//
// Created by Shae Bolt on 8/30/2021.
//

#ifndef VULKANWORKSPACE_TEMPARRAYPROXY_H
#define VULKANWORKSPACE_TEMPARRAYPROXY_H

#include <vkassert>
#include <gsl/span>

namespace vul {
//from https://raw.githubusercontent.com/KhronosGroup/Vulkan-Hpp/master/vulkan/vulkan.hpp
    template<typename T>
    class TempArrayProxy {
    public:
        constexpr TempArrayProxy() noexcept
                : m_size(0), m_ptr(nullptr) {}

        constexpr TempArrayProxy(std::nullptr_t) noexcept
                : m_size(0), m_ptr(nullptr) {}

        TempArrayProxy(T &value) noexcept
                : m_size(1), m_ptr(&value) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(typename std::remove_const<T>::type &value) noexcept
                : m_size(1), m_ptr(&value) {}

        TempArrayProxy(uint32_t count, T *ptr) noexcept
                : m_size(count), m_ptr(ptr) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(uint32_t count,
                       typename std::remove_const<T>::type *ptr) noexcept
                : m_size(count), m_ptr(ptr) {}

#  if __GNUC__ >= 9
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winit-list-lifetime"
#  endif

        TempArrayProxy(std::initializer_list<T> const &list) noexcept
                : m_size(static_cast<uint32_t>( list.size())),
                  m_ptr(list.begin()) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::initializer_list<typename std::remove_const<T>::type> const &list) noexcept
                : m_size(static_cast<uint32_t>( list.size())),
                  m_ptr(list.begin()) {}

        TempArrayProxy(std::initializer_list<T> &list) noexcept
                : m_size(static_cast<uint32_t>( list.size())),
                  m_ptr(list.begin()) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::initializer_list<typename std::remove_const<T>::type> &list) noexcept
                : m_size(static_cast<uint32_t>( list.size())),
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
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>,
                typename B      = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::vector<typename std::remove_const<T>::type, Allocator> const &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>>
        TempArrayProxy(std::vector<T, Allocator> &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>,
                typename B      = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::vector<typename std::remove_const<T>::type, Allocator> &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}


        template<size_t N = std::dynamic_extent>
        TempArrayProxy(gsl::span<T, N> const &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}

        template<size_t N = std::dynamic_extent,
                typename B = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                gsl::span<typename std::remove_const<T>::type, N> const &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}

        template<size_t N = std::dynamic_extent>
        TempArrayProxy(gsl::span<T, N> &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
                  m_ptr(data.data()) {}

        template<size_t N = std::dynamic_extent,
                typename B = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                gsl::span<typename std::remove_const<T>::type, N> &data) noexcept
                : m_size(static_cast<uint32_t>( data.size())),
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
        const T *data() const noexcept {
            return m_ptr;
        }

        [[nodiscard]]
        T *data() noexcept {
            return m_ptr;
        }

    private:
        std::size_t m_size;
        T *m_ptr;
    };
}
#endif //VULKANWORKSPACE_TEMPARRAYPROXY_H
