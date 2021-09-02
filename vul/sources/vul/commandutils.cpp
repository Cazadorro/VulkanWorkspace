//
// Created by Shae Bolt on 9/1/2021.
//

#include "commandutils.h"
#include "vul/semaphore.h"
#include "vul/device.h"
#include "vul/buffer.h"
#include "vul/commandpool.h"
#include "vul/commandbuffer.h"
#include "vul/queue.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"
#include "vul/bitmasks.h"
#include "vul/temparrayproxy.h"

vul::Result vul::copy(const Buffer &src, Buffer &dst, CommandPool &commandPool,
               Queue &queue) {
    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();
    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    commandBuffer.copyBuffer(src, dst);
    commandBuffer.end();
    const auto& device =src.getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if(!expectedSemaphore.hasValue()){
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);

    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue, vul::PipelineStageFlagBits2KHR::AllTransferBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    VkSubmitInfo2KHR submitInfo = {};
    submitInfo.pNext = nullptr; // All 3 struct above are built into VkSubmitInfo2KHR
    submitInfo.flags = 0; // VK_SUBMIT_PROTECTED_BIT_KHR also can be zero, replaces VkProtectedSubmitInfo
    submitInfo.waitSemaphoreInfoCount = 0;
    submitInfo.pWaitSemaphoreInfos = nullptr;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferInfo;
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &signalInfo;

    auto result = queue.submit(submitInfo);
    if(result != vul::Result::Success){
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}