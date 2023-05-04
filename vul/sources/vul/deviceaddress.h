//
// Created by Bolt on 5/4/2023.
//

#ifndef VULKANWORKSPACE_DEVICEADDRESS_H
#define VULKANWORKSPACE_DEVICEADDRESS_H
#include <vulkan/vulkan.h>
#include <cstdint>
#include <concepts>
namespace vul{
    struct DeviceAddress{
        DeviceAddress() = default;
        explicit DeviceAddress(VkDeviceAddress address);
        [[nodiscard]]
        VkDeviceAddress &get();
        [[nodiscard]]
        const VkDeviceAddress &get() const;

        explicit operator VkDeviceAddress() const;

        DeviceAddress& operator++();

        DeviceAddress operator++(int);

        DeviceAddress& operator--();

        DeviceAddress operator--(int);

        DeviceAddress& operator += (std::integral auto rhs){
            m_address += rhs;
            return *this;
        }
        DeviceAddress& operator -= (std::integral auto rhs){
            m_address -= rhs;
            return *this;
        }
        DeviceAddress& operator = (VkDeviceAddress address);
    private:
        VkDeviceAddress m_address = 0;
    };

    [[nodiscard]]
    DeviceAddress operator + (DeviceAddress address, std::integral auto rhs){
        auto new_address = address;
        new_address += rhs;
        return new_address;
    }
    [[nodiscard]]
    DeviceAddress operator - (DeviceAddress address, std::integral auto rhs){
        auto new_address = address;
        new_address += rhs;
        return new_address;
    }
}
#endif //VULKANWORKSPACE_DEVICEADDRESS_H
