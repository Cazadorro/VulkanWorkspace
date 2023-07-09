//
// Created by Bolt on 7/8/2023.
//

#include "imagecreateinfo.h"

vul::ImageCreateInfo::ImageCreateInfo() :
        pNext(nullptr),
        flags(0),
        imageType(vul::ImageType{}),
        format(vul::Format{}),
        extent(vul::Extent3D{}),
        mipLevels(0),
        arrayLayers(0),
        samples(vul::SampleCountFlagBits{}),
        tiling(vul::ImageTiling{}),
        usage(0),
        sharingMode(vul::SharingMode{}),
        queueFamilyIndexCount(0),
        pQueueFamilyIndices(nullptr),
        initialLayout(vul::ImageLayout::Undefined) {

}


vul::ImageCreateInfo::ImageCreateInfo(const void *pNext, uul::EnumFlags<ImageCreateFlagBits> flags,
                                      vul::ImageType imageType, vul::Format format, vul::Extent3D extent,
                                      std::uint32_t mipLevels, std::uint32_t arrayLayers,
                                      vul::SampleCountFlagBits samples, vul::ImageTiling tiling,
                                      uul::EnumFlags<ImageUsageFlagBits> usage, vul::SharingMode sharingMode,
                                      std::uint32_t queueFamilyIndexCount, const std::uint32_t *pQueueFamilyIndices,
                                      vul::ImageLayout initialLayout) :
        pNext(pNext),
        flags(flags),
        imageType(imageType),
        format(format),
        extent(extent),
        mipLevels(mipLevels),
        arrayLayers(arrayLayers),
        samples(samples),
        tiling(tiling),
        usage(usage),
        sharingMode(sharingMode),
        queueFamilyIndexCount(queueFamilyIndexCount),
        pQueueFamilyIndices(pQueueFamilyIndices),
        initialLayout(initialLayout) {
}

vul::ImageCreateInfo::ImageCreateInfo(const vul::ImageCreateInfo &rhs) noexcept:
        pNext(rhs.pNext),
        flags(rhs.flags),
        imageType(rhs.imageType),
        format(rhs.format),
        extent(rhs.extent),
        mipLevels(rhs.mipLevels),
        arrayLayers(rhs.arrayLayers),
        samples(rhs.samples),
        tiling(rhs.tiling),
        usage(rhs.usage),
        sharingMode(rhs.sharingMode),
        queueFamilyIndexCount(rhs.queueFamilyIndexCount),
        pQueueFamilyIndices(rhs.pQueueFamilyIndices),
        initialLayout(rhs.initialLayout) {

}

vul::ImageCreateInfo::ImageCreateInfo(vul::ImageCreateInfo &&rhs) noexcept:
        pNext(rhs.pNext),
        flags(rhs.flags),
        imageType(rhs.imageType),
        format(rhs.format),
        extent(rhs.extent),
        mipLevels(rhs.mipLevels),
        arrayLayers(rhs.arrayLayers),
        samples(rhs.samples),
        tiling(rhs.tiling),
        usage(rhs.usage),
        sharingMode(rhs.sharingMode),
        queueFamilyIndexCount(rhs.queueFamilyIndexCount),
        pQueueFamilyIndices(rhs.pQueueFamilyIndices),
        initialLayout(rhs.initialLayout) {

}

vul::ImageCreateInfo &vul::ImageCreateInfo::operator=(const vul::ImageCreateInfo &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        flags = rhs.flags;
        imageType = rhs.imageType;
        format = rhs.format;
        extent = rhs.extent;
        mipLevels = rhs.mipLevels;
        arrayLayers = rhs.arrayLayers;
        samples = rhs.samples;
        tiling = rhs.tiling;
        usage = rhs.usage;
        sharingMode = rhs.sharingMode;
        queueFamilyIndexCount = rhs.queueFamilyIndexCount;
        pQueueFamilyIndices = rhs.pQueueFamilyIndices;
        initialLayout = rhs.initialLayout;
    }
    return *this;
}

vul::ImageCreateInfo::~ImageCreateInfo() = default;

vul::ImageCreateInfo::operator VkImageCreateInfo() const noexcept {
    return get();
}

VkImageCreateInfo &vul::ImageCreateInfo::get() noexcept {
    return *reinterpret_cast<VkImageCreateInfo *>(this);
}

const VkImageCreateInfo &vul::ImageCreateInfo::get() const noexcept {
    return *reinterpret_cast<const VkImageCreateInfo *>(this);
}

void vul::ImageCreateInfo::setTransferSrc() {
    usage |= ImageUsageFlagBits::TransferSrcBit;
}

void vul::ImageCreateInfo::setTransferDst() {
    usage |= ImageUsageFlagBits::TransferDstBit;
}

vul::ImageCreateInfo
vul::ImageCreateInfo::_1D(uul::EnumFlags<ImageUsageFlagBits> usage, std::uint32_t extent, vul::Format format,
                          std::uint32_t mipLevels, std::uint32_t arrayLayers,
                          vul::ImageTiling tiling, vul::SampleCountFlagBits samples,
                          uul::EnumFlags<ImageCreateFlagBits> flags, vul::SharingMode sharingMode,
                          std::uint32_t queueFamilyIndexCount,
                          const std::uint32_t *pQueueFamilyIndices) {
    return {nullptr, flags, ImageType::_1D, format, Extent3D::fromDim(extent), mipLevels, arrayLayers, samples, tiling,
            usage,
            sharingMode, queueFamilyIndexCount, pQueueFamilyIndices,
            ImageLayout::Undefined};
}

vul::ImageCreateInfo
vul::ImageCreateInfo::_2D(uul::EnumFlags<ImageUsageFlagBits> usage, vul::Extent2D extent, vul::Format format,
                          std::uint32_t mipLevels, std::uint32_t arrayLayers,
                          vul::ImageTiling tiling, vul::SampleCountFlagBits samples,
                          uul::EnumFlags<ImageCreateFlagBits> flags, vul::SharingMode sharingMode,
                          std::uint32_t queueFamilyIndexCount,
                          const std::uint32_t *pQueueFamilyIndices) {
    return {nullptr, flags, ImageType::_2D, format, extent.to3D(), mipLevels, arrayLayers, samples, tiling, usage,
            sharingMode, queueFamilyIndexCount, pQueueFamilyIndices,
            ImageLayout::Undefined};
}


vul::ImageCreateInfo
vul::ImageCreateInfo::_3D(uul::EnumFlags<ImageUsageFlagBits> usage, vul::Extent3D extent, vul::Format format,
                          std::uint32_t mipLevels, std::uint32_t arrayLayers,
                          vul::ImageTiling tiling, vul::SampleCountFlagBits samples,
                          uul::EnumFlags<ImageCreateFlagBits> flags, vul::SharingMode sharingMode,
                          std::uint32_t queueFamilyIndexCount,
                          const std::uint32_t *pQueueFamilyIndices) {
    return {nullptr, flags, ImageType::_3D, format, extent, mipLevels, arrayLayers, samples, tiling, usage,
            sharingMode, queueFamilyIndexCount, pQueueFamilyIndices,
            ImageLayout::Undefined};
}


vul::ImageCreateInfo
vul::ImageCreateInfo::texture1D(std::uint32_t extent, vul::Format format, std::uint32_t mipLevels,
                                std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                const std::uint32_t *pQueueFamilyIndices) {
    return _1D(ImageUsageFlagBits::SampledBit | vul::ImageUsageFlagBits::TransferDstBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}

vul::ImageCreateInfo
vul::ImageCreateInfo::texture2D(vul::Extent2D extent, vul::Format format, std::uint32_t mipLevels,
                                std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                const std::uint32_t *pQueueFamilyIndices) {
    return _2D(ImageUsageFlagBits::SampledBit | vul::ImageUsageFlagBits::TransferDstBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}

vul::ImageCreateInfo
vul::ImageCreateInfo::texture3D(vul::Extent3D extent, vul::Format format, std::uint32_t mipLevels,
                                std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                const std::uint32_t *pQueueFamilyIndices) {
    return _3D(ImageUsageFlagBits::SampledBit | vul::ImageUsageFlagBits::TransferDstBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}


vul::ImageCreateInfo
vul::ImageCreateInfo::sampledStorage1D(std::uint32_t extent, vul::Format format,
                                       std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                       vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                       vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                       const std::uint32_t *pQueueFamilyIndices) {
    return _1D(ImageUsageFlagBits::SampledBit | vul::ImageUsageFlagBits::StorageBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}

vul::ImageCreateInfo
vul::ImageCreateInfo::sampledStorage2D(vul::Extent2D extent, vul::Format format,
                                       std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                       vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                       vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                       const std::uint32_t *pQueueFamilyIndices) {
    return _2D(ImageUsageFlagBits::SampledBit | vul::ImageUsageFlagBits::StorageBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);

}

vul::ImageCreateInfo
vul::ImageCreateInfo::sampledStorage3D(vul::Extent3D extent, vul::Format format,
                                       std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                       vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                       vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                       const std::uint32_t *pQueueFamilyIndices) {
    return _3D(ImageUsageFlagBits::SampledBit | vul::ImageUsageFlagBits::StorageBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);

}


vul::ImageCreateInfo
vul::ImageCreateInfo::colorAttachment2D(vul::Extent2D extent, vul::Format format, std::uint32_t mipLevels,
                                        std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                        vul::SampleCountFlagBits samples, uul::EnumFlags<ImageCreateFlagBits> flags,
                                        vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                        const std::uint32_t *pQueueFamilyIndices) {
    return _2D(ImageUsageFlagBits::ColorAttachmentBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);

}

vul::ImageCreateInfo
vul::ImageCreateInfo::sampledColorAttachment2D(vul::Extent2D extent, vul::Format format, std::uint32_t mipLevels,
                                               std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                               vul::SampleCountFlagBits samples,
                                               uul::EnumFlags<ImageCreateFlagBits> flags, vul::SharingMode sharingMode,
                                               std::uint32_t queueFamilyIndexCount,
                                               const std::uint32_t *pQueueFamilyIndices) {
    return _2D(ImageUsageFlagBits::ColorAttachmentBit | ImageUsageFlagBits::SampledBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}

vul::ImageCreateInfo
vul::ImageCreateInfo::depthStencilAttachment2D(vul::Extent2D extent, vul::Format format, std::uint32_t mipLevels,
                                               std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                               vul::SampleCountFlagBits samples,
                                               uul::EnumFlags<ImageCreateFlagBits> flags, vul::SharingMode sharingMode,
                                               std::uint32_t queueFamilyIndexCount,
                                               const std::uint32_t *pQueueFamilyIndices) {
    return _2D(ImageUsageFlagBits::DepthStencilAttachmentBit, extent, format, mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}

vul::ImageCreateInfo
vul::ImageCreateInfo::sampledDepthStencilAttachment2D(vul::Extent2D extent, vul::Format format, std::uint32_t mipLevels,
                                                      std::uint32_t arrayLayers, vul::ImageTiling tiling,
                                                      vul::SampleCountFlagBits samples,
                                                      uul::EnumFlags<ImageCreateFlagBits> flags,
                                                      vul::SharingMode sharingMode, std::uint32_t queueFamilyIndexCount,
                                                      const std::uint32_t *pQueueFamilyIndices) {
    return _2D(ImageUsageFlagBits::DepthStencilAttachmentBit | ImageUsageFlagBits::SampledBit, extent, format,
               mipLevels,
               arrayLayers, tiling, samples, flags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
}

void vul::ImageCreateInfo::setExtent(std::uint32_t extent_t) {
    extent = Extent3D::fromDim(extent_t);
}

void vul::ImageCreateInfo::setExtent(vul::Extent2D extent_t) {
    extent = extent_t.to3D();
}

void vul::ImageCreateInfo::setExtent(vul::Extent3D extent_t) {
    extent = extent_t;
}

