//
// Created by Shae Bolt on 6/9/2021.
//

#include "vul/queue.h"
#include "vul/device.h"
#include "vul/temparrayproxy.h"
#include "vul/enums.h"
#include "vul/extensionfunctions.h"
#include "vul/vkassert.h"
#include <range/v3/view/zip.hpp>

VkQueue vul::Queue::get() const {
    return m_handle;
}

vul::Result vul::Queue::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::Result vul::Queue::waitIdle() const {
    return static_cast<Result>(vkQueueWaitIdle(m_handle));
}

vul::Result vul::Queue::submit(const TempArrayProxy<const VkSubmitInfo2KHR>& submitInfos) const {
    auto vkQueueSubmit2KHR_f = (PFN_vkQueueSubmit2KHR) vkGetDeviceProcAddr(m_pDevice->get(), "vkQueueSubmit2KHR");
    return static_cast<Result>(vkQueueSubmit2KHR_f(m_handle, submitInfos.size(), submitInfos.data(), VK_NULL_HANDLE));
}

vul::Queue::Queue(const vul::Device &device, VkQueue handle) : m_pDevice(&device), m_handle(handle){

}

vul::Result vul::Queue::submit(
        const vul::TempArrayProxy<const VkSubmitInfo2KHR> &submitInfos,
        const vul::TempArrayProxy<const std::vector<uint64_t>> &objectHandlesList,
        const vul::TempArrayProxy<const size_t> submitInfoFinishedSemaphoreIndexList) const {
    VUL_ASSERT(submitInfos.size() == objectHandlesList.size() &&
    submitInfos.size() == submitInfoFinishedSemaphoreIndexList.size(),
    fmt::format("sizes don't match: submitInfos.size() == {}, "
                "objectHandlesList.size() == {}, "
                "submitInfoFinishedSemaphoreIndexList.size() == {}",
                submitInfos.size(),
                objectHandlesList.size(),
                submitInfoFinishedSemaphoreIndexList.size()
                ).c_str());

    for(const auto& [submitInfo, objectHandles, submitInfoFinishedSemaphoreIndex] : ranges::views::zip(submitInfos, objectHandlesList, submitInfoFinishedSemaphoreIndexList)){
        VUL_ASSERT(submitInfoFinishedSemaphoreIndex < submitInfo.signalSemaphoreInfoCount,
                   fmt::format("Trying to index into signal semaphore which doesn't exist, submitInfoFinishedSemaphoreIndex {}, signalSemaphoreInfoCount {}",
                               submitInfoFinishedSemaphoreIndex,
                               submitInfo.signalSemaphoreInfoCount).c_str());
        auto& signalSemaphoreInfo =  submitInfo.pSignalSemaphoreInfos[submitInfoFinishedSemaphoreIndex];
        //TODO try to enforce timeline semaphore! can have bugs with out it. could use timeline semaphore as input, then check each value for timelinesemaphore?
        m_pDevice->getSemaphoreOwnershipTracker().updateSemaphoreOwnership(objectHandles, reinterpret_cast<std::uintptr_t>(signalSemaphoreInfo.semaphore), signalSemaphoreInfo.value);
    }
    return submit(submitInfos);
}

