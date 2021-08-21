//
// Created by Shae Bolt on 6/9/2021.
//

#include "vmaallocator.h"
#include "vul/expectedresult.h"

vul::ExpectedResult<vul::VmaAllocator>
vul::VmaAllocator::create(const vul::Instance &instance,
                          const vul::PhysicalDevice &physicalDevice,
                          const vul::Device &device) {
    vul::VmaAllocator allocator;
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice.get();
    allocatorInfo.device = device.get();
    allocatorInfo.instance = instance.get();
    auto result = static_cast<Result>(vmaCreateAllocator(&allocatorInfo, &allocator.m_handle));
    return {result, std::move(allocator)};
}


vul::VmaAllocator::~VmaAllocator() {
    if(m_handle != VK_NULL_HANDLE) {//not always false though?
        vmaDestroyAllocator(m_handle);
    }
}
VmaAllocatorInfo vul::VmaAllocator::getAllocatorInfo() const {
    VmaAllocatorInfo allocatorInfo;
    vmaGetAllocatorInfo(m_handle, &allocatorInfo);
    return allocatorInfo;
}
VkDevice vul::VmaAllocator::getVkDevice() const {
    return getAllocatorInfo().device;
}

VkPhysicalDevice vul::VmaAllocator::getVkPhysicalDevice() const {
    return getAllocatorInfo().physicalDevice;
}

VkInstance vul::VmaAllocator::getVkInstance() const {
    return getAllocatorInfo().instance;
}



