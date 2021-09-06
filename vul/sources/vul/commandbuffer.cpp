//
// Created by Shae Bolt on 8/31/2021.
//

#include "vul/commandbuffer.h"
#include "vul/pipelinelayout.h"
#include "vul/graphicspipeline.h"
#include "vul/computepipeline.h"
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
#include "vul/extensionfunctions.h"

VkMemoryBarrier2KHR
vul::createMemoryBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                         vul::AccessFlagBitMask srcAccessMask,
                         vul::PipelineStageFlagBitMask dstStageMask,
                         vul::AccessFlagBitMask dstAccessMask,
                         const void *pNext) {
    VkMemoryBarrier2KHR barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
    barrier.pNext = pNext;
    barrier.srcStageMask = srcStageMask.to_underlying();
    barrier.srcAccessMask = srcAccessMask.to_underlying();
    barrier.dstStageMask = dstStageMask.to_underlying();
    barrier.dstAccessMask = dstAccessMask.to_underlying();
    return barrier;
}

VkDependencyInfoKHR vul::createDependencyInfo(
        const TempArrayProxy<VkMemoryBarrier2KHR> &memoryBarriers,
        const TempArrayProxy<VkBufferMemoryBarrier2KHR> &bufferMemoryBarriers,
        const TempArrayProxy<VkImageMemoryBarrier2KHR> &imageMemoryBarriers,
        vul::DependencyBitMask dependencyFlags, const void *pNext) {
    VkDependencyInfoKHR dependencyInfo = {};
    dependencyInfo.sType= VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependencyInfo.pNext = pNext;
    dependencyInfo.dependencyFlags = dependencyFlags.to_underlying();
    dependencyInfo.memoryBarrierCount = memoryBarriers.size();
    dependencyInfo.pMemoryBarriers = memoryBarriers.data();
    dependencyInfo.bufferMemoryBarrierCount = bufferMemoryBarriers.size();
    dependencyInfo.pBufferMemoryBarriers = bufferMemoryBarriers.data();
    dependencyInfo.imageMemoryBarrierCount = imageMemoryBarriers.size();
    dependencyInfo.pImageMemoryBarriers = imageMemoryBarriers.data();
    return dependencyInfo;
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
    auto vkCmdPipelineBarrier2KHR_f = (PFN_vkCmdPipelineBarrier2KHR) vkGetDeviceProcAddr(m_pCommandPool->getDevice().get(), "vkCmdPipelineBarrier2KHR");
    vkCmdPipelineBarrier2KHR_f(m_handle, &dependencyInfo);
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

void vul::CommandBuffer::bindPipeline(const vul::GraphicsPipeline &pipeline) {
    vkCmdBindPipeline(m_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get());
}

void vul::CommandBuffer::bindPipeline(const vul::ComputePipeline &pipeline) {
    vkCmdBindPipeline(m_handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.get());
}

void vul::CommandBuffer::bindVertexBuffers(
        const vul::TempArrayProxy<const vul::Buffer> &buffers,
        const vul::TempArrayProxy<const VkDeviceSize> &offsets,
        std::uint32_t firstBinding) {
    auto rawBuffers = buffers | ranges::views::transform([](auto &buffer) { return buffer.get(); }) | ranges::to<std::vector>();
    bindVertexBuffers(rawBuffers,offsets,firstBinding);
}

void vul::CommandBuffer::bindVertexBuffers(
        const vul::TempArrayProxy<const vul::Buffer *> &buffers,
        const vul::TempArrayProxy<const VkDeviceSize> &offsets,
        std::uint32_t firstBinding) {
    auto rawBuffers = buffers | ranges::views::transform([](auto &buffer) { return buffer->get(); }) | ranges::to<std::vector>();
    bindVertexBuffers(rawBuffers,offsets,firstBinding);
}

void vul::CommandBuffer::bindVertexBuffers(
        const vul::TempArrayProxy<const std::reference_wrapper<Buffer>> &buffers,
        const vul::TempArrayProxy<const VkDeviceSize> &offsets,
        std::uint32_t firstBinding) {
    auto rawBuffers = buffers | ranges::views::transform([](auto &buffer) { return buffer.get().get(); }) | ranges::to<std::vector>();
    bindVertexBuffers(rawBuffers,offsets,firstBinding);
}

void vul::CommandBuffer::bindVertexBuffers(
        const vul::TempArrayProxy<VkBuffer const> &buffers,
        const vul::TempArrayProxy<const VkDeviceSize> &offsets,
        std::uint32_t firstBinding) {
    VUL_ASSERT(buffers.size() == offsets.size(), "Expected buffers count to be the same as offsets");
    vkCmdBindVertexBuffers(m_handle, firstBinding, buffers.size(), buffers.data(), offsets.data());
}

void vul::CommandBuffer::bindIndexBuffer(const vul::Buffer &buffer,
                                         vul::IndexType indexType,
                                         VkDeviceSize offset) {
    vkCmdBindIndexBuffer(m_handle, buffer.get(), offset, vul::get(indexType));
}

void
vul::CommandBuffer::bindDescriptorSets(vul::PipelineBindPoint pipelineBindPoint,
                                       const vul::PipelineLayout &pipelineLayout,
                                       const vul::TempArrayProxy<VkDescriptorSet const> &descriptorSets,
                                       const vul::TempArrayProxy<const uint32_t> &dynamicOffsets,
                                       std::uint32_t firstSet) {
    vkCmdBindDescriptorSets(m_handle, vul::get(pipelineBindPoint), pipelineLayout.get(), firstSet, descriptorSets.size(), descriptorSets.data(), dynamicOffsets.size(), dynamicOffsets.data());
}

void
vul::CommandBuffer::bindDescriptorSets(vul::PipelineBindPoint pipelineBindPoint,
                                       const vul::PipelineLayout &pipelineLayout,
                                       const vul::TempArrayProxy<VkDescriptorSet const> &descriptorSets,
                                       std::uint32_t firstSet) {
    bindDescriptorSets(pipelineBindPoint, pipelineLayout, descriptorSets, {}, firstSet);
}

void vul::CommandBuffer::dispatch(std::uint32_t groupCountX,
                                  std::uint32_t groupCountY,
                                  std::uint32_t groupCountZ) {
    vkCmdDispatch(m_handle, groupCountX, groupCountY, groupCountZ);
}

void vul::CommandBuffer::dispatchIndirect(const vul::Buffer &buffer,
                                          VkDeviceSize offset) {
    vkCmdDispatchIndirect(m_handle, buffer.get(), offset);
}

void vul::CommandBuffer::setScissor(
        const vul::TempArrayProxy<const VkRect2D>& scissors,
        std::uint32_t firstScissor) {
    vkCmdSetScissor(m_handle, firstScissor, scissors.size(), scissors.data());
}

void vul::CommandBuffer::setViewport(
        const vul::TempArrayProxy<const VkViewport>& viewports,
        std::uint32_t firstViewport) {
    vkCmdSetViewport(m_handle, firstViewport, viewports.size(), viewports.data());
}

void
vul::CommandBuffer::pushConstants(const vul::PipelineLayout &pipelineLayout,
                                  vul::ShaderStageBitMask stageFlags,
                                  std::uint32_t offset, std::uint32_t size,
                                  const void *pValues) {
    vkCmdPushConstants(m_handle, pipelineLayout.get(),stageFlags.to_underlying(), offset, size, pValues);
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

void vul::SecondaryCommandBuffer::drawIndexed(std::uint32_t indexCount,
                                       std::uint32_t instanceCount,
                                       std::uint32_t firstIndex,
                                       std::int32_t vertexOffset,
                                       std::uint32_t firstInstance) {
    vkCmdDrawIndexed(m_handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void vul::SecondaryCommandBuffer::draw(std::uint32_t vertexCount,
                                std::uint32_t instanceCount,
                                std::uint32_t firstVertex,
                                std::uint32_t firstInstance) {
    vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void vul::SecondaryCommandBuffer::drawIndirect(const vul::Buffer &buffer,
                                        VkDeviceSize offset, uint32_t drawCount,
                                        uint32_t stride) {
    vkCmdDrawIndirect(m_handle, buffer.get(), offset, drawCount, stride);
}

void vul::SecondaryCommandBuffer::drawIndexedIndirect(const vul::Buffer &buffer,
                                               VkDeviceSize offset,
                                               uint32_t drawCount,
                                               uint32_t stride) {
    vkCmdDrawIndexedIndirect(m_handle, buffer.get(), offset, drawCount, stride);
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

void vul::RenderPassBlock::drawIndexed(std::uint32_t indexCount,
                                       std::uint32_t instanceCount,
                                       std::uint32_t firstIndex,
                                       std::int32_t vertexOffset,
                                       std::uint32_t firstInstance) {
    vkCmdDrawIndexed(m_handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void vul::RenderPassBlock::draw(std::uint32_t vertexCount,
                                std::uint32_t instanceCount,
                                std::uint32_t firstVertex,
                                std::uint32_t firstInstance) {
    vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void vul::RenderPassBlock::drawIndirect(const vul::Buffer &buffer,
                                        VkDeviceSize offset, uint32_t drawCount,
                                        uint32_t stride) {
    vkCmdDrawIndirect(m_handle, buffer.get(), offset, drawCount, stride);
}

void vul::RenderPassBlock::drawIndexedIndirect(const vul::Buffer &buffer,
                                               VkDeviceSize offset,
                                               uint32_t drawCount,
                                               uint32_t stride) {
    vkCmdDrawIndexedIndirect(m_handle, buffer.get(), offset, drawCount, stride);
}
