//
// Created by Shae Bolt on 8/29/2021.
//

#ifndef VULKANWORKSPACE_INPUTPROXY_H
#define VULKANWORKSPACE_INPUTPROXY_H
namespace vul {

    //Allows single element, multi element and owns elements.
    class VectorProxy{
    public:
    private:
        std::vector<
        std::size_t m_count;
        T * m_ptr
    };
#if !defined( VULKAN_HPP_DISABLE_ENHANCED_MODE )

    template<typename T>
    class TempArrayProxy {
    public:
        VULKAN_HPP_CONSTEXPR TempArrayProxy()

        VULKAN_HPP_NOEXCEPT: m_count(0), m_ptr(nullptr )
        {}

        VULKAN_HPP_CONSTEXPR TempArrayProxy(std::nullptr_t)

        VULKAN_HPP_NOEXCEPT
                : m_count(0)
        , m_ptr(
        nullptr )
        {}

        TempArrayProxy(T &value)

        VULKAN_HPP_NOEXCEPT
                : m_count(1)
        , m_ptr( &
        value )
        {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(typename std::remove_const<T>::type &value)

        VULKAN_HPP_NOEXCEPT
                : m_count(1)
        , m_ptr( &
        value )
        {}

        TempArrayProxy(uint32_t count, T *ptr)

        VULKAN_HPP_NOEXCEPT
                : m_count(count)
        , m_ptr(ptr) {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(uint32_t count, typename std::remove_const<T>::type *ptr)

        VULKAN_HPP_NOEXCEPT
                : m_count(count)
        , m_ptr(ptr) {}

#  if __GNUC__ >= 9
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winit-list-lifetime"
#  endif

        TempArrayProxy(std::initializer_list <T> const &list)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( list.size()))
        , m_ptr(list
        .

        begin()

        )
        {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::initializer_list<typename std::remove_const<T>::type> const &list)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( list.size()))
        , m_ptr(list
        .

        begin()

        )
        {}

        TempArrayProxy(std::initializer_list <T> &list)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( list.size()))
        , m_ptr(list
        .

        begin()

        )
        {}

        template<typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::initializer_list<typename std::remove_const<T>::type> &list)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( list.size()))
        , m_ptr(list
        .

        begin()

        )
        {}

#  if __GNUC__ >= 9
#    pragma GCC diagnostic pop
#  endif

        template<size_t N>
        TempArrayProxy(std::array <T, N> const &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(N)
        , m_ptr(data
        .

        data()

        )
        {}

        template<size_t N, typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::array<typename std::remove_const<T>::type, N> const &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(N)
        , m_ptr(data
        .

        data()

        )
        {}

        template<size_t N>
        TempArrayProxy(std::array <T, N> &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(N)
        , m_ptr(data
        .

        data()

        )
        {}

        template<size_t N, typename B = T, typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(std::array<typename std::remove_const<T>::type, N> &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(N)
        , m_ptr(data
        .

        data()

        )
        {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>>
        TempArrayProxy(std::vector <T, Allocator> const &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( data.size()))
        , m_ptr(data
        .

        data()

        )
        {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>,
                typename B      = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::vector<typename std::remove_const<T>::type, Allocator> const &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( data.size()))
        , m_ptr(data
        .

        data()

        )
        {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>>
        TempArrayProxy(std::vector <T, Allocator> &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( data.size()))
        , m_ptr(data
        .

        data()

        )
        {}

        template<class Allocator = std::allocator<typename std::remove_const<T>::type>,
                typename B      = T,
                typename std::enable_if<std::is_const<B>::value, int>::type = 0>
        TempArrayProxy(
                std::vector<typename std::remove_const<T>::type, Allocator> &data)

        VULKAN_HPP_NOEXCEPT
                : m_count(static_cast<uint32_t>( data.size()))
        , m_ptr(data
        .

        data()

        )
        {}

#  if defined( VULKAN_HPP_SUPPORT_SPAN )
        template <size_t N = std::dynamic_extent>
ArrayProxy( std::span<T, N> const & data ) VULKAN_HPP_NOEXCEPT
  : m_count( static_cast<uint32_t>( data.size() ) )
  , m_ptr( data.data() )
{}

template <size_t N                                                    = std::dynamic_extent,
          typename B                                                  = T,
          typename std::enable_if<std::is_const<B>::value, int>::type = 0>
ArrayProxy( std::span<typename std::remove_const<T>::type, N> const & data ) VULKAN_HPP_NOEXCEPT
  : m_count( static_cast<uint32_t>( data.size() ) )
  , m_ptr( data.data() )
{}

template <size_t N = std::dynamic_extent>
ArrayProxy( std::span<T, N> & data ) VULKAN_HPP_NOEXCEPT
  : m_count( static_cast<uint32_t>( data.size() ) )
  , m_ptr( data.data() )
{}

template <size_t N                                                    = std::dynamic_extent,
          typename B                                                  = T,
          typename std::enable_if<std::is_const<B>::value, int>::type = 0>
ArrayProxy( std::span<typename std::remove_const<T>::type, N> & data ) VULKAN_HPP_NOEXCEPT
  : m_count( static_cast<uint32_t>( data.size() ) )
  , m_ptr( data.data() )
{}
#  endif

        const T *begin() const

        VULKAN_HPP_NOEXCEPT
        {
            return m_ptr;
        }

        const T *end() const

        VULKAN_HPP_NOEXCEPT
        {
            return m_ptr + m_count;
        }

        const T &front() const

        VULKAN_HPP_NOEXCEPT
        {
            VULKAN_HPP_ASSERT(m_count && m_ptr);
            return *m_ptr;
        }

        const T &back() const

        VULKAN_HPP_NOEXCEPT
        {
            VULKAN_HPP_ASSERT(m_count && m_ptr);
            return *(m_ptr + m_count - 1);
        }

        bool empty() const

        VULKAN_HPP_NOEXCEPT
        {
            return (m_count == 0);
        }

        uint32_t size() const

        VULKAN_HPP_NOEXCEPT
        {
            return m_count;
        }

        T *data() const

        VULKAN_HPP_NOEXCEPT
        {
            return m_ptr;
        }

    private:
        uint32_t m_size;
        T *m_ptr;
    };

}
#endif //VULKANWORKSPACE_INPUTPROXY_H
