//
// Created by Shae Bolt on 6/9/2021.
//

#include "vul/device.h"
#include "vul/imageview.h"
#include "vul/framebuffer.h"
#include "vul/renderpass.h"
#include "vul/commandpool.h"
#include "vul/shadermodule.h"
#include "vul/semaphore.h"
#include "vul/framebuffer.h"
#include "vul/pipelinelayout.h"
#include "vul/descriptorpool.h"
#include "vul/descriptorsetlayout.h"
#include "vul/queue.h"
#include "vul/vkassert.h"
#include "vul/temparrayproxy.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>


vul::Device::Device(const vul::PhysicalDevice &physicalDevice, VkDevice handle,
                    std::vector<QueueFamilyIndexMapping> queueFamilyIndexMappings,
                    const VkAllocationCallbacks *pAllocator)
        : m_physicalDevice(physicalDevice),
          m_pAllocator(pAllocator),
          m_handle(handle),
          m_queueFamilyIndexMappings(std::move(queueFamilyIndexMappings)),
          m_queueInUseMask(m_queueFamilyIndexMappings.size(), 0),
          m_pSemaphoreOwnershipTracker(std::make_unique<SemaphoreOwnershipTracker>(*this)){

}

VkDevice vul::Device::get() const {
    return m_handle;
}

vul::Queue vul::Device::getQueue(std::uint32_t index) const {

    VkQueue queue;
    vkGetDeviceQueue(m_handle,
                     m_queueFamilyIndexMappings[index].queueFamilyIndex,
                     m_queueFamilyIndexMappings[index].queueIndex,
                     &queue);

    return {*this, queue};
}

std::optional<vul::Queue> vul::Device::getQueueAt(std::uint32_t index) {
    if (index < m_queueFamilyIndexMappings.size()) {
        if (!m_queueInUseMask[index]) {
            m_queueInUseMask[index] = 1u;
            std::optional<vul::Queue> test;
            test.emplace(getQueue(index));
            return test;
        }
    }
    return std::nullopt;
}

vul::Device::~Device() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDevice(m_handle, m_pAllocator);
    }
}

vul::Device::Device(vul::Device &&rhs) noexcept {
    using std::swap;
    swap(m_physicalDevice, rhs.m_physicalDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_queueFamilyIndexMappings, rhs.m_queueFamilyIndexMappings);
    swap(m_queueInUseMask, rhs.m_queueInUseMask);
    swap(m_pSemaphoreOwnershipTracker, rhs.m_pSemaphoreOwnershipTracker);
}

vul::Device &vul::Device::operator=(vul::Device &&rhs) noexcept {
    using std::swap;
    swap(m_physicalDevice, rhs.m_physicalDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_queueFamilyIndexMappings, rhs.m_queueFamilyIndexMappings);
    swap(m_queueInUseMask, rhs.m_queueInUseMask);
    swap(m_pSemaphoreOwnershipTracker, rhs.m_pSemaphoreOwnershipTracker);
    return *this;
}

vul::Result vul::Device::setObjectName(const std::string &string) {
    return setObjectName(m_handle, string);
}

vul::ExpectedResult<vul::BinarySemaphore>
vul::Device::createBinarySemaphore(
        const VkAllocationCallbacks *pAllocator) const {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkSemaphore semaphore;
    auto result = static_cast<Result>(vkCreateSemaphore(m_handle,
                                                        &semaphoreInfo,
                                                        pAllocator,
                                                        &semaphore));
    return {result, vul::BinarySemaphore(*this, semaphore, pAllocator)};
}

vul::ExpectedResult<vul::TimelineSemaphore>
vul::Device::createTimelineSemaphore(std::uint64_t initialValue,
                                     const VkAllocationCallbacks *pAllocator) const {
    VkSemaphoreTypeCreateInfo timelineCreateInfo;
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.pNext = nullptr;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = initialValue;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = &timelineCreateInfo;
    semaphoreInfo.flags = 0;

    VkSemaphore semaphore;
    auto result = static_cast<Result>(vkCreateSemaphore(m_handle,
                                                        &semaphoreInfo,
                                                        pAllocator,
                                                        &semaphore));
    return {result, vul::TimelineSemaphore(*this, semaphore, pAllocator)};
}

vul::Result vul::Device::waitIdle() const {
    return static_cast<Result>(vkDeviceWaitIdle(m_handle));
}

const vul::PhysicalDevice &vul::Device::getPhysicalDevice() const {
    return m_physicalDevice;
}

vul::ExpectedResult<vul::DescriptorPool> vul::Device::createDescriptorPool(
        const TempArrayProxy<const LayoutBuilderCount> &layoutBuilders,
        vul::DescriptorPoolCreateBitMask flags,
        const void *pNext,
        const VkAllocationCallbacks *pAllocator) const {

    std::vector<VkDescriptorPoolSize> poolSizes;
    std::uint32_t setCount = 0;
    for (const auto&[builder, count]: layoutBuilders) {
        auto newPoolSizes = builder.calcPoolSizes(count);
        setCount += count;
        poolSizes.insert(poolSizes.begin(), newPoolSizes.begin(),
                         newPoolSizes.end());
    }

    return createDescriptorPool(poolSizes, setCount, flags, pNext, pAllocator);
//    VkDescriptorPoolCreateInfo poolInfo = {};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.pNext = pNext;
//    poolInfo.flags = flags.to_underlying();
//    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//    poolInfo.pPoolSizes = poolSizes.data();
//    poolInfo.maxSets = setCount;
//
//    VkDescriptorPool descriptorPool;
//    auto result = static_cast<Result>(vkCreateDescriptorPool(m_handle,
//                                                             &poolInfo,
//                                                             pAllocator,
//                                                             &descriptorPool));
//    return {result, DescriptorPool(*this, descriptorPool, pAllocator)};
}

vul::ExpectedResult<vul::DescriptorPool> vul::Device::createDescriptorPool(
        const vul::TempArrayProxy<const VkDescriptorPoolSize> &poolSizes,
        std::uint32_t maxSets, vul::DescriptorPoolCreateBitMask flags,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {


    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = pNext;
    poolInfo.flags = flags.to_underlying();
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;

    VkDescriptorPool descriptorPool;
    auto result = static_cast<Result>(vkCreateDescriptorPool(m_handle,
                                                             &poolInfo,
                                                             pAllocator,
                                                             &descriptorPool));
    return {result, DescriptorPool(*this, descriptorPool, pAllocator)};
}


vul::ExpectedResult<vul::PipelineLayout> vul::Device::createPipelineLayout(
        const vul::TempArrayProxy<const vul::DescriptorSetLayout *> &setLayouts,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {


    return createPipelineLayout(setLayouts, {}, pNext, pAllocator);
}

vul::ExpectedResult<vul::PipelineLayout> vul::Device::createPipelineLayout(
        const vul::TempArrayProxy<const vul::DescriptorSetLayout *> &setLayouts,
        const vul::TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {
    auto rawLayouts = setLayouts | ranges::views::transform(
            [](auto &layout) { return layout->get(); }) |
                      ranges::to<std::vector>();

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.flags = 0;
    createInfo.setLayoutCount = rawLayouts.size();
    createInfo.pSetLayouts = rawLayouts.data();
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();

    VkPipelineLayout pipelineLayout;
    auto result = static_cast<Result>(vkCreatePipelineLayout(m_handle,
                                                             &createInfo,
                                                             pAllocator,
                                                             &pipelineLayout));

    return {result, PipelineLayout(*this, pipelineLayout, pAllocator)};
}

vul::ExpectedResult<vul::PipelineLayout> vul::Device::createPipelineLayout(
        const TempArrayProxy<const DescriptorSetLayout> &setLayouts,
        const TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {


    auto rawLayouts = setLayouts | ranges::views::transform(
            [](auto &layout) { return layout.get(); }) |
                      ranges::to<std::vector>();

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.flags = 0;
    createInfo.setLayoutCount = rawLayouts.size();
    createInfo.pSetLayouts = rawLayouts.data();
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();

    VkPipelineLayout pipelineLayout;
    auto result = static_cast<Result>(vkCreatePipelineLayout(m_handle,
                                                             &createInfo,
                                                             pAllocator,
                                                             &pipelineLayout));

    return {result, PipelineLayout(*this, pipelineLayout, pAllocator)};
}

vul::ExpectedResult<vul::PipelineLayout> vul::Device::createPipelineLayout(
        const TempArrayProxy<const std::reference_wrapper<DescriptorSetLayout>> &setLayouts,
        const TempArrayProxy<const VkPushConstantRange> &pushConstantRanges,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {
    auto rawLayouts = setLayouts | ranges::views::transform(
            [](auto &layout) { return layout.get().get(); }) |
                      ranges::to<std::vector>();

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.flags = 0;
    createInfo.setLayoutCount = rawLayouts.size();
    createInfo.pSetLayouts = rawLayouts.data();
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();

    VkPipelineLayout pipelineLayout;
    auto result = static_cast<Result>(vkCreatePipelineLayout(m_handle,
                                                             &createInfo,
                                                             pAllocator,
                                                             &pipelineLayout));

    return {result, PipelineLayout(*this, pipelineLayout, pAllocator)};
}


vul::ExpectedResult<vul::PipelineLayout> vul::Device::createPipelineLayout(
        const vul::TempArrayProxy<const vul::DescriptorSetLayout> &setLayouts,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {
    return createPipelineLayout(setLayouts, {}, pNext, pAllocator);
}

vul::ExpectedResult<vul::PipelineLayout> vul::Device::createPipelineLayout(
        const vul::TempArrayProxy<const std::reference_wrapper<DescriptorSetLayout>> &setLayouts,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {
    return createPipelineLayout(setLayouts, {}, pNext, pAllocator);
}


vul::Result vul::Device::wait(const VkSemaphoreWaitInfo &waitInfo,
                              std::uint64_t timeout) const {
    return static_cast<Result>(vkWaitSemaphores(m_handle, &waitInfo, timeout));
}

vul::Result vul::Device::wait(
        const vul::TempArrayProxy<const TimelineSemaphore *> &semaphores,
        const vul::TempArrayProxy<const uint64_t> &values,
        std::uint64_t timeout, vul::SemaphoreWaitBitMask waitFlags,
        const void *pNext) const {
    VUL_ASSERT(semaphores.size() == values.size(),
               "Expected semaphore count to match value count");
    auto rawSemaphores = semaphores | ranges::views::transform(
            [](auto &semaphore) { return semaphore->get(); }) |
                         ranges::to<std::vector>();
    VkSemaphoreWaitInfo waitInfo;
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = pNext;
    waitInfo.flags = waitFlags.to_underlying();
    waitInfo.semaphoreCount = rawSemaphores.size();
    waitInfo.pSemaphores = rawSemaphores.data();
    waitInfo.pValues = values.data();
    return wait(waitInfo, timeout);
}

vul::Result vul::Device::wait(
        const vul::TempArrayProxy<const std::reference_wrapper<TimelineSemaphore>> &semaphores,
        const vul::TempArrayProxy<const uint64_t> &values,
        std::uint64_t timeout, vul::SemaphoreWaitBitMask waitFlags,
        const void *pNext) const {
    VUL_ASSERT(semaphores.size() == values.size(),
               "Expected semaphore count to match value count");
    auto rawSemaphores = semaphores | ranges::views::transform(
            [](auto &semaphore) { return semaphore.get().get(); }) |
                         ranges::to<std::vector>();
    VkSemaphoreWaitInfo waitInfo;
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = pNext;
    waitInfo.flags = waitFlags.to_underlying();
    waitInfo.semaphoreCount = rawSemaphores.size();
    waitInfo.pSemaphores = rawSemaphores.data();
    waitInfo.pValues = values.data();
    return wait(waitInfo, timeout);
}

vul::ExpectedResult<vul::ShaderModule> vul::Device::createShaderModule(
        const vul::TempArrayProxy<const uint32_t> &code,
        const void *pNext, const VkAllocationCallbacks *pAllocator) const {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.codeSize = code.size() * 4;
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    auto result = static_cast<Result>(vkCreateShaderModule(m_handle,
                                                           &createInfo,
                                                           pAllocator,
                                                           &shaderModule));

    return {result, ShaderModule(*this, shaderModule, pAllocator)};
}

vul::ExpectedResult<vul::CommandPool>
vul::Device::createCommandPool(std::uint32_t queueFamilyIndex,
                               vul::CommandPoolCreateBitMask flags,
                               const void *pNext,
                               const VkAllocationCallbacks *pAllocator) const {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = pNext;
    poolInfo.flags = flags.to_underlying();
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool commandPool;
    auto result = static_cast<Result>(vkCreateCommandPool(m_handle, &poolInfo,
                                                          pAllocator,
                                                          &commandPool));

    return {result, CommandPool(*this, commandPool, pAllocator)};
}

vul::ExpectedResult<vul::Framebuffer>
vul::Device::createFramebuffer(const vul::RenderPass &renderPass,
                               const vul::TempArrayProxy<const std::reference_wrapper<ImageView>> &imageViews,
                               VkExtent2D widthHeight, std::uint32_t layers,
                               vul::FramebufferCreateBitMask flags,
                               const void *pNext,
                               const VkAllocationCallbacks *pAllocator) const {
    auto rawImageViews = imageViews | ranges::views::transform(
            [](auto &imageView) { return imageView.get().get(); }) |
                         ranges::to<std::vector>();
    return createFramebuffer(renderPass, rawImageViews, widthHeight, layers,
                             flags, pNext, pAllocator);
}

vul::ExpectedResult<vul::Framebuffer>
vul::Device::createFramebuffer(const vul::RenderPass &renderPass,
                               const vul::TempArrayProxy<const vul::ImageView> &imageViews,
                               VkExtent2D widthHeight, std::uint32_t layers,
                               vul::FramebufferCreateBitMask flags,
                               const void *pNext,
                               const VkAllocationCallbacks *pAllocator) const {
    auto rawImageViews = imageViews | ranges::views::transform(
            [](auto &imageView) { return imageView.get(); }) |
                         ranges::to<std::vector>();
    return createFramebuffer(renderPass, rawImageViews, widthHeight, layers,
                             flags, pNext, pAllocator);
}

vul::ExpectedResult<vul::Framebuffer>
vul::Device::createFramebuffer(const vul::RenderPass &renderPass,
                               const vul::TempArrayProxy<const vul::ImageView *> &imageViews,
                               VkExtent2D widthHeight, std::uint32_t layers,
                               vul::FramebufferCreateBitMask flags,
                               const void *pNext,
                               const VkAllocationCallbacks *pAllocator) const {
    auto rawImageViews = imageViews | ranges::views::transform(
            [](auto &imageView) { return imageView->get(); }) |
                         ranges::to<std::vector>();
    return createFramebuffer(renderPass, rawImageViews, widthHeight, layers,
                             flags, pNext, pAllocator);
}

vul::ExpectedResult<vul::Framebuffer>
vul::Device::createFramebuffer(const vul::RenderPass &renderPass,
                               const vul::TempArrayProxy<VkImageView> &imageViews,
                               VkExtent2D widthHeight, std::uint32_t layers,
                               vul::FramebufferCreateBitMask flags,
                               const void *pNext,
                               const VkAllocationCallbacks *pAllocator) const {
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.flags = flags.to_underlying();
    createInfo.renderPass = renderPass.get();
    createInfo.attachmentCount = imageViews.size();
    createInfo.pAttachments = imageViews.data();
    createInfo.width = widthHeight.width;
    createInfo.height = widthHeight.height;
    createInfo.layers = layers;

    VkFramebuffer framebuffer;
    auto result = static_cast<Result>(vkCreateFramebuffer(m_handle,
                                                          &createInfo,
                                                          pAllocator,
                                                          &framebuffer));

    return {result, Framebuffer(*this, framebuffer, pAllocator)};
}

void vul::Device::updateDescriptorSets(
        const vul::TempArrayProxy<const VkWriteDescriptorSet> &descriptorWrites) const {
    return updateDescriptorSets(descriptorWrites, {});
}

void vul::Device::updateDescriptorSets(
        const vul::TempArrayProxy<const VkWriteDescriptorSet> &descriptorWrites,
        const vul::TempArrayProxy<const VkCopyDescriptorSet> &descriptorCopies) const {
    vkUpdateDescriptorSets(m_handle, descriptorWrites.size(),
                           descriptorWrites.data(), descriptorCopies.size(),
                           descriptorCopies.data());
}

vul::SemaphoreOwnershipTracker &
vul::Device::getSemaphoreOwnershipTracker() const {
    return *m_pSemaphoreOwnershipTracker;
}


