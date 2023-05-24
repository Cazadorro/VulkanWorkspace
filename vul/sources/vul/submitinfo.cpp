//
// Created by Bolt on 5/4/2023.
//

#include "submitinfo.h"
#include "vul/temparrayproxy.h"

vul::SubmitInfo2::SubmitInfo2(const void *pNext, uul::EnumFlags<vul::SubmitFlagBits>flags, std::uint32_t waitSemaphoreInfoCount,
                              const VkSemaphoreSubmitInfo *pWaitSemaphoreInfos, std::uint32_t commandBufferInfoCount,
                              const VkCommandBufferSubmitInfo *pCommandBufferInfos,
                              std::uint32_t signalSemaphoreInfoCount,
                              const VkSemaphoreSubmitInfo *pSignalSemaphoreInfos) :
                              pNext(pNext),
                              flags(flags),
                              waitSemaphoreInfoCount(waitSemaphoreInfoCount),
                              pWaitSemaphoreInfos(pWaitSemaphoreInfos),
                              commandBufferInfoCount(commandBufferInfoCount),
                              pCommandBufferInfos(pCommandBufferInfos),
                              signalSemaphoreInfoCount(signalSemaphoreInfoCount),
                              pSignalSemaphoreInfos(pSignalSemaphoreInfos){

}

vul::SubmitInfo2::SubmitInfo2(VkSubmitInfo2 submitInfo) {
    get() = submitInfo;
}

vul::SubmitInfo2::operator VkSubmitInfo2() const {
    return get();
}

VkSubmitInfo2 &vul::SubmitInfo2::get() {
    return *reinterpret_cast<VkSubmitInfo2 *>(this);
}

const VkSubmitInfo2 &vul::SubmitInfo2::get() const {
    return *reinterpret_cast<const VkSubmitInfo2 *>(this);
}

vul::SubmitInfo2 &vul::SubmitInfo2::setWaitSemaphoreInfos(vul::TempArrayProxy<const VkSemaphoreSubmitInfo> span) {
    waitSemaphoreInfoCount = span.size();
    pWaitSemaphoreInfos = span.data();
    return *this;
}

vul::SubmitInfo2 &vul::SubmitInfo2::setCommandBufferInfos(vul::TempArrayProxy<const VkCommandBufferSubmitInfo> span) {
    commandBufferInfoCount = span.size();
    pCommandBufferInfos = span.data();
    return *this;
}

vul::SubmitInfo2 &vul::SubmitInfo2::seSignalSemaphoreInfos(vul::TempArrayProxy<const VkSemaphoreSubmitInfo> span) {
    signalSemaphoreInfoCount = span.size();
    pSignalSemaphoreInfos = span.data();
    return *this;
}
