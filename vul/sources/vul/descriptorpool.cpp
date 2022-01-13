//
// Created by Shae Bolt on 8/29/2021.
//

#include "descriptorpool.h"
#include "vul/descriptorsetlayout.h"
#include "vul/device.h"
#include "vul/enums.h"
#include "vul/temparrayproxy.h"
#include "vul/expectedresult.h"

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

vul::ExpectedResult<std::vector<VkDescriptorSet>>
vul::DescriptorPool::createDescriptorSets(
        const vul::TempArrayProxy<const vul::LayoutCount> &layouts,
        const void *pNext) {


    std::vector<VkDescriptorSetLayout> rawLayouts;
    for(const auto& layout : layouts){
        for(std::size_t i = 0; i < layout.count; ++i){
            rawLayouts.push_back(layout.layout.get());
        }
    }
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = pNext;
    allocInfo.descriptorPool = m_handle;
    allocInfo.descriptorSetCount = static_cast<std::uint32_t>(rawLayouts.size());
    allocInfo.pSetLayouts = rawLayouts.data();

    std::vector<VkDescriptorSet> descriptorSets(rawLayouts.size());
    auto result = static_cast<Result>(vkAllocateDescriptorSets(m_pDevice->get(), &allocInfo, descriptorSets.data()));
    return {result, std::move(descriptorSets)};
}

vul::ExpectedResult<VkDescriptorSet>
vul::DescriptorPool::createDescriptorSet(const vul::DescriptorSetLayout &layout,
                                         const void *pNext) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = pNext;
    allocInfo.descriptorPool = m_handle;
    allocInfo.descriptorSetCount =1;
    allocInfo.pSetLayouts = &(layout.get());

    VkDescriptorSet descriptorSet;
    auto result = static_cast<Result>(vkAllocateDescriptorSets(m_pDevice->get(), &allocInfo, &descriptorSet));
    return {result, std::move(descriptorSet)};
}