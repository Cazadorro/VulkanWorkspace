//
// Created by Bolt on 5/4/2023.
//

#ifndef VULKANWORKSPACE_SUBMITINFO_H
#define VULKANWORKSPACE_SUBMITINFO_H

#include "vul/enums.h"
#include "vul/bitmasks.h"
#include "vul/temparrayproxyfwd.h"
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {
    struct SubmitInfo2 {
        vul::StructureType sType = vul::StructureType::SubmitInfo2;
        const void *pNext = nullptr;
        vul::SubmitBitMask flags = {};
        std::uint32_t waitSemaphoreInfoCount = 0;
        //todo change submitinfos to be a custom struct
        const VkSemaphoreSubmitInfo *pWaitSemaphoreInfos = nullptr;
        std::uint32_t commandBufferInfoCount = 0;
        const VkCommandBufferSubmitInfo *pCommandBufferInfos = nullptr;
        std::uint32_t signalSemaphoreInfoCount = 0;
        const VkSemaphoreSubmitInfo *pSignalSemaphoreInfos = nullptr;

        SubmitInfo2() = default;


        explicit SubmitInfo2(
                const void *pNext,
                vul::SubmitBitMask flags,
                std::uint32_t waitSemaphoreInfoCount,
                const VkSemaphoreSubmitInfo *pWaitSemaphoreInfos,
                std::uint32_t commandBufferInfoCount,
                const VkCommandBufferSubmitInfo *pCommandBufferInfos,
                std::uint32_t signalSemaphoreInfoCount,
                const VkSemaphoreSubmitInfo *pSignalSemaphoreInfos
        );

        explicit SubmitInfo2(VkSubmitInfo2 submitInfo);

        explicit operator VkSubmitInfo2() const;

        [[nodiscard]]
        VkSubmitInfo2 &get();

        [[nodiscard]]
        const VkSubmitInfo2 &get() const;

        //TODO this should not be a temp array!
        SubmitInfo2& setWaitSemaphoreInfos(TempArrayProxy<const VkSemaphoreSubmitInfo>  span);
        SubmitInfo2& setCommandBufferInfos(TempArrayProxy<const VkCommandBufferSubmitInfo> span);
        SubmitInfo2& seSignalSemaphoreInfos(TempArrayProxy<const VkSemaphoreSubmitInfo> span);
    };
}
#endif //VULKANWORKSPACE_SUBMITINFO_H
