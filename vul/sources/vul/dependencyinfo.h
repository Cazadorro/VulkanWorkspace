//
// Created by Bolt on 7/7/2023.
//

#ifndef VULKANWORKSPACE_DEPENDENCYINFO_H
#define VULKANWORKSPACE_DEPENDENCYINFO_H
#include "vul/enums.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <span>
namespace vul{
//    typedef struct VkDependencyInfo {
//        VkStructureType                  sType;
//        const void*                      pNext;
//        VkDependencyFlags                dependencyFlags;
//        uint32_t                         memoryBarrierCount;
//        const VkMemoryBarrier2*          pMemoryBarriers;
//        uint32_t                         bufferMemoryBarrierCount;
//        const VkBufferMemoryBarrier2*    pBufferMemoryBarriers;
//        uint32_t                         imageMemoryBarrierCount;
//        const VkImageMemoryBarrier2*     pImageMemoryBarriers;
//    } VkDependencyInfo;

    class MemoryBarrier;
    class BufferMemoryBarrier;
    class ImageMemoryBarrier;
    class DependencyInfo {
    private:
        const vul::StructureType sType = vul::StructureType::DependencyInfo;
    public:
        const void *pNext;
        uul::EnumFlags<vul::DependencyFlagBits> dependencyFlags;
        std::uint32_t memoryBarrierCount;
        const MemoryBarrier*          pMemoryBarriers;
        uint32_t                      bufferMemoryBarrierCount;
        const BufferMemoryBarrier*    pBufferMemoryBarriers;
        uint32_t                      imageMemoryBarrierCount;
        const ImageMemoryBarrier*     pImageMemoryBarriers;

        DependencyInfo();

        DependencyInfo(
                std::span<const MemoryBarrier> memoryBarriers,
                std::span<const BufferMemoryBarrier> bufferMemoryBarriers,
                std::span<const ImageMemoryBarrier> imageMemoryBarriers,
                uul::EnumFlags<vul::DependencyFlagBits> dependencyFlags = {},
                const void *pNext = nullptr
        );

        DependencyInfo(const DependencyInfo &rhs) noexcept;

        DependencyInfo(DependencyInfo &&rhs) noexcept;

        DependencyInfo &operator=(const DependencyInfo &rhs) noexcept;

        ~DependencyInfo();

        explicit operator VkDependencyInfo() const noexcept;

        [[nodiscard]]
        VkDependencyInfo &get() noexcept;

        [[nodiscard]]
        const VkDependencyInfo &get() const noexcept;
    };


}
#endif //VULKANWORKSPACE_DEPENDENCYINFO_H
