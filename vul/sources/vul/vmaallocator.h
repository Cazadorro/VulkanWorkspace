//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_VMAALLOCATOR_H
#define VULKANWORKSPACE_VMAALLOCATOR_H
#include "vul/device.h"
#include "vul/physicaldevice.h"
#include "vul/instance.h"
#include <vk_mem_alloc.h>
namespace vul{
    template<typename T>
    class ExpectedResult;
    class VmaAllocator {
    public:
        VmaAllocator() = default;
        [[nodiscard]]
        static ExpectedResult<VmaAllocator> create(const Instance& instance, const PhysicalDevice& physicalDevice, const Device& device);
        ~VmaAllocator();
        [[nodiscard]]
        VmaAllocatorInfo getAllocatorInfo() const;
        [[nodiscard]]
        VkDevice getVkDevice() const;
        [[nodiscard]]
        VkPhysicalDevice getVkPhysicalDevice() const;
        [[nodiscard]]
        VkInstance getVkInstance() const;
        //TODO MOVE ONLY!
    private:
        ::VmaAllocator m_handle = VK_NULL_HANDLE;
    };
}


#endif //VULKANWORKSPACE_VMAALLOCATOR_H
