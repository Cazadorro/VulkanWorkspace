//
// Created by Shae Bolt on 9/2/2021.
//

#include "image.h"
#include "vul/imageview.h"
#include "vul/vmaallocation.h"
#include "vul/vmaallocator.h"
#include "vul/device.h"
#include "vul/expectedresult.h"

vul::ImageSubresourceRange::ImageSubresourceRange(
        vul::ImageAspectBitMask aspectMask_t, std::uint32_t baseMipLevel_t,
        std::uint32_t levelCount_t, std::uint32_t baseArrayLayer_t,
        std::uint32_t layerCount_t) : aspectMask(aspectMask_t),
                                      baseMipLevel(baseMipLevel_t),
                                      levelCount(levelCount_t),
                                      baseArrayLayer(baseArrayLayer_t) {

}

const VkImageSubresourceRange &vul::ImageSubresourceRange::get() const {
    return *reinterpret_cast<const VkImageSubresourceRange *>(this);
}

vul::ImageSubresourceLayers::ImageSubresourceLayers(
        vul::ImageAspectBitMask aspectMask_t, std::uint32_t mipLevel_t,
        std::uint32_t baseArrayLayer_t, std::uint32_t layerCount_t)
        : aspectMask(aspectMask_t),
          mipLevel(mipLevel_t),
          baseArrayLayer(baseArrayLayer_t) {

}

const VkImageSubresourceLayers &vul::ImageSubresourceLayers::get() const {
    return *reinterpret_cast<const VkImageSubresourceLayers *>(this);
}


vul::Image::~Image() {
    if (m_allocation.get() != VK_NULL_HANDLE) {
        m_allocation.unmapMemory();
        vmaDestroyImage(m_allocation.getAllocator().get(), m_handle,
                        m_allocation.get());
    }
}

const VkImage &vul::Image::get() const {
    return m_handle;
}

const vul::Device &vul::Image::getDevice() const {
    return m_allocation.getAllocator().getDevice();
}


vul::Result vul::Image::setDebugObjectName(const std::string &name) {
    return m_allocation.getAllocator().getDevice().setObjectName(m_handle,
                                                                 name);
}

vul::VmaAllocation &vul::Image::getAllocation() {
    return m_allocation;
}

const vul::VmaAllocation &vul::Image::getAllocation() const {
    return m_allocation;
}

VkImageCopy vul::Image::genFullCopyRegion(vul::ImageAspectFlagBits imageAspect,
                                          std::uint32_t mipLevel) const {
    return genFullCopyRegion(imageAspect, imageAspect, mipLevel);
}

VkImageCopy
vul::Image::genFullCopyRegion(vul::ImageAspectFlagBits imageAspectSrc,
                              vul::ImageAspectFlagBits imageAspectDst,
                              std::uint32_t mipLevel) const {
    VkImageCopy copy = {};
    copy.srcSubresource = ImageSubresourceLayers(imageAspectSrc,
                                                 mipLevel).get();
    copy.srcOffset = {};
    copy.dstSubresource = ImageSubresourceLayers(imageAspectDst,
                                                 mipLevel).get();
    copy.dstOffset = {};
    copy.extent = m_extent;
    return copy;
}



vul::Image::Image(vul::Image &&rhs) noexcept {
    using std::swap;
    swap(m_allocation, rhs.m_allocation);
    swap(m_handle, rhs.m_handle);
    swap(m_imageType, rhs.m_imageType);
    swap(m_format, rhs.m_format);
    swap(m_extent, rhs.m_extent);
    swap(m_mipLevels, rhs.m_mipLevels);
    swap(m_arrayLayers, rhs.m_arrayLayers);
    swap(m_samples, rhs.m_samples);
    swap(m_tiling, rhs.m_tiling);
}

vul::Image &vul::Image::operator=(vul::Image &&rhs) noexcept {
    using std::swap;
    swap(m_allocation, rhs.m_allocation);
    swap(m_handle, rhs.m_handle);
    swap(m_imageType, rhs.m_imageType);
    swap(m_format, rhs.m_format);
    swap(m_extent, rhs.m_extent);
    swap(m_mipLevels, rhs.m_mipLevels);
    swap(m_arrayLayers, rhs.m_arrayLayers);
    swap(m_samples, rhs.m_samples);
    swap(m_tiling, rhs.m_tiling);
    return *this;
}

vul::Image::Image(vul::VmaAllocation &&allocation, VkImage handle,
                  VkImageType imageType, VkFormat format, VkExtent3D extent,
                  uint32_t mipLevels, uint32_t arrayLayers,
                  VkSampleCountFlagBits samples, VkImageTiling tiling)
        : m_allocation(
        std::move(allocation)), m_handle(handle),
          m_imageType(imageType),
          m_format(format),
          m_extent(extent),
          m_mipLevels(mipLevels),
          m_arrayLayers(arrayLayers),
          m_samples(samples),
          m_tiling(tiling) {

}


vul::ImageType vul::Image::getImageType() const {
    return static_cast<ImageType>(m_imageType);
}

vul::Format vul::Image::getImageFormat() const {
    return static_cast<Format>(m_format);
}

VkExtent3D vul::Image::getExtent3D() const {
    return m_extent;
}

VkExtent2D vul::Image::getExtent2D() const {
    return {m_extent.width, m_extent.height};
}

std::uint32_t vul::Image::getMipLevelCount() const {
    return m_mipLevels;
}

std::uint32_t vul::Image::getArrayLayerCount() const {
    return m_arrayLayers;
}

vul::SampleCountBitMask vul::Image::getSamples() const {
    return static_cast<SampleCountBitMask>(m_samples);
}

vul::ImageTiling vul::Image::getImageTiling() const {
    return static_cast<ImageTiling>(m_tiling);
}

vul::ExpectedResult <vul::ImageView>
vul::Image::createImageView(const vul::ImageSubresourceRange &subresourceRange,
                            const VkComponentMapping &components,
                            vul::ImageViewCreateBitMask flags,
                            const void *pNext,
                            const VkAllocationCallbacks *pAllocator) const {

    return createImageView(static_cast<vul::ImageViewType>(m_imageType), subresourceRange, components, flags, pNext, pAllocator);
}

vul::ExpectedResult <vul::ImageView>
vul::Image::createImageView(vul::ImageViewType viewType,
                            const vul::ImageSubresourceRange &subresourceRange,
                            const VkComponentMapping &components,
                            vul::ImageViewCreateBitMask flags,
                            const void *pNext,
                            const VkAllocationCallbacks *pAllocator) const {
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = pNext;
    imageViewCreateInfo.flags = flags.to_underlying();
    imageViewCreateInfo.image = m_handle;
    imageViewCreateInfo.viewType = vul::get(viewType);
    imageViewCreateInfo.format = m_format;
    imageViewCreateInfo.components = components;
    imageViewCreateInfo.subresourceRange = subresourceRange.get();

    VkImageView imageView;
    auto result = static_cast<Result>(vkCreateImageView( m_allocation.getAllocator().getDevice().get(), &imageViewCreateInfo,
                                                         pAllocator,
                                                         &imageView));
    return {result, ImageView(m_allocation.getAllocator().getDevice(), imageView, pAllocator)};
}


VkImageMemoryBarrier2KHR
vul::Image::createMemoryBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                                vul::AccessFlagBitMask srcAccessMask,
                                vul::PipelineStageFlagBitMask dstStageMask,
                                vul::AccessFlagBitMask dstAccessMask,
                                vul::ImageLayout oldLayout,
                                vul::ImageLayout newLayout,
                                const vul::ImageSubresourceRange &subresourceRange,
                                std::uint32_t srcQueueFamilyIndex,
                                std::uint32_t dstQueueFamilyIndex,
                                const void *pNext) const {
    VkImageMemoryBarrier2KHR barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
    barrier.pNext = pNext;
    barrier.srcStageMask = srcAccessMask.to_underlying();
    barrier.srcAccessMask = srcAccessMask.to_underlying();
    barrier.dstStageMask = dstStageMask.to_underlying();
    barrier.dstAccessMask = dstAccessMask.to_underlying();
    barrier.oldLayout = vul::get(oldLayout);
    barrier.newLayout = vul::get(newLayout);
    barrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
    barrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
    barrier.image = m_handle;
    barrier.subresourceRange = subresourceRange.get();
    return barrier;
}

VkImageMemoryBarrier2KHR
vul::Image::createToTransferBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                                    vul::AccessFlagBitMask srcAccessMask,
                                    const vul::ImageSubresourceRange &subresourceRange,
                                    vul::ImageLayout oldLayout,
                                    std::uint32_t srcQueueFamilyIndex,
                                    std::uint32_t dstQueueFamilyIndex,
                                    const void *pNext) const {
    return createMemoryBarrier(srcStageMask, srcAccessMask,
                                   vul::PipelineStageFlagBits2KHR::TransferBit,
                                   vul::AccessFlagBits2KHR::TransferWriteBit,
                                   oldLayout,
                                   vul::ImageLayout::TransferDstOptimal,
                                   subresourceRange,
                                   srcQueueFamilyIndex,
                                   dstQueueFamilyIndex,
                                   pNext);
}

VkImageMemoryBarrier2KHR vul::Image::createFromnTransferBarrier(
        vul::PipelineStageFlagBitMask dstStageMask,
        vul::AccessFlagBitMask dstAccessMask, vul::ImageLayout newLayout,
        const vul::ImageSubresourceRange &subresourceRange,
        std::uint32_t srcQueueFamilyIndex, std::uint32_t dstQueueFamilyIndex,
        const void *pNext) const {
    return createMemoryBarrier(vul::PipelineStageFlagBits2KHR::TransferBit, vul::AccessFlagBits2KHR::TransferWriteBit,
                               dstStageMask,
                               dstAccessMask,
                               vul::ImageLayout::TransferDstOptimal,
                               newLayout,
                               subresourceRange,
                               srcQueueFamilyIndex,
                               dstQueueFamilyIndex,
                               pNext);
}

