//
// Created by Bolt on 7/7/2023.
//

#ifndef VULKANWORKSPACE_MEMORYBARRIER_H
#define VULKANWORKSPACE_MEMORYBARRIER_H

#include "vul/enums.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <cstdint>
namespace vul {
    class MemoryBarrier {
    private:
        const vul::StructureType sType = vul::StructureType::MemoryBarrier2;
    public:
        const void *pNext;
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask;
        uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask;
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask;
        uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask;

        MemoryBarrier();

        MemoryBarrier(
                const void *pNext,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask
        );

        MemoryBarrier(const MemoryBarrier &rhs) noexcept;

        MemoryBarrier(MemoryBarrier &&rhs) noexcept;

        MemoryBarrier &operator=(const MemoryBarrier &rhs) noexcept;

        ~MemoryBarrier();

        operator VkMemoryBarrier2() const noexcept;

        [[nodiscard]]
        VkMemoryBarrier2 &get() noexcept;

        [[nodiscard]]
        const VkMemoryBarrier2 &get() const noexcept;

        [[nodiscard]]
        static MemoryBarrier readAfterWrite(
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask
        );

        static MemoryBarrier WriteAfterRead(
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask
        );

        [[nodiscard]]
        static MemoryBarrier readWriteAfterReadWrite(
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask
        );

        [[nodiscard]]
        static MemoryBarrier computeToComputeRAW();

        [[nodiscard]]
        static MemoryBarrier computeToComputeRWARW();

        [[nodiscard]]
        static MemoryBarrier computeToFragmentRAW();

        [[nodiscard]]
        static MemoryBarrier fragmentToComputeRAW();
    };
}
#endif //VULKANWORKSPACE_MEMORYBARRIER_H
