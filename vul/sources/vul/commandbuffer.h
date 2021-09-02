//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_COMMANDBUFFER_H
#define VULKANWORKSPACE_COMMANDBUFFER_H

#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include <vulkan/vulkan.h>
#include <string>

namespace vul {

//    for (size_t i = 0; i < commandBuffers.size(); i++) {
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
//    throw std::runtime_error("failed to begin recording command buffer!");
//}
//
//VkRenderPassBeginInfo renderPassInfo{};
//renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//renderPassInfo.renderPass = renderPass;
//renderPassInfo.framebuffer = swapChainFramebuffers[i];
//renderPassInfo.renderArea.offset = {0, 0};
//renderPassInfo.renderArea.extent = swapChainExtent;
//
//std::array<VkClearValue, 2> clearValues{};
//clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
//clearValues[1].depthStencil = {1.0f, 0};
//
//renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//renderPassInfo.pClearValues = clearValues.data();
//
//vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
//
//VkBuffer vertexBuffers[] = {vertexBuffer};
//VkDeviceSize offsets[] = {0};
//vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
//
//vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
//
//vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
//
//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
//
//vkCmdEndRenderPass(commandBuffers[i]);
//
//if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
//throw std::runtime_error("failed to record command buffer!");
//}
//}

//TODO Renderpass block
// vertex, descriptor sets, index, drawindexed/etc.. only possible in renderpass block
//TODO pipline/memory barriers with synchronization 2
//TODO cmddispatch
    class RenderPass;

    class Framebuffer;

    class Buffer;

    template<typename T>
    class TempArrayProxy;

    class RenderPassBlock {
    public:
        RenderPassBlock(VkCommandBuffer commandBuffer,
                        const RenderPass &renderPass,
                        const Framebuffer &framebuffer,
                        const VkRect2D &renderArea,
                        const TempArrayProxy<VkClearValue> &clearValues,
                        vul::SubpassContents subpassContents,
                        const void *pNext = nullptr);

        ~RenderPassBlock();

        RenderPassBlock(RenderPassBlock &&rhs) noexcept;

//        was noexcept?
        RenderPassBlock &operator=(RenderPassBlock &&rhs) noexcept;

        RenderPassBlock(const RenderPassBlock &rhs) = delete;

        RenderPassBlock &operator=(RenderPassBlock &rhs) = delete;

    private:
        VkCommandBuffer m_handle = VK_NULL_HANDLE;
    };

    class CommandPool;

    class CommandBuffer {
    public:
        CommandBuffer() = default;

        CommandBuffer(const CommandPool &commandPool,
                      VkCommandBuffer handle);

        [[nodiscard]]
        VkCommandBuffer get() const;

        ~CommandBuffer();

        CommandBuffer(CommandBuffer &&rhs) noexcept;

//        was noexcept?
        CommandBuffer &operator=(CommandBuffer &&rhs) noexcept;

        CommandBuffer(const CommandBuffer &rhs) = delete;

        CommandBuffer &operator=(CommandBuffer &rhs) = delete;

        Result setObjectName(const std::string &string);

        Result end();

        void copyBuffer(const Buffer &srcBuffer, Buffer &dstBuffer);

        void copyBuffer(const Buffer &srcBuffer, Buffer &dstBuffer,
                        const TempArrayProxy<const VkBufferCopy> &copyRegions);



    protected:
        const CommandPool *m_pCommandPool = nullptr;
        VkCommandBuffer m_handle = VK_NULL_HANDLE;
    };


    class SecondaryCommandBuffer : public CommandBuffer {
    public:

        using CommandBuffer::CommandBuffer;

        Result begin(vul::CommandBufferUsageBitMask flags,
                     const VkCommandBufferInheritanceInfo *pInheritanceInfo,
                     const void *pNext = nullptr);
        //apparently can only be primary
//        [[nodiscard]]
//        RenderPassBlock beginRenderPass(const RenderPass& renderPass,
//                                        const Framebuffer& framebuffer,
//                                        VkRect2D renderArea,
//                                        const TempArrayProxy<VkClearValue>& clearValues,
//                                        const void* pNext = nullptr);

    private:
    };

    class PrimaryCommandBuffer : public CommandBuffer {
    public:
        using CommandBuffer::CommandBuffer;

        Result begin(vul::CommandBufferUsageBitMask flags,
                     const void *pNext = nullptr);

        [[nodiscard]]
        RenderPassBlock beginRenderPass(const RenderPass &renderPass,
                                        const Framebuffer &framebuffer,
                                        VkRect2D renderArea,
                                        const TempArrayProxy<VkClearValue> &clearValues,
                                        const void *pNext = nullptr);

        void executeCommands(
                const TempArrayProxy<const SecondaryCommandBuffer> &secondaryCommandBuffers);

        void executeCommands(
                const TempArrayProxy<std::reference_wrapper<const SecondaryCommandBuffer>> &secondaryCommandBuffers);

        void executeCommands(
                const TempArrayProxy<SecondaryCommandBuffer *> &secondaryCommandBuffers);
        [[nodiscard]]
        VkCommandBufferSubmitInfoKHR createSubmitInfo(
                std::uint32_t deviceMask  = 0, const void * pNext = nullptr) const;
    private:
    };
}
#endif //VULKANWORKSPACE_COMMANDBUFFER_H
