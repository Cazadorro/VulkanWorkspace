//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_DEVICE_H
#define VULKANWORKSPACE_DEVICE_H


#include"vul/physicaldevice.h"
#include"vul/debugutils.h"
#include"vul/temparrayproxyfwd.h"
#include"vul/enums.h"
#include<uul/enumflags.h>
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<unordered_map>

namespace vul {
    class PhysicalDevice;

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

    struct LayoutBuilderCount;

    class PipelineLayout;

    class TimelineSemaphore;

    class CommandPool;

    struct PushConstantRange;

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
        Queue getQueue(std::uint32_t index)const;

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
                const TempArrayProxy<const LayoutBuilderCount> &layoutBuilders,
                uul::EnumFlags<vul::DescriptorPoolCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<DescriptorPool> createDescriptorPool(
                const TempArrayProxy<const VkDescriptorPoolSize> &poolSizes,
                std::uint32_t maxSets,
                uul::EnumFlags<vul::DescriptorPoolCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;


        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayoutImpl(
                const TempArrayProxy<const VkDescriptorSetLayout> &setLayouts,
                const TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
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
                const TempArrayProxy<const PushConstantRange> &pushConstantRanges,
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
                const TempArrayProxy<const PushConstantRange> &pushConstantRanges,
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
                const TempArrayProxy<const PushConstantRange> &pushConstantRanges,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<PipelineLayout> createPipelineLayout(
                const TempArrayProxy<const PushConstantRange> &pushConstantRanges,
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<ShaderModule>
        createShaderModule(const TempArrayProxy<const std::uint32_t> &code,
                           const void *pNext = nullptr,
                           const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<ShaderModule>
        createShaderModule(std::string_view spv_source_loc,
                           const void *pNext = nullptr,
                           const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<CommandPool>
        createCommandPool(std::uint32_t queueFamilyIndex,
                          uul::EnumFlags<vul::CommandPoolCreateFlagBits> flags = {},
                          const void *pNext = nullptr,
                          const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<VkImageView> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                uul::EnumFlags<vul::FramebufferCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<const std::reference_wrapper<ImageView>> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                uul::EnumFlags<vul::FramebufferCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<const ImageView> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                uul::EnumFlags<vul::FramebufferCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<Framebuffer>
        createFramebuffer(
                const RenderPass &renderPass,
                const TempArrayProxy<const ImageView*> &imageViews,
                VkExtent2D widthHeight,
                std::uint32_t layers = 1,
                uul::EnumFlags<vul::FramebufferCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        Result waitIdle() const;

        ~Device();

        Device(Device &&rhs) noexcept;

//        was noexcept?
        Device &operator=(Device &&rhs) noexcept;

        Device(const Device &rhs) = delete;

        Device &operator=(const Device &rhs) = delete;

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
                    uul::EnumFlags<vul::SemaphoreWaitFlagBits> waitFlags = {},
                    const void *pNext = nullptr) const;

        Result
        wait(const TempArrayProxy<const std::reference_wrapper<TimelineSemaphore>> &semaphores,
             const TempArrayProxy<const std::uint64_t> &values,
             std::uint64_t timeout = UINT64_MAX,
             uul::EnumFlags<vul::SemaphoreWaitFlagBits> waitFlags = {},
             const void *pNext = nullptr) const;

        void updateDescriptorSets(const TempArrayProxy<const VkWriteDescriptorSet> &descriptorWrites) const;
        void updateDescriptorSets(const TempArrayProxy<const VkWriteDescriptorSet> &descriptorWrites, const TempArrayProxy<const VkCopyDescriptorSet> &descriptorCopies) const;
    private:
        PhysicalDevice m_physicalDevice;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDevice m_handle = VK_NULL_HANDLE;
        std::vector<QueueFamilyIndexMapping> m_queueFamilyIndexMappings;
        std::vector<std::uint8_t> m_queueInUseMask;
    };
}


#endif //VULKANWORKSPACE_DEVICE_H
