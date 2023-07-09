//
// Created by Bolt on 7/7/2023.
//

#include "vul/buffermemorybarrier.h"
#include "vul/buffer.h"

vul::BufferMemoryBarrier::BufferMemoryBarrier() :
        pNext(nullptr),
        srcStageMask(0),
        srcAccessMask(0),
        dstStageMask(0),
        dstAccessMask(0),
        srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
        dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
        buffer(VK_NULL_HANDLE),
        offset(0),
        size(VK_WHOLE_SIZE) {
}

vul::BufferMemoryBarrier::BufferMemoryBarrier(const void *pNext,
                                              uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                              uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                                              uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                              uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                              std::uint32_t srcQueueFamilyIndex, std::uint32_t dstQueueFamilyIndex,
                                              VkBuffer buffer,
                                              VkDeviceSize offset,
                                              VkDeviceSize size) :
        pNext(pNext),
        srcStageMask(srcStageMask),
        srcAccessMask(srcAccessMask),
        dstStageMask(dstStageMask),
        dstAccessMask(dstAccessMask),
        srcQueueFamilyIndex(srcQueueFamilyIndex),
        dstQueueFamilyIndex(dstQueueFamilyIndex),
        buffer(buffer),
        offset(offset),
        size(size) {

}

vul::BufferMemoryBarrier::BufferMemoryBarrier(const vul::BufferMemoryBarrier &rhs) noexcept:
        pNext(rhs.pNext),
        srcStageMask(rhs.srcStageMask),
        srcAccessMask(rhs.srcAccessMask),
        dstStageMask(rhs.dstStageMask),
        dstAccessMask(rhs.dstAccessMask),
        srcQueueFamilyIndex(rhs.srcQueueFamilyIndex),
        dstQueueFamilyIndex(rhs.dstQueueFamilyIndex),
        buffer(rhs.buffer),
        offset(rhs.offset),
        size(rhs.size) {

}

vul::BufferMemoryBarrier::BufferMemoryBarrier(vul::BufferMemoryBarrier &&rhs) noexcept:
        pNext(rhs.pNext),
        srcStageMask(rhs.srcStageMask),
        srcAccessMask(rhs.srcAccessMask),
        dstStageMask(rhs.dstStageMask),
        dstAccessMask(rhs.dstAccessMask),
        srcQueueFamilyIndex(rhs.srcQueueFamilyIndex),
        dstQueueFamilyIndex(rhs.dstQueueFamilyIndex),
        buffer(rhs.buffer),
        offset(rhs.offset),
        size(rhs.size) {

}

vul::BufferMemoryBarrier &vul::BufferMemoryBarrier::operator=(const vul::BufferMemoryBarrier &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        srcStageMask = rhs.srcStageMask;
        srcAccessMask = rhs.srcAccessMask;
        dstStageMask = rhs.dstStageMask;
        dstAccessMask = rhs.dstAccessMask;
        srcQueueFamilyIndex = rhs.srcQueueFamilyIndex;
        dstQueueFamilyIndex = rhs.dstQueueFamilyIndex;
        buffer = rhs.buffer;
        offset = rhs.offset;
        size = rhs.size;
    }
    return *this;
}

vul::BufferMemoryBarrier::~BufferMemoryBarrier() = default;

vul::BufferMemoryBarrier::operator VkBufferMemoryBarrier2() const noexcept {
    return get();
}

VkBufferMemoryBarrier2 &vul::BufferMemoryBarrier::get() noexcept {
    return *reinterpret_cast<VkBufferMemoryBarrier2 *>(this);
}

const VkBufferMemoryBarrier2 &vul::BufferMemoryBarrier::get() const noexcept {
    return *reinterpret_cast<const VkBufferMemoryBarrier2 *>(this);
}

vul::BufferMemoryBarrier vul::BufferMemoryBarrier::fromBuffer(
        const vul::Buffer &buffer,
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
        uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
        uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
        std::uint32_t srcQueueFamilyIndex,
        std::uint32_t dstQueueFamilyIndex,
        const void *pNext) {
    vul::BufferMemoryBarrier memoryBarrier = {};
    memoryBarrier.pNext = pNext;
    memoryBarrier.srcStageMask = srcStageMask;
    memoryBarrier.srcAccessMask = srcAccessMask;
    memoryBarrier.dstStageMask = dstStageMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    memoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
    memoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
    memoryBarrier.buffer = buffer.get();
//    memoryBarrier.offset = 0;
//    memoryBarrier.size = VK_WHOLE_SIZE;
    return memoryBarrier;
}

vul::BufferMemoryBarrier
vul::BufferMemoryBarrier::readAfterWrite(const vul::Buffer &buffer,
                                         uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                         uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask) {
    return fromBuffer(buffer,
                      srcStageMask,
                      vul::AccessFlagBits2::ShaderWriteBit,
                      dstStageMask,
                      vul::AccessFlagBits2::ShaderReadBit);

}

vul::BufferMemoryBarrier vul::BufferMemoryBarrier::WriteAfterRead(const vul::Buffer &buffer,
                                                                  uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                                                  uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask) {
    return fromBuffer(buffer,
                      srcStageMask,
                      {},
                      dstStageMask,
                      {});
}


vul::BufferMemoryBarrier
vul::BufferMemoryBarrier::readWriteAfterReadWrite(
        const vul::Buffer &buffer, uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask) {
    return fromBuffer(buffer,
                      srcStageMask,
                      vul::AccessFlagBits2::ShaderWriteBit | vul::AccessFlagBits2::ShaderReadBit,
                      dstStageMask,
                      vul::AccessFlagBits2::ShaderWriteBit | vul::AccessFlagBits2::ShaderReadBit);
}

vul::BufferMemoryBarrier
vul::BufferMemoryBarrier::computeToComputeRAW(const vul::Buffer &buffer) {
    return readAfterWrite(buffer, vul::PipelineStageFlagBits2::ComputeShaderBit,
                          vul::PipelineStageFlagBits2::ComputeShaderBit);
}

vul::BufferMemoryBarrier
vul::BufferMemoryBarrier::computeToComputeRWARW(const vul::Buffer &buffer) {
    return readWriteAfterReadWrite(buffer, vul::PipelineStageFlagBits2::ComputeShaderBit,
                                   vul::PipelineStageFlagBits2::ComputeShaderBit);
}

vul::BufferMemoryBarrier
vul::BufferMemoryBarrier::computeToFragmentRAW(const vul::Buffer &buffer) {
    return readAfterWrite(buffer, vul::PipelineStageFlagBits2::ComputeShaderBit,
                          vul::PipelineStageFlagBits2::FragmentShaderBit);
}





