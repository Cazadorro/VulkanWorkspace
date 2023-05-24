//
// Created by Shae Bolt on 9/2/2021.
//

#ifndef VULKANWORKSPACE_IMAGE_H
#define VULKANWORKSPACE_IMAGE_H

#include "vul/vmaallocation.h"
#include "vul/enums.h"
#include "vul/commandutils.h"
#include "vul/commandbuffer.h"

#include <uul/enumflags.h>
#include <gsl/span>
#include <vulkan/vulkan.h>

namespace vul {

    class Device;

    class VmaAllocation;

    template<typename T>
    class ExpectedResult;

    class ImageView;

    class ImageSubresourceRange {
    public:
        ImageSubresourceRange() = default;

        explicit ImageSubresourceRange(uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                                       std::uint32_t baseMipLevel = 0,
                                       std::uint32_t levelCount = VK_REMAINING_MIP_LEVELS,
                                       std::uint32_t baseArrayLayer = 0,
                                       std::uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS);

        uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask;
        std::uint32_t baseMipLevel = 0;
        std::uint32_t levelCount = 0;
        std::uint32_t baseArrayLayer = 0;
        std::uint32_t layerCount = 0;

        [[nodiscard]]
        const VkImageSubresourceRange &get() const;
    };

    class ImageSubresourceLayers {
    public:
        ImageSubresourceLayers() = default;

        explicit ImageSubresourceLayers(uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                                        std::uint32_t mipLevel = 0,
                                        std::uint32_t baseArrayLayer = 0,
                                        std::uint32_t layerCount = 1);

        uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask;
        std::uint32_t mipLevel = 0;
        std::uint32_t baseArrayLayer = 0;
        std::uint32_t layerCount = 0;

        [[nodiscard]]
        const VkImageSubresourceLayers &get() const;
    };

    vul::ImageViewType toImageViewType(vul::ImageType type, std::uint32_t arrayLayers, bool isCube = false);

    class Image {
    public:
        Image() = default;

        Image(vul::VmaAllocation &&allocation, VkImage handle,
              vul::ImageType imageType,
              vul::Format format,
              vul::Extent3D extent,
              std::uint32_t mipLevels,
              std::uint32_t arrayLayers,
              uul::EnumFlags<vul::SampleCountFlagBits> samples,
              vul::ImageTiling tiling);

        Image(const Image &) = delete;

        Image &operator=(const Image &) = delete;


        Image(Image &&rhs) noexcept;

        Image &operator=(Image &&rhs) noexcept;

        ~Image();

        [[nodiscard]]
        const VkImage &get() const;


        [[nodiscard]]
        const Device &getDevice() const;


        Result setDebugObjectName(const std::string &name);


        [[nodiscard]]
        vul::VmaAllocation &getAllocation();

        [[nodiscard]]
        const vul::VmaAllocation &getAllocation() const;

        [[nodiscard]]
        VkImageCopy genFullCopyRegion(ImageAspectFlagBits imageAspect,
                                      std::uint32_t mipLevel = 0) const;

        [[nodiscard]]
        VkImageCopy genFullCopyRegion(ImageAspectFlagBits imageAspectSrc,
                                      ImageAspectFlagBits imageAspectDst,
                                      std::uint32_t mipLevel = 0) const;

        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createMemoryBarrier(uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                            uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                            uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                            uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                            vul::ImageLayout oldLayout,
                            vul::ImageLayout newLayout,
                            const ImageSubresourceRange &subresourceRange,
                            std::uint32_t srcQueueFamilyIndex = 0,
                            std::uint32_t dstQueueFamilyIndex = 0,
                            const void *pNext = nullptr) const;

        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createToTransferBarrier(uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                                uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                                const ImageSubresourceRange &subresourceRange,
                                vul::ImageLayout oldLayout = vul::ImageLayout::Undefined,
                                std::uint32_t srcQueueFamilyIndex = 0,
                                std::uint32_t dstQueueFamilyIndex = 0,
                                const void *pNext = nullptr) const;

        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createFromnTransferBarrier(uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                   uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                   vul::ImageLayout newLayout,
                                   const ImageSubresourceRange &subresourceRange,
                                   std::uint32_t srcQueueFamilyIndex = 0,
                                   std::uint32_t dstQueueFamilyIndex = 0,
                                   const void *pNext = nullptr) const;

        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createTransitionBarrier(uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                                uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                                vul::ImageLayout newLayout,
                                const ImageSubresourceRange &subresourceRange,
                                std::uint32_t srcQueueFamilyIndex = 0,
                                std::uint32_t dstQueueFamilyIndex = 0,
                                const void *pNext = nullptr);

        [[nodiscard]]
        vul::ImageType getImageType() const;

        [[nodiscard]]
        vul::Format getImageFormat() const;

        [[nodiscard]]
        VkExtent3D getExtent3D() const;

        [[nodiscard]]
        VkExtent2D getExtent2D() const;

        [[nodiscard]]
        std::uint32_t getMipLevelCount() const;

        [[nodiscard]]
        std::uint32_t getArrayLayerCount() const;

        [[nodiscard]]
        uul::EnumFlags<vul::SampleCountFlagBits> getSamples() const;

        [[nodiscard]]
        vul::ImageTiling getImageTiling() const;


        //Note will add stencil bit to depth/stencil format images in image view, if not wanted, use another.
        [[nodiscard]]
        ExpectedResult<ImageView> createImageView(
                uul::EnumFlags<vul::ImageAspectFlagBits> aspectBitMask = {},
                bool isCube = false,
                const VkComponentMapping &components = {},
                uul::EnumFlags<vul::ImageViewCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<ImageView> createImageView(
                const vul::ImageSubresourceRange &subresourceRange,
                bool isCube = false,
                const VkComponentMapping &components = {},
                uul::EnumFlags<vul::ImageViewCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<ImageView> createImageView(
                vul::ImageViewType viewType,
                const vul::ImageSubresourceRange &subresourceRange,
                const VkComponentMapping &components = {},
                uul::EnumFlags<vul::ImageViewCreateFlagBits> flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        void *mapMemory();

        [[nodiscard]]
        void *getMappedMemory() const;

        void unmapMemory();

        template<typename T>
        void copyToMapped(const TempArrayProxy<T> &array) {
            m_allocation.mappedCopyFrom(array);
        }

        [[nodiscard]]
        bool isMapped() const;

        void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        void
        invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        [[nodiscard]]
        vul::ImageViewType
        toImageViewType(const vul::ImageSubresourceRange &subresourceRange, bool isCube = false) const;

    private:
        vul::VmaAllocation m_allocation;
        VkImage m_handle = VK_NULL_HANDLE;
        vul::ImageType m_imageType;
        vul::Format m_format;
        vul::Extent3D m_extent;
        std::uint32_t m_mipLevels;
        std::uint32_t m_arrayLayers;
        uul::EnumFlags<vul::SampleCountFlagBits> m_samples;
        vul::ImageTiling m_tiling;
    };

    struct ImageCreateInfo {
        vul::StructureType sType = vul::StructureType::ImageCreateInfo;
        const void *pNext;
        uul::EnumFlags<vul::ImageCreateFlagBits> flags;
        vul::ImageType imageType;
        vul::Format format;
        vul::Extent3D extent;
        std::uint32_t mipLevels;
        std::uint32_t arrayLayers;
        uul::EnumFlags<vul::SampleCountFlagBits> samples;
        vul::ImageTiling tiling;
        uul::EnumFlags<vul::ImageUsageFlagBits> usage;
        vul::SharingMode sharingMode;
        std::uint32_t queueFamilyIndexCount;
        const std::uint32_t *pQueueFamilyIndices;
        vul::ImageLayout initialLayout;

        ImageCreateInfo() = default;


        explicit ImageCreateInfo(
                const void *pNext,
                uul::EnumFlags<vul::ImageCreateFlagBits> flags,
                vul::ImageType imageType,
                vul::Format format,
                vul::Extent3D extent,
                std::uint32_t mipLevels,
                std::uint32_t arrayLayers,
                uul::EnumFlags<vul::SampleCountFlagBits> samples,
                vul::ImageTiling tiling,
                uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                vul::SharingMode sharingMode,
                std::uint32_t queueFamilyIndexCount,
                const std::uint32_t *pQueueFamilyIndices,
                vul::ImageLayout initialLayout
        );

        explicit ImageCreateInfo(VkImageCreateInfo imageCreateInfo);

        explicit operator VkImageCreateInfo() const;

        [[nodiscard]]
        VkImageCreateInfo &get();

        [[nodiscard]]
        const VkImageCreateInfo &get() const;
    };


    [[nodiscard]]
    ImageCreateInfo createSimpleImageInfo(vul::ImageType image_type, vul::Format format, vul::Extent3D extent,
                                          uul::EnumFlags<vul::ImageUsageFlagBits> usage, std::uint32_t mipLevels,
                                          std::uint32_t arrayLayers,
                                          vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimpleImageInfo(vul::ImageType image_type, vul::Format format, vul::Extent3D extent,
                                          uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                          vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimple1DImageInfo(vul::Format format, std::uint32_t extent, uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                            std::uint32_t mipLevels, std::uint32_t arrayLayers,
                                            vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimple1DImageInfo(vul::Format format, std::uint32_t extent, uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                            vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimple2DImageInfo(vul::Format format, vul::Extent2D extent, uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                            std::uint32_t mipLevels, std::uint32_t arrayLayers,
                                            vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimple2DImageInfo(vul::Format format, vul::Extent2D extent, uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                            vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimple3DImageInfo(vul::Format format, vul::Extent3D extent, uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                            std::uint32_t mipLevels, std::uint32_t arrayLayers,
                                            vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createSimple3DImageInfo(vul::Format format, vul::Extent3D extent, uul::EnumFlags<vul::ImageUsageFlagBits> usage,
                                            vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    ImageCreateInfo createDepthStencilImageInfo(vul::Extent2D extent, vul::Format format = vul::Format::D24UnormS8Uint,
                                                vul::ImageTiling tiling = vul::ImageTiling::Optimal);


}
#endif //VULKANWORKSPACE_IMAGE_H
