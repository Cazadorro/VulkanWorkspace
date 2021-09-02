//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_DEVICE_H
#define VULKANWORKSPACE_DEVICE_H

#include"vul/physicaldevice.h"
#include "vul/bitmasks.h"
#include"vul/debugutils.h"
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<unordered_map>

namespace vul {
    class PhysicalDevice;;

    class Queue;

    class RenderPass;

    class Framebuffer;

    class ImageView;

    class ShaderModule;

    class BinarySemaphore;

    class TimelineSemaphore;

    class DescriptorPool;

    class DescriptorSetLayoutBuilder;

    class DescriptorSetLayout;

    class LayoutBuilderCount;

    class PipelineLayout;

    class TimelineSemaphore;

    class CommandPool;

    template<typename T>
    class TempArrayProxy;

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
        ExpectedResult<BinarySemaphore> createBinarySemaphore(
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<TimelineSemaphore>
        createTimelineSemaphore(std::uint64_t initialValue,
                                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<DescriptorPool> createDescriptorPool(
                const gsl::span<const LayoutBuilderCount> &layoutBuilders,
                vul::DescriptorPoolCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const DescriptorSetLayout *> &setLayouts,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const DescriptorSetLayout *> &setLayouts,
                const TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const DescriptorSetLayout> &setLayouts,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const DescriptorSetLayout> &setLayouts,
                const TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const std::reference_wrapper<DescriptorSetLayout>> &setLayouts,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const std::reference_wrapper<DescriptorSetLayout>> &setLayouts,
                const TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<ShaderModule>
        createShaderModule(const TempArrayProxy<const std::uint32_t> &code,
                           const void *pNext = nullptr,
                           const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<CommandPool>
        createCommandPool(std::uint32_t queueFamilyIndex,
                          vul::CommandPoolCreateBitMask flags = {},
                          const void *pNext = nullptr,
                          const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<VkImageView> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                vul::FramebufferCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<const std::reference_wrapper<ImageView>> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                vul::FramebufferCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<const ImageView> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                vul::FramebufferCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<const ImageView*> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                vul::FramebufferCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        Result waitIdle() const;

        ~Device();

        Device(Device &&rhs) noexcept;

//        was noexcept?
        Device &operator=(Device &&rhs) noexcept;

        Device(const Device &rhs) = delete;

        Device &operator=(Device &rhs) = delete;

        template<typename T>
        Result setObjectName(T objectHandle, const std::string &string) const {
            auto objectNameInfo = vul::createObjectNameInfo(objectHandle,
                                                            string);
            auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(
                    m_handle, "vkSetDebugUtilsObjectNameEXT");
            if (func != nullptr) {
                return static_cast<Result>(func(m_handle, &objectNameInfo));
            } else {
                return Result::ErrorExtensionNotPresent;
            }
        }

        Result setObjectName(const std::string &string);

        [[nodiscard]]
        const PhysicalDevice &getPhysicalDevice() const;

        Result wait(const VkSemaphoreWaitInfo &waitInfo,
                    std::uint64_t timeout = UINT64_MAX) const;

        Result wait(const TempArrayProxy<const TimelineSemaphore *> &semaphores,
                    const TempArrayProxy<const std::uint64_t> &values,
                    std::uint64_t timeout = UINT64_MAX,
                    vul::SemaphoreWaitBitMask waitFlags = {},
                    const void *pNext = nullptr) const;

        Result
        wait(const TempArrayProxy<const std::reference_wrapper<TimelineSemaphore>> &semaphores,
             const TempArrayProxy<const std::uint64_t> &values,
             std::uint64_t timeout = UINT64_MAX,
             vul::SemaphoreWaitBitMask waitFlags = {},
             const void *pNext = nullptr) const;

    private:
        PhysicalDevice m_physicalDevice;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDevice m_handle = VK_NULL_HANDLE;
        std::vector<QueueFamilyIndexMapping> m_queueFamilyIndexMappings;
        std::vector<std::uint8_t> m_queueInUseMask;
    };
}


#endif //VULKANWORKSPACE_DEVICE_H
