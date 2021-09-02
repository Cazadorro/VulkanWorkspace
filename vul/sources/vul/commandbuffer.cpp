//
// Created by Shae Bolt on 8/31/2021.
//

#include "commandbuffer.h"
#include "vul/image.h"
#include "vul/buffer.h"
#include "vul/renderpass.h"
#include "vul/framebuffer.h"
#include "vul/device.h"
#include "vul/commandpool.h"
#include "vul/enums.h"
#include "vul/bitmasks.h"
#include "vul/temparrayproxy.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>


VkMemoryBarrier2KHR
vul::createMemoryBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                         vul::AccessFlagBitMask srcAccessMask,
                         vul::PipelineStageFlagBitMask dstStageMask,
                         vul::AccessFlagBitMask dstAccessMask,
                         const void *pNext) {
    VkMemoryBarrier2KHR barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
    barrier.pNext = pNext;
    barrier.srcStageMask = srcStageMask.to_underlying();
    barrier.srcAccessMask = srcAccessMask.to_underlying();
    barrier.dstStageMask = dstStageMask.to_underlying();
    barrier.dstAccessMask = dstAccessMask.to_underlying();
    return barrier;
}

vul::CommandBuffer::CommandBuffer(const vul::CommandPool &commandPool,
                                  VkCommandBuffer handle)
        : m_pCommandPool(&commandPool), m_handle(handle) {

}

VkCommandBuffer vul::CommandBuffer::get() const {
    return m_handle;
}

vul::CommandBuffer::~CommandBuffer() {
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_pCommandPool->getDevice().get(),
                             m_pCommandPool->get(), 1, &m_handle);
    }
}

vul::CommandBuffer::CommandBuffer(vul::CommandBuffer &&rhs) noexcept {
    using std::swap;
    swap(m_pCommandPool, rhs.m_pCommandPool);
    swap(m_handle, rhs.m_handle);
}

vul::CommandBuffer &
vul::CommandBuffer::operator=(vul::CommandBuffer &&rhs) noexcept {
    using std::swap;
    swap(m_pCommandPool, rhs.m_pCommandPool);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::CommandBuffer::setObjectName(const std::string &string) {
    return m_pCommandPool->getDevice().setObjectName(m_handle, string);
}

vul::Result vul::CommandBuffer::end() {
    return static_cast<Result>(vkEndCommandBuffer(m_handle));
}

void vul::CommandBuffer::copyBuffer(const vul::Buffer &srcBuffer,
                                    vul::Buffer &dstBuffer,
                                    const vul::TempArrayProxy<const VkBufferCopy> &copyRegions) {
    vkCmdCopyBuffer(m_handle,
                    srcBuffer.get(),
                    dstBuffer.get(), copyRegions.size(),
                    copyRegions.data());
}

void vul::CommandBuffer::copyBuffer(const vul::Buffer &srcBuffer,
                                    vul::Buffer &dstBuffer) {
    copyBuffer(srcBuffer, dstBuffer, VkBufferCopy{0, 0,
                                                  std::min(srcBuffer.size(),
                                                           dstBuffer.size())});
}

void
vul::CommandBuffer::pipelineBarrier(const VkDependencyInfoKHR &dependencyInfo) {
    vkCmdPipelineBarrier2KHR(m_handle, &dependencyInfo);
}

void vul::CommandBuffer::copyBufferToImage(const vul::Buffer &srcBuffer,
                                           vul::Image &dstImage,
                                           vul::ImageAspectBitMask flags,
                                           std::uint32_t mipLevel_t) {

    VkBufferImageCopy copyRegion = {};
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.bufferImageHeight = 0;
    copyRegion.imageSubresource = vul::ImageSubresourceLayers(flags, mipLevel_t).get();
    copyRegion.imageOffset = {};
    copyRegion.imageExtent = dstImage.getExtent3D();
    copyBufferToImage(srcBuffer, dstImage, copyRegion);
}

void vul::CommandBuffer::copyBufferToImage(const vul::Buffer &srcBuffer,
                                           vul::Image &dstImage,
                                           const vul::TempArrayProxy<const VkBufferImageCopy> &copyRegions) {
    vkCmdCopyBufferToImage(m_handle,
                           srcBuffer.get(),
                           dstImage.get(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           copyRegions.size(),
                           copyRegions.data());
}

void vul::CommandBuffer::blitImage(const vul::Image &srcImage,
                                   vul::Image &dstImage,
                                   const TempArrayProxy<const VkImageBlit> &blitRegions) {
    vkCmdBlitImage(m_handle,
                   srcImage.get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage.get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   blitRegions.size(), blitRegions.data(),
                   VK_FILTER_LINEAR);
}


vul::Result
vul::PrimaryCommandBuffer::begin(vul::CommandBufferUsageBitMask flags,
                                 const void *pNext) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = pNext;
    beginInfo.flags = flags.to_underlying();
    return static_cast<Result>(vkBeginCommandBuffer(m_handle, &beginInfo));
}

vul::RenderPassBlock
vul::PrimaryCommandBuffer::beginRenderPass(const vul::RenderPass &renderPass,
                                           const vul::Framebuffer &framebuffer,
                                           VkRect2D renderArea,
                                           const vul::TempArrayProxy<VkClearValue> &clearValues,
                                           const void *pNext) {
    return vul::RenderPassBlock(m_handle, renderPass, framebuffer,
                                renderArea, clearValues,
                                vul::SubpassContents::Inline,
                                pNext);
}

void vul::PrimaryCommandBuffer::executeCommands(
        const vul::TempArrayProxy<const vul::SecondaryCommandBuffer> &secondaryCommandBuffers) {
    std::vector<VkCommandBuffer> commandBuffers = secondaryCommandBuffers |
                                                  ranges::views::transform(
                                                          [](auto &commandBuffer) { return commandBuffer.get(); }) |
                                                  ranges::to<std::vector>();
    vkCmdExecuteCommands(m_handle, commandBuffers.size(),
                         commandBuffers.data());
}

void vul::PrimaryCommandBuffer::executeCommands(
        const vul::TempArrayProxy<std::reference_wrapper<const SecondaryCommandBuffer>> &secondaryCommandBuffers) {
    std::vector<VkCommandBuffer> commandBuffers = secondaryCommandBuffers |
                                                  ranges::views::transform(
                                                          [](auto &commandBuffer) { return commandBuffer.get().get(); }) |
                                                  ranges::to<std::vector>();
    vkCmdExecuteCommands(m_handle, commandBuffers.size(),
                         commandBuffers.data());
}

void vul::PrimaryCommandBuffer::executeCommands(
        const vul::TempArrayProxy<vul::SecondaryCommandBuffer *> &secondaryCommandBuffers) {
    std::vector<VkCommandBuffer> commandBuffers = secondaryCommandBuffers |
                                                  ranges::views::transform(
                                                          [](auto &commandBuffer) { return commandBuffer->get(); }) |
                                                  ranges::to<std::vector>();
    vkCmdExecuteCommands(m_handle, commandBuffers.size(),
                         commandBuffers.data());
}

VkCommandBufferSubmitInfoKHR
vul::PrimaryCommandBuffer::createSubmitInfo(std::uint32_t deviceMask,
                                            const void *pNext) const {
    VkCommandBufferSubmitInfoKHR submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
    submitInfo.pNext = pNext;
    submitInfo.commandBuffer = m_handle;
    submitInfo.deviceMask = deviceMask;
    return submitInfo;
}

vul::Result
vul::SecondaryCommandBuffer::begin(vul::CommandBufferUsageBitMask flags,
                                   const VkCommandBufferInheritanceInfo *pInheritanceInfo,
                                   const void *pNext) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = pNext;
    beginInfo.flags = flags.to_underlying();
    beginInfo.pInheritanceInfo = pInheritanceInfo;
    return static_cast<Result>(vkBeginCommandBuffer(m_handle, &beginInfo));
}

//vul::RenderPassBlock
//vul::SecondaryCommandBuffer::beginRenderPass(const vul::RenderPass &renderPass,
//                                             const vul::Framebuffer &framebuffer,
//                                             VkRect2D renderArea,
//                                             const vul::TempArrayProxy<VkClearValue> &clearValues,
//                                             const void *pNext) {
//    return vul::RenderPassBlock(m_handle, renderPass, framebuffer,
//                                renderArea, clearValues,
//                                vul::SubpassContents::SecondaryCommandBuffers,
//                                pNext);
//}

vul::RenderPassBlock::RenderPassBlock(VkCommandBuffer commandBuffer,
                                      const RenderPass &renderPass,
                                      const Framebuffer &framebuffer,
                                      const VkRect2D &renderArea,
                                      const TempArrayProxy<VkClearValue> &clearValues,
                                      vul::SubpassContents subpassContents,
                                      const void *pNext) : m_handle(
        commandBuffer) {

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.pNext = pNext;
    renderPassInfo.renderPass = renderPass.get();
    renderPassInfo.framebuffer = framebuffer.get();
    renderPassInfo.renderArea = renderArea;
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(m_handle, &renderPassInfo, vul::get(subpassContents));
}

vul::RenderPassBlock::~RenderPassBlock() {
    if (m_handle != VK_NULL_HANDLE) {
        vkCmdEndRenderPass(m_handle);
    }
}

vul::RenderPassBlock::RenderPassBlock(vul::RenderPassBlock &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
}

vul::RenderPassBlock &
vul::RenderPassBlock::operator=(vul::RenderPassBlock &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
    return *this;
}
