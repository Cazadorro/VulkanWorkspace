//
// Created by Bolt on 7/5/2023.
//

#include "bool32.h"

vul::Bool32::Bool32(bool value) noexcept: m_value(value ? VK_TRUE : VK_FALSE){

}

vul::Bool32::Bool32(VkBool32 value)  noexcept: m_value(value){

}

vul::Bool32::operator bool() const noexcept{
    return (m_value == VK_TRUE);
}

vul::Bool32::operator VkBool32() const noexcept{
    return m_value;
}

vul::Bool32 &vul::Bool32::operator=(bool value) noexcept {
    m_value = value ? VK_TRUE : VK_FALSE;
    return *this;
}

vul::Bool32::Bool32() noexcept = default;

vul::Bool32 vul::operator!(vul::Bool32 a) noexcept {
    return (a == true) ? Bool32(VK_FALSE) : Bool32(VK_TRUE);
}

vul::Bool32 vul::operator&&(vul::Bool32 a, vul::Bool32 b) noexcept {
    return vul::Bool32{static_cast<bool>(a) && static_cast<bool>(b)};
}

vul::Bool32 vul::operator||(vul::Bool32 a, vul::Bool32 b) noexcept {
    return vul::Bool32{static_cast<bool>(a) || static_cast<bool>(b)};
}

bool vul::operator&&(vul::Bool32 a, bool b) noexcept {
    return static_cast<bool>(a) && b;
}

bool vul::operator||(vul::Bool32 a, bool b) noexcept {
    return static_cast<bool>(a) || b;
}

bool vul::operator&&(bool a, vul::Bool32 b) noexcept {
    return a && static_cast<bool>(b);
}

bool vul::operator||(bool a, vul::Bool32 b) noexcept {
    return a || static_cast<bool>(b);
}

bool vul::operator==(vul::Bool32 a, vul::Bool32 b) noexcept {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

bool vul::operator!=(vul::Bool32 a, vul::Bool32 b) noexcept {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

bool vul::operator==(VkBool32 a, vul::Bool32 b) noexcept {
    return (a == VK_TRUE) == static_cast<bool>(b);
}

bool vul::operator!=(VkBool32 a, vul::Bool32 b) noexcept {
    return (a == VK_TRUE) != static_cast<bool>(b);
}

bool vul::operator==(vul::Bool32 a, VkBool32 b) noexcept {
    return static_cast<bool>(a) == (b == VK_TRUE);
}

bool vul::operator!=(vul::Bool32 a, VkBool32 b) noexcept {
    return static_cast<bool>(a) != (b == VK_TRUE);
}

bool vul::operator==(bool a, vul::Bool32 b) noexcept {
    return a == static_cast<bool>(b);
}

bool vul::operator!=(bool a, vul::Bool32 b) noexcept {
    return a != static_cast<bool>(b);
}

bool vul::operator==(vul::Bool32 a, bool b) {
    return static_cast<bool>(a) == b;
}

bool vul::operator!=(vul::Bool32 a, bool b) {
    return static_cast<bool>(a) != b;
}
