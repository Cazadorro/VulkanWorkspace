//
// Created by Bolt on 5/4/2023.
//

#ifndef VULKANWORKSPACE_SUBMITINFOBUILDER_H
#define VULKANWORKSPACE_SUBMITINFOBUILDER_H
#include "vul/submitinfo.h"
#include "vul/enums.h"
#include "vul/temparrayproxyfwd.h"
#include "vul/expectedresult.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace vul{
    class Device;

    class SubmitInfoBuilder{
    public:
//        SubmitInfoBuilder() = default;

        SubmitInfoBuilder& pNext(const void* value);
        SubmitInfoBuilder& flags(uul::EnumFlags<vul::SubmitFlagBits> value);
        SubmitInfoBuilder& waitSemaphoreInfos(TempArrayProxy<const VkSemaphoreSubmitInfo> span);
        SubmitInfoBuilder& commandBufferInfos(TempArrayProxy<const VkCommandBufferSubmitInfo> span);
        SubmitInfoBuilder& signalSemaphoreInfos(TempArrayProxy<const VkSemaphoreSubmitInfo> span);

        SubmitInfoBuilder& addFlags(uul::EnumFlags<vul::SubmitFlagBits> value);

        SubmitInfoBuilder& addWaitSemaphoreInfos(TempArrayProxy<const VkSemaphoreSubmitInfo> span);
        SubmitInfoBuilder& addCommandBufferInfos(TempArrayProxy<const VkCommandBufferSubmitInfo> span);
        SubmitInfoBuilder& addSignalSemaphoreInfos(TempArrayProxy<const VkSemaphoreSubmitInfo> span);

        [[nodiscard]]
        SubmitInfo2 create() const;
    private:
        SubmitInfo2 m_submitInfo;
        std::vector<VkSemaphoreSubmitInfo> m_waitSemaphoreInfos;
        std::vector<VkCommandBufferSubmitInfo> m_commandBufferInfos;
        std::vector<VkSemaphoreSubmitInfo> m_signalSemaphoreInfos;

    };

}
#endif //VULKANWORKSPACE_SUBMITINFOBUILDER_H
