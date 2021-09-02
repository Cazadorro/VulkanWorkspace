//
// Created by Shae Bolt on 6/9/2021.
//

#include "vmaallocator.h"
#include "vul/buffer.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"

vul::ExpectedResult<vul::VmaAllocator>
vul::VmaAllocator::create(const vul::Instance &instance,
                          const vul::PhysicalDevice &physicalDevice,
                          const vul::Device &device){
    vul::VmaAllocator allocator;
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice.get();
    allocatorInfo.device = device.get();
    allocatorInfo.instance = instance.get();

    auto result = static_cast<Result>(vmaCreateAllocator(&allocatorInfo,
                                                         &allocator.m_handle));
    allocator.m_pDevice = &device;
    return {result, std::move(allocator)};
}


vul::VmaAllocator::~VmaAllocator() {
    if (m_handle != VK_NULL_HANDLE) {//not always false though?
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

::VmaAllocator vul::VmaAllocator::get() const {
    return m_handle;
}

vul::VmaAllocator::VmaAllocator(vul::VmaAllocator &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
}

vul::VmaAllocator &
vul::VmaAllocator::operator=(vul::VmaAllocator &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::ExpectedResult<vul::Buffer>
vul::VmaAllocator::createBuffer(const VmaAllocationCreateInfo &allocInfo,
                                const VkBufferCreateInfo &bufferInfo) const {

    VkBuffer buffer;
    ::VmaAllocation allocation;
    auto result = static_cast<Result>(vmaCreateBuffer(m_handle, &bufferInfo, &allocInfo, &buffer,
                    &allocation, nullptr));
    return {result, Buffer(vul::VmaAllocation(*this, allocation), buffer, bufferInfo.size)};
}
const vul::Device &vul::VmaAllocator::getDevice() const {
    return *m_pDevice;
}

vul::ExpectedResult<vul::Buffer>
vul::VmaAllocator::createMappedCoherentBuffer(VkDeviceSize size,
                                              vul::BufferUsageBitMask usages) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = 0;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    allocInfo.preferredFlags = 0;
//    allocInfo.memoryTypeBits;
//    allocInfo.pool;
//    allocInfo.pUserData;
//    allocInfo.priority;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // bufferInfo.pNext = nullptr;
    // bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = usages.to_underlying();
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // bufferInfo.queueFamilyIndexCount = 0;
    // bufferInfo.pQueueFamilyIndices = nullptr;
    
    return createBuffer(allocInfo, bufferInfo);
}


vul::ExpectedResult<vul::Buffer>
vul::VmaAllocator::createStagingBuffer(VkDeviceSize size,
                                       vul::BufferUsageBitMask otherUsages) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
                      | VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT
                      | VMA_ALLOCATION_CREATE_MAPPED_BIT
                      | VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    allocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
//    allocInfo.memoryTypeBits;
//    allocInfo.pool;
//    allocInfo.pUserData;
//    allocInfo.priority;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // bufferInfo.pNext = nullptr;
    // bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | otherUsages.to_underlying();
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // bufferInfo.queueFamilyIndexCount = 0;
    // bufferInfo.pQueueFamilyIndices = nullptr;

    return createBuffer(allocInfo, bufferInfo);
}



vul::ExpectedResult<vul::Buffer>
vul::VmaAllocator::createDeviceBuffer(VkDeviceSize size,
                                       vul::BufferUsageBitMask usages) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = 0;
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags =VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocInfo.preferredFlags = 0;
//    allocInfo.memoryTypeBits;
//    allocInfo.pool;
//    allocInfo.pUserData;
//    allocInfo.priority;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // bufferInfo.pNext = nullptr;
    // bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = usages.to_underlying();
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // bufferInfo.queueFamilyIndexCount = 0;
    // bufferInfo.pQueueFamilyIndices = nullptr;

    return createBuffer(allocInfo, bufferInfo);
}

vul::ExpectedResult<vul::Image>
vul::VmaAllocator::createImage(const VmaAllocationCreateInfo &allocInfo,
                               const VkImageCreateInfo &imageInfo) const {
    VkImage image;
    ::VmaAllocation allocation;
    auto result = static_cast<Result>(vmaCreateImage(m_handle, &imageInfo, &allocInfo, &image,
                                                      &allocation, nullptr));
    return {result, Image(vul::VmaAllocation(*this, allocation), image,
                          imageInfo.imageType,
                          imageInfo.format,
                          imageInfo.extent,
                          imageInfo.mipLevels,
                          imageInfo.arrayLayers,
                          imageInfo.samples,
                          imageInfo.tiling)};
}

vul::ExpectedResult<vul::Image>
vul::VmaAllocator::createDeviceImage(const VkImageCreateInfo &imageInfo) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = 0;
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags =VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocInfo.preferredFlags = 0;
//    allocInfo.memoryTypeBits;
//    allocInfo.pool;
//    allocInfo.pUserData;
//    allocInfo.priority;
    return createImage(allocInfo, imageInfo);
}





