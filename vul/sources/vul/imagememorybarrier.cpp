//
// Created by Bolt on 7/7/2023.
//

#include "vul/imagememorybarrier.h"

vul::ImageMemoryBarrier::ImageMemoryBarrier() :
        pNext(nullptr),
        srcStageMask(0),
        srcAccessMask(0),
        dstStageMask(0),
        dstAccessMask(0),
        oldLayout(ImageLayout{}),
        newLayout(ImageLayout{}),
        srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
        dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
        image(VK_NULL_HANDLE),
        subresourceRange(vul::ImageSubresourceRange{}) {

}

vul::ImageMemoryBarrier::ImageMemoryBarrier(const void *pNext, uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                            uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                                            uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                            uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                            vul::ImageLayout oldLayout, vul::ImageLayout newLayout,
                                            std::uint32_t srcQueueFamilyIndex, std::uint32_t dstQueueFamilyIndex,
                                            VkImage image, vul::ImageSubresourceRange subresourceRange) :
        pNext(pNext),
        srcStageMask(srcStageMask),
        srcAccessMask(srcAccessMask),
        dstStageMask(dstStageMask),
        dstAccessMask(dstAccessMask),
        oldLayout(oldLayout),
        newLayout(newLayout),
        srcQueueFamilyIndex(srcQueueFamilyIndex),
        dstQueueFamilyIndex(dstQueueFamilyIndex),
        image(image),
        subresourceRange(subresourceRange) {

}

vul::ImageMemoryBarrier::ImageMemoryBarrier(const vul::ImageMemoryBarrier &rhs) noexcept:
        pNext(rhs.pNext),
        srcStageMask(rhs.srcStageMask),
        srcAccessMask(rhs.srcAccessMask),
        dstStageMask(rhs.dstStageMask),
        dstAccessMask(rhs.dstAccessMask),
        oldLayout(rhs.oldLayout),
        newLayout(rhs.newLayout),
        srcQueueFamilyIndex(rhs.srcQueueFamilyIndex),
        dstQueueFamilyIndex(rhs.dstQueueFamilyIndex),
        image(rhs.image),
        subresourceRange(rhs.subresourceRange) {

}

vul::ImageMemoryBarrier::ImageMemoryBarrier(vul::ImageMemoryBarrier &&rhs) noexcept:
        pNext(rhs.pNext),
        srcStageMask(rhs.srcStageMask),
        srcAccessMask(rhs.srcAccessMask),
        dstStageMask(rhs.dstStageMask),
        dstAccessMask(rhs.dstAccessMask),
        oldLayout(rhs.oldLayout),
        newLayout(rhs.newLayout),
        srcQueueFamilyIndex(rhs.srcQueueFamilyIndex),
        dstQueueFamilyIndex(rhs.dstQueueFamilyIndex),
        image(rhs.image),
        subresourceRange(rhs.subresourceRange) {

}

vul::ImageMemoryBarrier &vul::ImageMemoryBarrier::operator=(const vul::ImageMemoryBarrier &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        srcStageMask = rhs.srcStageMask;
        srcAccessMask = rhs.srcAccessMask;
        dstStageMask = rhs.dstStageMask;
        dstAccessMask = rhs.dstAccessMask;
        oldLayout = rhs.oldLayout;
        newLayout = rhs.newLayout;
        srcQueueFamilyIndex = rhs.srcQueueFamilyIndex;
        dstQueueFamilyIndex = rhs.dstQueueFamilyIndex;
        image = rhs.image;
        subresourceRange = rhs.subresourceRange;
    }
    return *this;
}

vul::ImageMemoryBarrier::~ImageMemoryBarrier() = default;

vul::ImageMemoryBarrier::operator VkImageMemoryBarrier2() const noexcept {
    return get();
}

VkImageMemoryBarrier2 &vul::ImageMemoryBarrier::get() noexcept {
    return *reinterpret_cast<VkImageMemoryBarrier2 *>(this);
}

const VkImageMemoryBarrier2 &vul::ImageMemoryBarrier::get() const noexcept {
    return *reinterpret_cast<const VkImageMemoryBarrier2 *>(this);
}

vul::ImageMemoryBarrier vul::ImageMemoryBarrier::fromImage(
        const vul::Image &image,
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
        uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
        uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
        vul::ImageLayout oldLayout, vul::ImageLayout newLayout,
        std::uint32_t srcQueueFamilyIndex,
        std::uint32_t dstQueueFamilyIndex,
        const void *pNext) {
    vul::ImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.pNext = pNext;
    memoryBarrier.srcStageMask = srcStageMask;
    memoryBarrier.srcAccessMask = srcAccessMask;
    memoryBarrier.dstStageMask = dstStageMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    memoryBarrier.oldLayout = oldLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
    memoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
    memoryBarrier.image = image.get();
    memoryBarrier.subresourceRange = vul::ImageSubresourceRange(image.calcImageAspect());
    return memoryBarrier;
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::readAfterWrite(const vul::Image &image,
                                        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                        vul::ImageLayout oldLayout, vul::ImageLayout newLayout) {
    return fromImage(image,
                     srcStageMask,
                     vul::AccessFlagBits2::ShaderWriteBit,
                     dstStageMask,
                     vul::AccessFlagBits2::ShaderReadBit,
                     oldLayout, newLayout);

}

vul::ImageMemoryBarrier vul::ImageMemoryBarrier::WriteAfterRead(const vul::Image &image,
                                                                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                                                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                                                vul::ImageLayout oldLayout,
                                                                vul::ImageLayout newLayout) {
    return fromImage(image,
                     srcStageMask,
                     {},
                     dstStageMask,
                     {},
                     oldLayout, newLayout);
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::readWriteAfterReadWrite(
        const vul::Image &image, uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
        vul::ImageLayout oldLayout, vul::ImageLayout newLayout) {
    return fromImage(image,
                     srcStageMask,
                     vul::AccessFlagBits2::ShaderWriteBit | vul::AccessFlagBits2::ShaderReadBit,
                     dstStageMask,
                     vul::AccessFlagBits2::ShaderWriteBit | vul::AccessFlagBits2::ShaderReadBit,
                     oldLayout, newLayout);
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::computeToComputeRAW(const vul::Image &image, vul::ImageLayout oldLayout,
                                             vul::ImageLayout newLayout) {
    return readAfterWrite(image, vul::PipelineStageFlagBits2::ComputeShaderBit,
                          vul::PipelineStageFlagBits2::ComputeShaderBit, oldLayout, newLayout);
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::computeToComputeRWARW(const vul::Image &image, vul::ImageLayout oldLayout,
                                               vul::ImageLayout newLayout) {
    return readWriteAfterReadWrite(image, vul::PipelineStageFlagBits2::ComputeShaderBit,
                                   vul::PipelineStageFlagBits2::ComputeShaderBit, oldLayout, newLayout);
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::computeToFragmentRAW(const vul::Image &image, vul::ImageLayout oldLayout,
                                              vul::ImageLayout newLayout) {
    return readAfterWrite(image, vul::PipelineStageFlagBits2::ComputeShaderBit,
                          vul::PipelineStageFlagBits2::FragmentShaderBit, oldLayout, newLayout);
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::fragmentToComputeRAW(const vul::Image &image, vul::ImageLayout oldLayout,
                                              vul::ImageLayout newLayout) {
    return readAfterWrite(image, vul::PipelineStageFlagBits2::FragmentShaderBit,
                          vul::PipelineStageFlagBits2::ComputeShaderBit, oldLayout, newLayout);
}

vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::layoutTransition(const vul::Image &image,
                                          uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                          uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                          uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                          vul::ImageLayout oldLayout, vul::ImageLayout newLayout) {
    return fromImage(image, srcStageMask, {}, dstStageMask, dstAccessMask, oldLayout, newLayout);
}


vul::ImageMemoryBarrier
vul::ImageMemoryBarrier::unfilledLayoutTransition(uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                                  uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                                  uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                                  vul::ImageLayout oldLayout, vul::ImageLayout newLayout) {
    vul::ImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcStageMask = srcStageMask;
    memoryBarrier.srcAccessMask = {};
    memoryBarrier.dstStageMask = dstStageMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    memoryBarrier.oldLayout = oldLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.srcQueueFamilyIndex = 0;
    memoryBarrier.dstQueueFamilyIndex = 0;
    memoryBarrier.image = VK_NULL_HANDLE;
    memoryBarrier.subresourceRange = {};
    return memoryBarrier;
}

void vul::ImageMemoryBarrier::setImage(const vul::Image &image_t) {
    image = image_t.get();
    subresourceRange = vul::ImageSubresourceRange(image_t.calcImageAspect());
}







