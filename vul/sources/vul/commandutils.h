//
// Created by Shae Bolt on 9/1/2021.
//

#ifndef VULKANWORKSPACE_COMMANDUTILS_H
#define VULKANWORKSPACE_COMMANDUTILS_H
#include "vul/enumsfwd.h"
#include <vulkan/vulkan.h>

namespace vul{
    class Queue;
    class Buffer;
    class CommandPool;

    Result copy(const Buffer& src, Buffer& dst, CommandPool& commandPool, Queue& queue);
    template<typename T>
    class TempArrayProxy;
//    const VkSemaphoreSubmitInfoKHR*        pWaitSemaphoreInfos;
//    uint32_t                               commandBufferInfoCount;
//    const VkCommandBufferSubmitInfoKHR*    pCommandBufferInfos;
//    uint32_t                               signalSemaphoreInfoCount;
//    const VkSemaphoreSubmitInfoKHR*        pSignalSemaphoreInfos;
//    VkSubmitInfo2KHR createSubmitInfo(  const TempArrayProxy<VkSemaphoreSubmitInfoKHR> VkSubmitFlagsKHR flags = 0, const void* pNext = nullptr);
}
#endif //VULKANWORKSPACE_COMMANDUTILS_H
