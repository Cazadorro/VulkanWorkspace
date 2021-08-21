//
// Created by Shae Bolt on 6/9/2021.
//

#include "vul/device.h"
#include "vul/queue.h"
#include "vul/semaphore.h"


vul::Device::Device(const vul::PhysicalDevice &physicalDevice, VkDevice handle,
                    std::vector<QueueFamilyIndexMapping> queueFamilyIndexMappings,
                    const VkAllocationCallbacks *pAllocator)
        : m_physicalDevice(physicalDevice),
          m_pAllocator(pAllocator),
          m_handle(handle),
          m_queueFamilyIndexMappings(std::move(queueFamilyIndexMappings)),
          m_queueInUseMask(m_queueFamilyIndexMappings.size(), 0) {

}

VkDevice vul::Device::get() const {
    return m_handle;
}

vul::Queue vul::Device::getQueue(std::uint32_t index) const {
    Queue queue;
    vkGetDeviceQueue(m_handle,
                     m_queueFamilyIndexMappings[index].queueFamilyIndex,
                     m_queueFamilyIndexMappings[index].queueIndex,
                     reinterpret_cast<VkQueue *>(&queue));

    return queue;
}

std::optional<vul::Queue> vul::Device::getQueueAt(std::uint32_t index) {
    if (index < m_queueFamilyIndexMappings.size()) {
        if (!m_queueInUseMask[index]) {
            m_queueInUseMask[index] = 1u;
            return getQueue(index);
        }
    }
    return std::nullopt;
}

vul::Device::~Device() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDevice(m_handle, m_pAllocator);
    }
}

vul::Device::Device(vul::Device &&rhs) noexcept {
    using std::swap;
    swap(m_physicalDevice, rhs.m_physicalDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_queueFamilyIndexMappings, rhs.m_queueFamilyIndexMappings);
    swap(m_queueInUseMask, rhs.m_queueInUseMask);
}

vul::Device &vul::Device::operator=(vul::Device &&rhs) noexcept {
    using std::swap;
    swap(m_physicalDevice, rhs.m_physicalDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_queueFamilyIndexMappings, rhs.m_queueFamilyIndexMappings);
    swap(m_queueInUseMask, rhs.m_queueInUseMask);
    return *this;
}

vul::Result vul::Device::setObjectName(const std::string &string) {
    return setObjectName(m_handle, string);
}

vul::ExpectedResult<vul::BinarySemaphore>
vul::Device::createBinarySemaphore(const VkAllocationCallbacks *pAllocator) {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkSemaphore semaphore;
    auto result = static_cast<Result>(vkCreateSemaphore(m_handle, &semaphoreInfo, pAllocator,
                      &semaphore));
    return {result, vul::BinarySemaphore(*this, semaphore, pAllocator)};
}

vul::ExpectedResult<vul::TimelineSemaphore>
vul::Device::createTimelineSemaphore(std::uint64_t initialValue,
                                     const VkAllocationCallbacks *pAllocator) {
    VkSemaphoreTypeCreateInfo timelineCreateInfo;
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.pNext = nullptr;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = initialValue;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = &timelineCreateInfo;
    semaphoreInfo.flags = 0;

    VkSemaphore semaphore;
    auto result = static_cast<Result>(vkCreateSemaphore(m_handle, &semaphoreInfo, pAllocator,
                                                        &semaphore));
    return {result, vul::TimelineSemaphore(*this, semaphore, pAllocator)};
}

vul::Result vul::Device::waitIdle() const {
    return static_cast<Result>(vkDeviceWaitIdle(m_handle));
}


