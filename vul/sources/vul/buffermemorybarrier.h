//
// Created by Bolt on 7/7/2023.
//

#ifndef VULKANWORKSPACE_BUFFERMEMORYBARRIER_H
#define VULKANWORKSPACE_BUFFERMEMORYBARRIER_H


#include "vul/enums.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {
//typedef struct VkBufferMemoryBarrier2 {
//    VkStructureType          sType;
//    const void*              pNext;
//    VkPipelineStageFlags2    srcStageMask;
//    VkAccessFlags2           srcAccessMask;
//    VkPipelineStageFlags2    dstStageMask;
//    VkAccessFlags2           dstAccessMask;
//    uint32_t                 srcQueueFamilyIndex;
//    uint32_t                 dstQueueFamilyIndex;
//    VkBuffer                 buffer;
//    VkDeviceSize             offset;
//    VkDeviceSize             size;
//} VkBufferMemoryBarrier2;
    class Buffer;

    class BufferMemoryBarrier {
    private:
        const vul::StructureType sType = vul::StructureType::BufferMemoryBarrier2;
    public:
        const void *pNext;
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask;
        uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask;
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask;
        uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask;
        std::uint32_t srcQueueFamilyIndex;
        std::uint32_t dstQueueFamilyIndex;
        VkBuffer buffer;
        VkDeviceSize offset;
        VkDeviceSize size;

        BufferMemoryBarrier();

        BufferMemoryBarrier(const void *pNext,
                            uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                            uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                            uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                            uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                            std::uint32_t srcQueueFamilyIndex,
                            std::uint32_t dstQueueFamilyIndex,
                            VkBuffer buffer,
                            VkDeviceSize offset,
                            VkDeviceSize size);


        BufferMemoryBarrier(const BufferMemoryBarrier &rhs) noexcept;

        BufferMemoryBarrier(BufferMemoryBarrier &&rhs) noexcept;

        BufferMemoryBarrier &operator=(const BufferMemoryBarrier &rhs) noexcept;

        ~BufferMemoryBarrier();

        operator VkBufferMemoryBarrier2() const noexcept;

        [[nodiscard]]
        VkBufferMemoryBarrier2 &get() noexcept;

        [[nodiscard]]
        const VkBufferMemoryBarrier2 &get() const noexcept;

        [[nodiscard]]
        static BufferMemoryBarrier fromBuffer(
                const vul::Buffer &buffer,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                std::uint32_t srcQueueFamilyIndex = 0,
                std::uint32_t dstQueueFamilyIndex = 0,
                const void *pNext = nullptr
        );

        [[nodiscard]]
        static BufferMemoryBarrier readAfterWrite(
                const vul::Buffer &buffer,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask
        );

        [[nodiscard]]
        static BufferMemoryBarrier WriteAfterRead(
                const vul::Buffer &buffer,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask
        );

        [[nodiscard]]
        static BufferMemoryBarrier readWriteAfterReadWrite(
                const vul::Buffer &buffer,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask
        );

        [[nodiscard]]
        static BufferMemoryBarrier computeToComputeRAW(
                const vul::Buffer &buffer
        );

        [[nodiscard]]
        static BufferMemoryBarrier computeToComputeRWARW(
                const vul::Buffer &buffer
        );

        [[nodiscard]]
        static BufferMemoryBarrier computeToFragmentRAW(
                const vul::Buffer &buffer
        );

    };
}
#endif //VULKANWORKSPACE_BUFFERMEMORYBARRIER_H
