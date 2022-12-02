//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_COMMANDBUFFER_H
#define VULKANWORKSPACE_COMMANDBUFFER_H

#include "vul/enumsfwd.h"
#include "vul/bitmasks.h"
#include "vul/vkstructutils.h"
#include "vul/temparrayproxyfwd.h"
#include <vulkan/vulkan.h>
#include <string>

namespace vul {

//TODO pipline/memory barriers with synchronization 2
//TODO cmddispatch
    class PipelineLayout;

    class GraphicsPipeline;

    class ComputePipeline;

    class RenderPass;

    class Framebuffer;

    class Buffer;

    class Image;


    [[nodiscard]]
    VkMemoryBarrier2KHR
    createMemoryBarrier(vul::PipelineStage2BitMask srcStageMask,
                        vul::Access2BitMask srcAccessMask,
                        vul::PipelineStage2BitMask dstStageMask,
                        vul::Access2BitMask dstAccessMask,
                        const void *pNext = nullptr);

    [[nodiscard]]
    VkMemoryBarrier2KHR createComputeBarrierRAW(vul::PipelineStage2BitMask srcStageMaskExtra = {},
                                                vul::Access2BitMask srcAccessMaskExtra = {},
                                                vul::PipelineStage2BitMask dstStageMaskExtra = {},
                                                vul::Access2BitMask dstAccessMaskExtra = {});
    [[nodiscard]]
    VkMemoryBarrier2KHR createComputeBarrierRWARW(vul::PipelineStage2BitMask srcStageMaskExtra = {},
                                                vul::Access2BitMask srcAccessMaskExtra = {},
                                                vul::PipelineStage2BitMask dstStageMaskExtra = {},
                                                vul::Access2BitMask dstAccessMaskExtra = {});
    [[nodiscard]]
    VkMemoryBarrier2KHR createComputeFragmentBarrierRAW(vul::PipelineStage2BitMask srcStageMaskExtra = {},
                                                        vul::Access2BitMask srcAccessMaskExtra = {},
                                                        vul::PipelineStage2BitMask dstStageMaskExtra = {},
                                                        vul::Access2BitMask dstAccessMaskExtra = {});

    VkDependencyInfoKHR createDependencyInfo(const TempArrayProxy<VkMemoryBarrier2KHR>& memoryBarriers,
                                             const TempArrayProxy<VkBufferMemoryBarrier2KHR>& bufferMemoryBarriers,
                                             const TempArrayProxy<VkImageMemoryBarrier2KHR>& imageMemoryBarriers,
                                             vul::DependencyBitMask dependencyFlags = {},
                                             const void* pNext = nullptr);

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

        RenderPassBlock &operator=(const RenderPassBlock &rhs) = delete;

        void drawIndexed(std::uint32_t indexCount,
                         std::uint32_t instanceCount = 1,
                         std::uint32_t firstIndex = 0,
                         std::int32_t vertexOffset = 0,
                         std::uint32_t firstInstance = 0);

        void draw(std::uint32_t vertexCount,
                  std::uint32_t instanceCount = 1,
                  std::uint32_t firstVertex = 0,
                  std::uint32_t firstInstance = 0);

        void drawIndirect(const Buffer &buffer,
                          VkDeviceSize offset = 0,
                          uint32_t drawCount = 1,
                          uint32_t stride = 0);

        void drawIndexedIndirect(const Buffer &buffer,
                                 VkDeviceSize offset = 0,
                                 uint32_t drawCount = 1,
                                 uint32_t stride = 0);

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

        CommandBuffer &operator=(const CommandBuffer &rhs) = delete;

        Result setObjectName(const std::string &string);

        Result end();

        void copyBuffer(const Buffer &srcBuffer, Buffer &dstBuffer);

        void copyBuffer(const Buffer &srcBuffer, Buffer &dstBuffer, VkDeviceSize dstOffset);

        void copyBuffer(const Buffer &srcBuffer, Buffer &dstBuffer,
                        const TempArrayProxy<const VkBufferCopy> &copyRegions);

        void copyBufferToImage(const Buffer &srcBuffer, Image &dstImage,
                               vul::ImageAspectBitMask flags,
                               std::uint32_t mipLevel_t = 0,
                               std::uint32_t arrayLayerCount =1);

        void copyBufferToImage(const Buffer &srcBuffer, Image &dstImage,
                               const TempArrayProxy<const VkBufferImageCopy> &copyRegions);

        void pipelineBarrier(const VkDependencyInfoKHR &dependencyInfo);

        void blitImage(const Image &srcImage, Image &dstImage,
                       const TempArrayProxy<const VkImageBlit> &blitRegions);

        void bindPipeline(const GraphicsPipeline &pipeline);

        void bindPipeline(const ComputePipeline &pipeline);

        void bindVertexBuffers(const TempArrayProxy<const VkBuffer> &buffers,
                               const TempArrayProxy<const VkDeviceSize> &offsets,
                               std::uint32_t firstBinding = 0);

        void bindVertexBuffers(const TempArrayProxy<const Buffer> &buffers,
                               const TempArrayProxy<const VkDeviceSize> &offsets,
                               std::uint32_t firstBinding = 0);

        void bindVertexBuffers(const TempArrayProxy<const Buffer *> &buffers,
                               const TempArrayProxy<const VkDeviceSize> &offsets,
                               std::uint32_t firstBinding = 0);

        void bindVertexBuffers(
                const TempArrayProxy<const std::reference_wrapper<Buffer>> &buffers,
                const TempArrayProxy<const VkDeviceSize> &offsets,
                std::uint32_t firstBinding = 0);

        void bindIndexBuffer(const Buffer &buffer,
                             vul::IndexType indexType,
                             VkDeviceSize offset = 0);

        void bindDescriptorSets(vul::PipelineBindPoint pipelineBindPoint,
                                const PipelineLayout &pipelineLayout,
                                const TempArrayProxy<const VkDescriptorSet> &descriptorSets,
                                const TempArrayProxy<const uint32_t> &dynamicOffsets,
                                std::uint32_t firstSet = 0);

        void bindDescriptorSets(vul::PipelineBindPoint pipelineBindPoint,
                                const PipelineLayout &pipelineLayout,
                                const TempArrayProxy<const VkDescriptorSet> &descriptorSets,
                                std::uint32_t firstSet = 0);

        void dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY = 1, std::uint32_t groupCountZ = 1);
        void dispatchIndirect(const Buffer &buffer, VkDeviceSize offset);

        void setScissor(const TempArrayProxy<const VkRect2D>& scissors, std::uint32_t firstScissor = 0);
        void setViewport(const TempArrayProxy<const VkViewport>& viewports, std::uint32_t firstViewport = 0);
        void setScissor(const TempArrayProxy<const vul::Rect2D>& scissors, std::uint32_t firstScissor = 0);
        void setViewport(const TempArrayProxy<const vul::Viewport>& viewports, std::uint32_t firstViewport = 0);

        void pushConstants(const PipelineLayout& pipelineLayout, vul::ShaderStageBitMask stageFlags, std::uint32_t offset, std::uint32_t size, const void* pValues);
        template<typename T>
        void pushConstants(const PipelineLayout& pipelineLayout, vul::ShaderStageBitMask stageFlags, const T& values, std::uint32_t offset = 0){
            pushConstants(pipelineLayout, stageFlags, offset, static_cast<std::uint32_t>(sizeof(values)), reinterpret_cast<const void*>(&values));
        }
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

        void drawIndexed(std::uint32_t indexCount,
                         std::uint32_t instanceCount = 1,
                         std::uint32_t firstIndex = 0,
                         std::int32_t vertexOffset = 0,
                         std::uint32_t firstInstance = 0);

        void draw(std::uint32_t vertexCount,
                  std::uint32_t instanceCount = 1,
                  std::uint32_t firstVertex = 0,
                  std::uint32_t firstInstance = 0);

        void drawIndirect(const Buffer &buffer,
                          VkDeviceSize offset = 0,
                          uint32_t drawCount = 1,
                          uint32_t stride = 0);

        void drawIndexedIndirect(const Buffer &buffer,
                                 VkDeviceSize offset = 0,
                                 uint32_t drawCount = 1,
                                 uint32_t stride = 0);

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
                std::uint32_t deviceMask = 0,
                const void *pNext = nullptr) const;

    private:
    };
}
#endif //VULKANWORKSPACE_COMMANDBUFFER_H
