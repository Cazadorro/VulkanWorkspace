//
// Created by Shae Bolt on 8/29/2021.
//

#ifndef VULKANWORKSPACE_DESCRIPTORPOOL_H
#define VULKANWORKSPACE_DESCRIPTORPOOL_H
#include "vul/enumsfwd.h"
#include <vulkan/vulkan.h>
#include <string>

namespace vul{
    //TODO actually fill out
    class Device;
    class DescriptorPool{
    public:
        DescriptorPool() = default;

        DescriptorPool(const Device &device,
                       VkDescriptorPool handle,
                const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkDescriptorPool get() const;

        ~DescriptorPool();

        DescriptorPool(DescriptorPool &&rhs) noexcept;

//        was noexcept?
        DescriptorPool &operator=(DescriptorPool &&rhs) noexcept;

        DescriptorPool(const DescriptorPool &rhs) = delete;

        DescriptorPool &operator=(DescriptorPool &rhs) = delete;

        Result setObjectName(const std::string &string);
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDescriptorPool m_handle = VK_NULL_HANDLE;
    };
}
#endif //VULKANWORKSPACE_DESCRIPTORPOOL_H
