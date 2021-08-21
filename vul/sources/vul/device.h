//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_DEVICE_H
#define VULKANWORKSPACE_DEVICE_H

#include"vul/physicaldevice.h"
#include"vul/debugutils.h"
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<unordered_map>

namespace vul {
    class PhysicalDevice;

    class Queue;

    class BinarySemaphore;
    class TimelineSemaphore;

    struct QueueFamilyIndexMapping {
        std::uint32_t queueFamilyIndex;
        std::uint32_t queueIndex;
    };

    class Device {
    public:
        Device() = default;
        Device(const PhysicalDevice &physicalDevice, VkDevice handle,
               std::vector<QueueFamilyIndexMapping> queueFamilyIndexMappings,
               const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkDevice get() const;

        [[nodiscard]]
        Queue getQueue(std::uint32_t index) const;

        [[nodiscard]]
        std::optional<Queue> getQueueAt(std::uint32_t index);

        [[nodiscard]]
        ExpectedResult<BinarySemaphore> createBinarySemaphore(const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        ExpectedResult<TimelineSemaphore> createTimelineSemaphore(std::uint64_t initialValue, const VkAllocationCallbacks *pAllocator = nullptr);

        Result waitIdle() const;

        ~Device();
        Device(Device &&rhs) noexcept;
//        was noexcept?
        Device &operator=(Device &&rhs) noexcept;

        Device(const Device &rhs) = delete;

        Device &operator=(Device &rhs) = delete;

        template<typename T>
        Result setObjectName(T objectHandle, const std::string& string) const{
            auto objectNameInfo = vul::createObjectNameInfo(objectHandle, string);
            auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(m_handle, "vkSetDebugUtilsObjectNameEXT");
            if (func != nullptr) {
                return static_cast<Result>(func(m_handle, &objectNameInfo));
            } else {
                return Result::ErrorExtensionNotPresent;
            }
        }

        Result setObjectName(const std::string& string);
    private:
        PhysicalDevice m_physicalDevice;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDevice m_handle = VK_NULL_HANDLE;
        std::vector<QueueFamilyIndexMapping> m_queueFamilyIndexMappings;
        std::vector<std::uint8_t> m_queueInUseMask;
    };
}


#endif //VULKANWORKSPACE_DEVICE_H
