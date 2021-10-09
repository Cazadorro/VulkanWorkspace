//
// Created by Shae Bolt on 9/26/2021.
//

#ifndef VULKANWORKSPACE_TEMPVOIDARRAYPROXY_H
#define VULKANWORKSPACE_TEMPVOIDARRAYPROXY_H
#include <gsl/span> 
#include <vector>
#include <array> 
#include <initializer_list>
#include <cstddef>
#include <cstdint>
namespace vul {
    class TempConstVoidArrayProxy {
        public:
            using value_type = std::byte;
            constexpr TempConstVoidArrayProxy() noexcept;

            constexpr TempConstVoidArrayProxy(std::nullptr_t) noexcept;

            TempConstVoidArrayProxy(std::size_t count, const void *ptr) noexcept;

            template<typename T>
            TempConstVoidArrayProxy(T &value) noexcept : TempConstVoidArrayProxy(sizeof(T),&value){}


#  if __GNUC__ >= 9
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winit-list-lifetime"
#  endif
            template<typename T>
            TempConstVoidArrayProxy(const std::initializer_list<T> &list) noexcept : TempConstVoidArrayProxy(list.size() * sizeof(T),list.begin()){}

#  if __GNUC__ >= 9
#    pragma GCC diagnostic pop
#  endif

            template<typename T, size_t N>
            TempConstVoidArrayProxy(const std::array<T, N> &data) noexcept: TempConstVoidArrayProxy(data.size() * sizeof(T), data.data()){}
                  
            template<typename T, class Allocator = std::allocator<typename std::remove_const<T>::type>>
            TempConstVoidArrayProxy(const std::vector<T, Allocator> &data) noexcept: TempConstVoidArrayProxy(data.size() * sizeof(T), data.data()){}


            template<typename T, size_t N = gsl::dynamic_extent>
            TempConstVoidArrayProxy(const gsl::span<T, N> &data) noexcept: TempConstVoidArrayProxy(data.size() * sizeof(T), data.data()){}

            [[nodiscard]]
            const std::byte *begin() const noexcept;

            [[nodiscard]]
            const std::byte *end() const noexcept;

            [[nodiscard]]
            bool empty() const noexcept;

            [[nodiscard]]
            std::size_t size_bytes() const noexcept;

            [[nodiscard]]
            const void *data() const noexcept ;
        private:
            std::size_t m_size_bytes;
            const std::byte *m_ptr;

    };




}


#endif //VULKANWORKSPACE_TEMPVOIDARRAYPROXY_H
