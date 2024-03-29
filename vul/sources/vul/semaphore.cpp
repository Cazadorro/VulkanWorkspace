//
// Created by Shae Bolt on 8/17/2021.
//

#include "vul/semaphore.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/vkassert.h"
#include "vul/enums.h"
#include "vul/bitmasks.h"
#include <vector>

vul::BinarySemaphore::BinarySemaphore(const vul::Device &device,
                                      VkSemaphore handle,
                                      const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_handle(handle), m_pAllocator(pAllocator) {
}

vul::BinarySemaphore::BinarySemaphore(vul::BinarySemaphore &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::BinarySemaphore &
vul::BinarySemaphore::operator=(vul::BinarySemaphore &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::BinarySemaphore::~BinarySemaphore() {
    if(m_handle != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

const VkSemaphore& vul::BinarySemaphore::get() const {
    return m_handle;
}

vul::Result vul::BinarySemaphore::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

VkSemaphoreSubmitInfoKHR
vul::BinarySemaphore::createSubmitInfo(vul::PipelineStageFlagBitMask stageMask,
                                       std::uint32_t deviceIndex,
                                       const void *pNext) const{
    VkSemaphoreSubmitInfoKHR submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
    submitInfo.pNext = pNext;
    submitInfo.semaphore = m_handle;
    submitInfo.value = 0;
    submitInfo.stageMask = stageMask.to_underlying();
    submitInfo.deviceIndex = deviceIndex;
    return submitInfo;
}


vul::TimelineSemaphore::TimelineSemaphore(const vul::Device &device,
                                      VkSemaphore handle,
                                      const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_handle(handle), m_pAllocator(pAllocator) {
}

vul::TimelineSemaphore::TimelineSemaphore(vul::TimelineSemaphore &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::TimelineSemaphore &
vul::TimelineSemaphore::operator=(vul::TimelineSemaphore &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::TimelineSemaphore::~TimelineSemaphore() {
    if(m_handle != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

const VkSemaphore& vul::TimelineSemaphore::get() const {
    return m_handle;
}

vul::Result vul::TimelineSemaphore::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::ExpectedResult<std::uint64_t> vul::TimelineSemaphore::getCounterValue() const {
    std::uint64_t value;
    auto result = static_cast<Result>(vkGetSemaphoreCounterValue(m_pDevice->get(), m_handle, &value));
    return {result, std::uint64_t{value}};
}

vul::Result vul::TimelineSemaphore::wait(std::uint64_t waitValue,
                                         std::uint64_t timeout_ns) {
    VkSemaphoreWaitInfo waitInfo = {};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.flags = 0;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &m_handle;
    waitInfo.pValues = &waitValue;

    return static_cast<Result>(vkWaitSemaphores(m_pDevice->get(), &waitInfo, timeout_ns));
}

vul::Result
vul::TimelineSemaphore::wait(const gsl::span<const TimelineSemaphore *const> &semaphores,
          const gsl::span<const std::uint64_t> &waitValues,
          std::uint64_t timeout_ns, SemaphoreWaitBitMask flags) {

    VUL_ASSERT(!semaphores.empty(), "Semaphore array can't be empty");
    VUL_ASSERT(semaphores.size() == waitValues.size(), "Wait values size differs from semaphore array size");

    std::vector<VkSemaphore> vkSemaphores;
    vkSemaphores.reserve(semaphores.size());
    for(const auto& semaphore : semaphores){
        vkSemaphores.push_back(semaphore->get());
    }

    VkSemaphoreWaitInfo waitInfo = {};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.flags = flags.to_underlying();
    waitInfo.semaphoreCount = static_cast<std::uint32_t>(semaphores.size());
    waitInfo.pSemaphores = vkSemaphores.data();
    waitInfo.pValues = waitValues.data();

    return static_cast<Result>(vkWaitSemaphores(semaphores[0]->m_pDevice->get(), &waitInfo, timeout_ns));
}

vul::Result
vul::TimelineSemaphore::wait(const gsl::span<const TimelineSemaphore *const> &semaphores,
          const gsl::span<const std::uint64_t> &waitValues,
          std::uint64_t timeout_ns) {
    return wait(semaphores, waitValues, timeout_ns, SemaphoreWaitBitMask{});
}

vul::Result
vul::TimelineSemaphore::waitAny(const gsl::span<const TimelineSemaphore *const> &semaphores,
             const gsl::span<const std::uint64_t> &waitValues,
             std::uint64_t timeout_ns) {
    return wait(semaphores, waitValues, timeout_ns, SemaphoreWaitFlagBits::AnyBit);
}

vul::Result vul::TimelineSemaphore::signal(std::uint64_t signalValue) {
    VkSemaphoreSignalInfoKHR signalInfo = {};
    signalInfo.sType =VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO_KHR;
    signalInfo.semaphore = m_handle;
    signalInfo.value     = signalValue;
    return static_cast<Result>(vkSignalSemaphore(m_pDevice->get(), &signalInfo));
}

VkSemaphoreSubmitInfoKHR
vul::TimelineSemaphore::createSubmitInfo(std::uint64_t value,
                                         vul::PipelineStageFlagBitMask stageMask,
                                         std::uint32_t deviceIndex,
                                         const void *pNext) const{
    VkSemaphoreSubmitInfoKHR submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
    submitInfo.pNext = pNext;
    submitInfo.semaphore = m_handle;
    submitInfo.value = value;
    submitInfo.stageMask = stageMask.to_underlying();
    submitInfo.deviceIndex = deviceIndex;
    return submitInfo;
}
