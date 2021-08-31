//
// Created by Shae Bolt on 8/29/2021.
//

#include "pipelinelayout.h"
#include "vul/device.h"

vul::PipelineLayout::PipelineLayout(const vul::Device &device,
                                    VkPipelineLayout handle,
                                    const VkAllocationCallbacks *pAllocator): m_pDevice(
        &device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkPipelineLayout vul::PipelineLayout::get() const {
    return m_handle;
}

vul::PipelineLayout::~PipelineLayout() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::PipelineLayout::PipelineLayout(vul::PipelineLayout &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::PipelineLayout &
vul::PipelineLayout::operator=(vul::PipelineLayout &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::PipelineLayout::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}
