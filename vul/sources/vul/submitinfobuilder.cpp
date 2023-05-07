//
// Created by Bolt on 5/4/2023.
//

#include "submitinfobuilder.h"
#include "vul/temparrayproxy.h"


vul::SubmitInfoBuilder &vul::SubmitInfoBuilder::pNext(const void *value) {
    m_submitInfo.pNext = value;
    return *this;
}

vul::SubmitInfoBuilder &vul::SubmitInfoBuilder::flags(vul::SubmitBitMask value) {
    m_submitInfo.flags = value;
    return *this;
}

vul::SubmitInfoBuilder &
vul::SubmitInfoBuilder::waitSemaphoreInfos(vul::TempArrayProxy<const VkSemaphoreSubmitInfo> span) {
    m_waitSemaphoreInfos.assign(span.begin(), span.end());
    return *this;
}

vul::SubmitInfoBuilder &
vul::SubmitInfoBuilder::commandBufferInfos(vul::TempArrayProxy<const VkCommandBufferSubmitInfo> span) {
    m_commandBufferInfos.assign(span.begin(), span.end());
    return *this;
}

vul::SubmitInfoBuilder &
vul::SubmitInfoBuilder::signalSemaphoreInfos(vul::TempArrayProxy<const VkSemaphoreSubmitInfo> span) {
    m_signalSemaphoreInfos.assign(span.begin(), span.end());
    return *this;
}

vul::SubmitInfoBuilder &vul::SubmitInfoBuilder::addFlags(vul::SubmitBitMask value) {
    m_submitInfo.flags |= value;
    return *this;
}

vul::SubmitInfoBuilder &
vul::SubmitInfoBuilder::addWaitSemaphoreInfos(vul::TempArrayProxy<const VkSemaphoreSubmitInfo> span) {
    m_waitSemaphoreInfos.insert(m_waitSemaphoreInfos.end(), span.begin(), span.end());
    return *this;
}

vul::SubmitInfoBuilder &
vul::SubmitInfoBuilder::addCommandBufferInfos(vul::TempArrayProxy<const VkCommandBufferSubmitInfo> span) {
    m_commandBufferInfos.insert(m_commandBufferInfos.end(), span.begin(), span.end());
    return *this;
}

vul::SubmitInfoBuilder &
vul::SubmitInfoBuilder::addSignalSemaphoreInfos(vul::TempArrayProxy<const VkSemaphoreSubmitInfo> span) {
    m_signalSemaphoreInfos.insert(m_signalSemaphoreInfos.end(), span.begin(), span.end());
    return *this;
}

vul::SubmitInfo2 vul::SubmitInfoBuilder::create() const {
    auto submitInfo = m_submitInfo;

    submitInfo.setWaitSemaphoreInfos(m_waitSemaphoreInfos);
    submitInfo.setCommandBufferInfos(m_commandBufferInfos);
    submitInfo.seSignalSemaphoreInfos(m_signalSemaphoreInfos);

    return submitInfo;
}
