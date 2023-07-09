//
// Created by Bolt on 7/7/2023.
//

#include "vul/memorybarrier.h"

vul::MemoryBarrier::MemoryBarrier() :
        pNext(nullptr),
        srcStageMask(0),
        srcAccessMask(0),
        dstStageMask(0),
        dstAccessMask(0) {

}

vul::MemoryBarrier::MemoryBarrier(const void *pNext, uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                  uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                                  uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                  uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask) :
        pNext(pNext),
        srcStageMask(srcStageMask),
        srcAccessMask(srcAccessMask),
        dstStageMask(dstStageMask),
        dstAccessMask(dstAccessMask) {

}

vul::MemoryBarrier::MemoryBarrier(const vul::MemoryBarrier &rhs) noexcept:
        pNext(rhs.pNext),
        srcStageMask(rhs.srcStageMask),
        srcAccessMask(rhs.srcAccessMask),
        dstStageMask(rhs.dstStageMask),
        dstAccessMask(rhs.dstAccessMask) {

}

vul::MemoryBarrier::MemoryBarrier(vul::MemoryBarrier &&rhs) noexcept:
        pNext(rhs.pNext),
        srcStageMask(rhs.srcStageMask),
        srcAccessMask(rhs.srcAccessMask),
        dstStageMask(rhs.dstStageMask),
        dstAccessMask(rhs.dstAccessMask) {

}

vul::MemoryBarrier &vul::MemoryBarrier::operator=(const vul::MemoryBarrier &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        srcStageMask = rhs.srcStageMask;
        srcAccessMask = rhs.srcAccessMask;
        dstStageMask = rhs.dstStageMask;
        dstAccessMask = rhs.dstAccessMask;
    }
    return *this;
}

vul::MemoryBarrier::~MemoryBarrier() = default;

vul::MemoryBarrier::operator VkMemoryBarrier2() const noexcept {
    return get();
}

VkMemoryBarrier2 &vul::MemoryBarrier::get() noexcept {
    return *reinterpret_cast<VkMemoryBarrier2 *>(this);
}

const VkMemoryBarrier2 &vul::MemoryBarrier::get() const noexcept {
    return *reinterpret_cast<const VkMemoryBarrier2 *>(this);
}


vul::MemoryBarrier
vul::MemoryBarrier::readAfterWrite(
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask) {
    return {nullptr, srcStageMask, vul::AccessFlagBits2::ShaderWriteBit, dstStageMask,
            vul::AccessFlagBits2::ShaderReadBit};
}


vul::MemoryBarrier vul::MemoryBarrier::WriteAfterRead(uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                                      uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask) {
    return {nullptr, srcStageMask, {}, dstStageMask, {}};
}


vul::MemoryBarrier
vul::MemoryBarrier::readWriteAfterReadWrite(
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask) {
    return {nullptr, srcStageMask, vul::AccessFlagBits2::ShaderWriteBit | vul::AccessFlagBits2::ShaderReadBit,
            dstStageMask, vul::AccessFlagBits2::ShaderWriteBit | vul::AccessFlagBits2::ShaderReadBit};
}

vul::MemoryBarrier
vul::MemoryBarrier::computeToComputeRAW() {
    return readAfterWrite(vul::PipelineStageFlagBits2::ComputeShaderBit,
                          vul::PipelineStageFlagBits2::ComputeShaderBit);
}

vul::MemoryBarrier
vul::MemoryBarrier::computeToComputeRWARW() {
    return readWriteAfterReadWrite(vul::PipelineStageFlagBits2::ComputeShaderBit,
                                   vul::PipelineStageFlagBits2::ComputeShaderBit);
}

vul::MemoryBarrier
vul::MemoryBarrier::computeToFragmentRAW() {
    return readAfterWrite(vul::PipelineStageFlagBits2::ComputeShaderBit,
                          vul::PipelineStageFlagBits2::FragmentShaderBit);
}

vul::MemoryBarrier vul::MemoryBarrier::fragmentToComputeRAW() {
    return readAfterWrite(vul::PipelineStageFlagBits2::FragmentShaderBit,
                          vul::PipelineStageFlagBits2::ComputeShaderBit);
}

