//
// Created by Shae Bolt on 9/26/2021.
//

#include "tempvoidarrayproxy.h"

constexpr vul::TempConstVoidArrayProxy::TempConstVoidArrayProxy() noexcept
        : m_size_bytes(0), m_ptr(nullptr) {}

constexpr vul::TempConstVoidArrayProxy::TempConstVoidArrayProxy(
        std::nullptr_t) noexcept
        : m_size_bytes(0), m_ptr(nullptr) {}

vul::TempConstVoidArrayProxy::TempConstVoidArrayProxy(std::size_t count,
                                                      const void *ptr) noexcept
        : m_size_bytes(count), m_ptr(reinterpret_cast<const std::byte*>(ptr)) {}

const std::byte *vul::TempConstVoidArrayProxy::begin() const noexcept {
    return m_ptr;
}

const std::byte *vul::TempConstVoidArrayProxy::end() const noexcept {
    return m_ptr + m_size_bytes;
}

bool vul::TempConstVoidArrayProxy::empty() const noexcept  {
    return (m_size_bytes == 0);
}

std::size_t vul::TempConstVoidArrayProxy::size_bytes() const noexcept{
    return m_size_bytes;
}

const void *vul::TempConstVoidArrayProxy::data() const noexcept{
    return m_ptr;
}

std::size_t vul::TempConstVoidArrayProxy::size() const noexcept {
    return size_bytes();
}
