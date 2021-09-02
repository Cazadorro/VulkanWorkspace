//
// Created by Shae Bolt on 8/29/2021.
//

#include "descriptorpool.h"
#include "vul/device.h"
#include "vul/enums.h"

vul::DescriptorPool::DescriptorPool(const vul::Device &device,
                                    VkDescriptorPool handle,
                                    const VkAllocationCallbacks *pAllocator) :
                                    m_pDevice(&device), m_pAllocator(pAllocator), m_handle(handle){

}

VkDescriptorPool vul::DescriptorPool::get() const {
    return m_handle;
}

vul::DescriptorPool::~DescriptorPool() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::DescriptorPool::DescriptorPool(vul::DescriptorPool &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::DescriptorPool &
vul::DescriptorPool::operator=(vul::DescriptorPool &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::DescriptorPool::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}
