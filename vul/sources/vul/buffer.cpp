//
// Created by Shae Bolt on 8/31/2021.
//

#include "buffer.h"
#include "vul/vmaallocation.h"
#include "vul/vmaallocator.h"
#include "vul/device.h"
#include "vul/bitmasks.h"
vul::Buffer::Buffer(vul::VmaAllocation && allocation, VkBuffer handle,
                    VkDeviceSize size) : m_allocation(std::move(allocation)), m_handle(handle), m_size(size) {

}
vul::Buffer::~Buffer() {
    if (m_allocation.get() != VK_NULL_HANDLE) {
        m_allocation.unmapMemory();
        vmaDestroyBuffer(m_allocation.getAllocator().get(), m_handle, m_allocation.get());
    }
}

vul::Buffer::Buffer(vul::Buffer &&rhs) noexcept {
using std::swap;
swap(m_allocation, rhs.m_allocation);
swap(m_handle, rhs.m_handle);
swap(m_size, rhs.m_size);
}

vul::Buffer &vul::Buffer::operator=(vul::Buffer &&rhs) noexcept {
    using std::swap;
    swap(m_allocation, rhs.m_allocation);
    swap(m_handle, rhs.m_handle);
    swap(m_size, rhs.m_size);
    return *this;
}

const VkBuffer &vul::Buffer::get() const {
    return m_handle;
}

const vul::Device &vul::Buffer::getDevice() const {
    return m_allocation.getAllocator().getDevice();
}



const VkDeviceSize &vul::Buffer::size() const {
    return m_size;
}

vul::Result vul::Buffer::setDebugObjectName(const std::string &name) {
    auto device = m_allocation.getAllocator().getVkDevice();
    auto objectNameInfo = vul::createObjectNameInfo(m_handle, name);
    auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
    if (func != nullptr) {
        return static_cast<Result>(func(device, &objectNameInfo));
    } else {
        return Result::ErrorExtensionNotPresent;
    }
}

VkBufferCopy vul::Buffer::genFullCopyRegion() const {
    return VkBufferCopy{0,0,m_size};
}

vul::VmaAllocation &vul::Buffer::getAllocation() {
    return m_allocation;
}

const vul::VmaAllocation &vul::Buffer::getAllocation() const {
    return m_allocation;
}

VkDescriptorBufferInfo
vul::Buffer::createDescriptorInfo(VkDeviceSize offset, VkDeviceSize range) const {
    return  {m_handle, offset, range};
}

VkBufferMemoryBarrier2KHR
vul::Buffer::createMemoryBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                                 vul::AccessFlagBitMask srcAccessMask,
                                 vul::PipelineStageFlagBitMask dstStageMask,
                                 vul::AccessFlagBitMask dstAccessMask,
                                 VkDeviceSize offset, VkDeviceSize size,
                                 std::uint32_t srcQueueFamilyIndex,
                                 std::uint32_t dstQueueFamilyIndex,
                                 const void *pNext) const {
    VkBufferMemoryBarrier2KHR memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
    memoryBarrier.pNext = pNext;
    memoryBarrier.srcStageMask = srcStageMask.to_underlying();
    memoryBarrier.srcAccessMask = srcAccessMask.to_underlying();
    memoryBarrier.dstStageMask= dstStageMask.to_underlying();
    memoryBarrier.dstAccessMask= dstAccessMask.to_underlying();
    memoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
    memoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
    memoryBarrier.buffer = m_handle;
    memoryBarrier.offset = offset;
    memoryBarrier.size = size;
    return memoryBarrier;
}

