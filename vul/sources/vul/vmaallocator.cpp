//
// Created by Shae Bolt on 6/9/2021.
//

#include "vmaallocator.h"
#include "vul/buffer.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"
#include "vul/temparrayproxy.h"
#include "vul/tempvoidarrayproxy.h"
#include <uul/enumflags.h>

vul::ExpectedResult<vul::VmaAllocator>
vul::VmaAllocator::create(const vul::Instance &instance,
                          const vul::PhysicalDevice &physicalDevice,
                          const vul::Device &device,
                          VmaAllocatorCreateFlags flags) {
    vul::VmaAllocator allocator;
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.flags = flags;
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
    swap(m_pDevice, rhs.m_pDevice);
}

vul::VmaAllocator &
vul::VmaAllocator::operator=(vul::VmaAllocator &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
    swap(m_pDevice, rhs.m_pDevice);
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
                                              uul::EnumFlags<vul::BufferUsageFlagBits> usages) const {
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
                                       uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages) const {
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
vul::VmaAllocator::createHostDestinationBuffer(VkDeviceSize size, uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
                      | VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT
                      | VMA_ALLOCATION_CREATE_MAPPED_BIT
                      | VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    allocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | otherUsages.to_underlying();
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


    return createBuffer(allocInfo, bufferInfo);
}


vul::ExpectedResult<vul::Buffer>
vul::VmaAllocator::createDeviceBuffer(VkDeviceSize size,
                                      uul::EnumFlags<vul::BufferUsageFlagBits> usages) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = 0;
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
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
                               const ImageCreateInfo &imageInfo) const {
    VkImage image;
    ::VmaAllocation allocation;
    auto result = static_cast<Result>(vmaCreateImage(m_handle, &(imageInfo.get()), &allocInfo, &image,
                                                     &allocation, nullptr));
    static int i = 0;
    i += 1;
    m_pDevice->setObjectName(image, "CREATED_IMAGE_" + std::to_string(i));
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
vul::VmaAllocator::createDeviceImage(const ImageCreateInfo &imageInfo) const {
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = 0;
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocInfo.preferredFlags = 0;
//    allocInfo.memoryTypeBits;
//    allocInfo.pool;
//    allocInfo.pUserData;
//    allocInfo.priority;
    return createImage(allocInfo, imageInfo);
}

vul::ExpectedResult<vul::Buffer> vul::VmaAllocator::createMappedCoherentBuffer(
        const vul::TempConstVoidArrayProxy &array,
        uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages) const {
    auto expectedResult = createMappedCoherentBuffer(array.size_bytes(),
                                                     otherUsages);
    if (!expectedResult.hasValue()) {
        return expectedResult;
    }
    expectedResult.value.getAllocation().mappedCopyFrom(array);
    return expectedResult;
}

vul::ExpectedResult<vul::Buffer> vul::VmaAllocator::createStagingBuffer(
        const vul::TempConstVoidArrayProxy &array,
        uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages) const {
    auto expectedResult = createStagingBuffer(array.size_bytes(),
                                              otherUsages);
    if (!expectedResult.hasValue()) {
        return expectedResult;
    }
    expectedResult.value.mappedCopyFrom(array);
    return expectedResult;
}


vul::ExpectedResult<vul::Buffer>
vul::VmaAllocator::createDeviceBuffer(vul::CommandPool &commandPool,
                                      vul::Queue &queue,
                                      const vul::TempConstVoidArrayProxy &array,
                                      uul::EnumFlags<vul::BufferUsageFlagBits> usages) const {
    auto expectedStageBuffer = createStagingBuffer(array);
    if (!expectedStageBuffer.hasValue()) {
        return {expectedStageBuffer.result, {}};
    }
    auto stagingBuffer = std::move(expectedStageBuffer.value);
    auto expectedBuffer = createDeviceBuffer(array.size_bytes(), vul::BufferUsageFlagBits::TransferDstBit | usages);
    if (!expectedBuffer.hasValue()) {
        return {expectedBuffer.result, {}};
    }
    auto buffer = std::move(expectedBuffer.value);
    auto result = vul::copy(stagingBuffer, buffer, commandPool, queue);
    return {result, std::move(buffer)};
}


vul::ExpectedResult<vul::Image>
vul::VmaAllocator::createDeviceImageInitImpl(vul::CommandPool &commandPool, vul::Queue &queue,
                                             vul::ExpectedResult<vul::Buffer> expectedStageBuffer,
                                             const vul::ImageCreateInfo &imageInfo, uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                                             uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                             vul::ImageLayout dstLayout, std::uint32_t mipLevel) const {
    if (!expectedStageBuffer.hasValue()) {
        fmt::print(stderr, "Couldn't create staging buffer\n");
        return {expectedStageBuffer.result, {}};
    }
    auto stagingBuffer = std::move(expectedStageBuffer.value);
    auto tempImageInfo = imageInfo;
    tempImageInfo.usage |= vul::ImageUsageFlagBits::TransferDstBit;
    auto expectedImage = createDeviceImage(tempImageInfo);
    if (!expectedImage.hasValue()) {
        fmt::print(stderr, "Couldn't create image\n");
        return {expectedImage.result, {}};
    }
    auto image = std::move(expectedImage.value);
    auto result = vul::copy(stagingBuffer, image, commandPool, queue, aspectMask, dstStageMask, dstAccessMask,
                            dstLayout, mipLevel);
    if (result != Result::Success) {
        fmt::print(stderr, "Copy not successfull\n");
    }
    return {result, std::move(image)};
}


vul::ExpectedResult<vul::Image>
vul::VmaAllocator::createDeviceImage(vul::CommandPool &commandPool,
                                     vul::Queue &queue,
                                     const vul::TempConstVoidArrayProxy &array,
                                     const ImageCreateInfo &imageInfo,
                                     uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                                     uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
                                    uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                     vul::ImageLayout dstLayout,
                                     std::uint32_t mipLevel) const {
    return createDeviceImageInitImpl(commandPool, queue, createStagingBuffer(array), imageInfo, aspectMask,
                                     dstStageMask, dstAccessMask, dstLayout, mipLevel);

}


vul::ExpectedResult<vul::Image>
vul::VmaAllocator::createDeviceTexture(vul::CommandPool &commandPool,
                                       vul::Queue &queue,
                                       const vul::TempConstVoidArrayProxy &array,
                                       const ImageCreateInfo &imageInfo,
                                       std::uint32_t mipLevel) const {
    return createDeviceImage(commandPool, queue, array, imageInfo, vul::ImageAspectFlagBits::ColorBit,
                             vul::PipelineStageFlagBits2::AllCommandsBit,
                             vul::AccessFlagBits2::ShaderReadBit,
                             vul::ImageLayout::ShaderReadOnlyOptimal,
                             mipLevel);
}

//vul::ExpectedResult<vul::Image>
//vul::VmaAllocator::createDeviceTextureArray(vul::CommandPool &commandPool,
//                                            vul::Queue &queue,
//                                            const vul::TempArrayProxy<vul::TempConstVoidArrayProxy> &array,
//                                            const ImageCreateInfo &imageInfo,
//                                            std::uint32_t mipLevel) const {
//    return createDeviceImageArray(commandPool, queue, array, imageInfo, vul::ImageAspectFlagBits::ColorBit,
//                                  vul::PipelineStageFlagBits2::AllCommandsBit,
//                                  vul::AccessFlagBits2::ShaderReadBit,
//                                  vul::ImageLayout::ShaderReadOnlyOptimal,
//                                  mipLevel);
//}

vul::ExpectedResult<vul::Image>
vul::VmaAllocator::createStorageImage(vul::CommandPool &commandPool,
                                      vul::Queue &queue,
                                      const vul::TempConstVoidArrayProxy &array,
                                      const ImageCreateInfo &imageInfo,
                                      std::uint32_t mipLevel) const {
    return createDeviceImage(commandPool, queue, array, imageInfo, vul::ImageAspectFlagBits::ColorBit,
                             vul::PipelineStageFlagBits2::AllCommandsBit,
                             vul::AccessFlagBits2::ShaderReadBit | vul::AccessFlagBits2::ShaderWriteBit,
                             vul::ImageLayout::General,
                             mipLevel);
}



