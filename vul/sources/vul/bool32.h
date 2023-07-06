//
// Created by Bolt on 7/5/2023.
//

#ifndef VULKANWORKSPACE_BOOL32_H
#define VULKANWORKSPACE_BOOL32_H
#include <vulkan/vulkan.h>
namespace vul{
    class Bool32{
    public:
        Bool32() noexcept;
        Bool32(bool value) noexcept;
        explicit Bool32(VkBool32 value) noexcept;
        [[nodiscard]]
        explicit operator bool() const noexcept;
        [[nodiscard]]
        explicit operator VkBool32() const noexcept;
        Bool32& operator =(bool value) noexcept;
    private:
        VkBool32 m_value;
    };

    [[nodiscard]]
    Bool32 operator !(Bool32 a) noexcept;
    [[nodiscard]]
    Bool32 operator &&(Bool32 a, Bool32 b) noexcept;
    [[nodiscard]]
    Bool32 operator ||(Bool32 a, Bool32 b) noexcept;

    [[nodiscard]]
    bool operator &&(Bool32 a, bool b) noexcept;
    [[nodiscard]]
    bool operator ||(Bool32 a, bool b) noexcept;
    [[nodiscard]]
    bool operator &&(bool a, Bool32 b) noexcept;
    [[nodiscard]]
    bool operator ||(bool a, Bool32 b) noexcept;

    [[nodiscard]]
    bool operator == (Bool32 a, Bool32 b) noexcept;
    [[nodiscard]]
    bool operator != (Bool32 a, Bool32 b) noexcept;

    [[nodiscard]]
    bool operator == (VkBool32 a, Bool32 b) noexcept;
    [[nodiscard]]
    bool operator != (VkBool32 a, Bool32 b) noexcept;

    [[nodiscard]]
    bool operator == (Bool32 a, VkBool32 b) noexcept;
    [[nodiscard]]
    bool operator != (Bool32 a, VkBool32 b) noexcept;

    [[nodiscard]]
    bool operator == (bool a, Bool32 b) noexcept;
    [[nodiscard]]
    bool operator != (bool a, Bool32 b) noexcept;

    [[nodiscard]]
    bool operator == (Bool32 a, bool b);
    [[nodiscard]]
    bool operator != (Bool32 a, bool b);
}
#endif //VULKANWORKSPACE_BOOL32_H
