//
// Created by Shae Bolt on 9/1/2021.
//

#include "vul/commandutils.h"
#include "vul/semaphore.h"
#include "vul/device.h"
#include "vul/buffer.h"
#include "vul/image.h"
#include "vul/commandpool.h"
#include "vul/commandbuffer.h"
#include "vul/queue.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"
#include "vul/temparrayproxy.h"
#include "vul/submitinfobuilder.h"
#include <uul/enumflags.h>

vul::Result vul::copy(const Buffer &src, Buffer &dst, CommandPool &commandPool,
                      Queue &queue, VkDeviceSize offset) {
    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();
    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    commandBuffer.copyBuffer(src, dst, offset);
    commandBuffer.end();
    const auto &device = commandPool.getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if (!expectedSemaphore.hasValue()) {
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);

    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue, vul::PipelineStageFlagBits2::AllTransferBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    auto result = queue.submit(vul::SubmitInfoBuilder()
                                       .commandBufferInfos(commandBufferInfo)
                                       .signalSemaphoreInfos(signalInfo)
                                       .create());
    if (result != vul::Result::Success) {
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}


vul::Result vul::copy(const Buffer &src, Buffer &dst, CommandPool &commandPool,
                      Queue &queue, std::uint32_t srcIndex, std::uint32_t dstIndex, VkDeviceSize offset) {
    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();
    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    commandBuffer.copyBuffer(src, dst, offset);
    auto memoryBarrier = dst.createMemoryBarrier(vul::PipelineStageFlagBits2::TransferBit,
                                                 vul::AccessFlagBits2::MemoryWriteBit,
                                                 vul::PipelineStageFlagBits2::AllTransferBit,
                                                 vul::AccessFlagBits2::None,
                                                 0, VK_WHOLE_SIZE, srcIndex, dstIndex);

    {
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.memoryBarrierCount = 0;
        dependencyInfo.pMemoryBarriers = nullptr;
        dependencyInfo.bufferMemoryBarrierCount = 1;
        dependencyInfo.pBufferMemoryBarriers = &memoryBarrier;
        dependencyInfo.imageMemoryBarrierCount = 0;
        dependencyInfo.pImageMemoryBarriers = nullptr;
        commandBuffer.pipelineBarrier(dependencyInfo);
    }
    commandBuffer.end();
    const auto &device = commandPool.getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if (!expectedSemaphore.hasValue()) {
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);

    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue, vul::PipelineStageFlagBits2::AllTransferBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    auto result = queue.submit(vul::SubmitInfoBuilder()
                                       .commandBufferInfos(commandBufferInfo)
                                       .signalSemaphoreInfos(signalInfo)
                                       .create());
    if (result != vul::Result::Success) {
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}

vul::Result vul::copy(const Buffer &src, Image &dst, CommandPool &commandPool, Queue &queue,
                      uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                      uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
                     uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                      vul::ImageLayout dstLayout,
                      std::uint32_t mipLevel) {


    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();
    auto toTransferBarrier = dst.createToTransferBarrier(vul::PipelineStageFlagBits2::None,
                                                         vul::AccessFlagBits2::None,
                                                         vul::ImageSubresourceRange(aspectMask));
    auto fromTransferBarrier = dst.createFromnTransferBarrier(dstStageMask,
                                                              dstAccessMask,
                                                              dstLayout,
                                                              vul::ImageSubresourceRange(aspectMask));
    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    {
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.memoryBarrierCount = 0;
        dependencyInfo.pMemoryBarriers = nullptr;
        dependencyInfo.bufferMemoryBarrierCount = 0;
        dependencyInfo.pBufferMemoryBarriers = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &toTransferBarrier;
        commandBuffer.pipelineBarrier(dependencyInfo);
    }
    commandBuffer.copyBufferToImage(src, dst, aspectMask, mipLevel, dst.getArrayLayerCount());
    {
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.memoryBarrierCount = 0;
        dependencyInfo.pMemoryBarriers = nullptr;
        dependencyInfo.bufferMemoryBarrierCount = 0;
        dependencyInfo.pBufferMemoryBarriers = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &fromTransferBarrier;
        commandBuffer.pipelineBarrier(dependencyInfo);
    }
    commandBuffer.end();
    const auto &device = commandPool.getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if (!expectedSemaphore.hasValue()) {
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);

    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue, vul::PipelineStageFlagBits2::AllTransferBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    auto result = queue.submit(vul::SubmitInfoBuilder()
                                       .commandBufferInfos(commandBufferInfo)
                                       .signalSemaphoreInfos(signalInfo)
                                       .create());
    if (result != vul::Result::Success) {
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}

vul::Result transition(const VkImageMemoryBarrier2KHR &barrier, vul::CommandPool &commandPool, vul::Queue &queue) {
    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();
    VkDependencyInfoKHR dependencyInfo = {};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependencyInfo.pNext = nullptr;
    dependencyInfo.dependencyFlags = {};
    dependencyInfo.memoryBarrierCount = 0;
    dependencyInfo.pMemoryBarriers = nullptr;
    dependencyInfo.bufferMemoryBarrierCount = 0;
    dependencyInfo.pBufferMemoryBarriers = nullptr;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;

    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    commandBuffer.pipelineBarrier(dependencyInfo);
    commandBuffer.end();
    const auto &device = commandPool.getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if (!expectedSemaphore.hasValue()) {
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);

    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue, vul::PipelineStageFlagBits2::BottomOfPipeBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();


    auto result = queue.submit(vul::SubmitInfoBuilder()
                                       .commandBufferInfos(commandBufferInfo)
                                       .signalSemaphoreInfos(signalInfo)
                                       .create());
    if (result != vul::Result::Success) {
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}

vul::Result vul::transition(vul::Image &image, vul::CommandPool &commandPool, vul::Queue &queue,
                            uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                            uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
                           uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                            vul::ImageLayout dstLayout) {
    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();

    auto transitionBarrier = image.createTransitionBarrier(dstStageMask, dstAccessMask, dstLayout,
                                                           vul::ImageSubresourceRange(aspectMask));

    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    {
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = {};
        dependencyInfo.memoryBarrierCount = 0;
        dependencyInfo.pMemoryBarriers = nullptr;
        dependencyInfo.bufferMemoryBarrierCount = 0;
        dependencyInfo.pBufferMemoryBarriers = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &transitionBarrier;
        commandBuffer.pipelineBarrier(dependencyInfo);
    }
    commandBuffer.end();
    const auto &device = commandPool.getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if (!expectedSemaphore.hasValue()) {
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);

    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue, vul::PipelineStageFlagBits2::AllTransferBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    auto result = queue.submit(vul::SubmitInfoBuilder()
                                       .commandBufferInfos(commandBufferInfo)
                                       .signalSemaphoreInfos(signalInfo)
                                       .create());
    if (result != vul::Result::Success) {
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}