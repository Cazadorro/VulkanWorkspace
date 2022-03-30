//
// Created by Shae Bolt on 9/2/2021.
//

#ifndef VULKANWORKSPACE_IMAGE_H
#define VULKANWORKSPACE_IMAGE_H

#include "vul/vmaallocation.h"
#include "vul/enums.h"
#include "vul/bitmasks.h"
#include "commandutils.h"
#include "commandbuffer.h"


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

        explicit ImageSubresourceRange(vul::ImageAspectBitMask aspectMask,
                                       std::uint32_t baseMipLevel = 0,
                                       std::uint32_t levelCount = VK_REMAINING_MIP_LEVELS,
                                       std::uint32_t baseArrayLayer = 0,
                                       std::uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS);

        vul::ImageAspectBitMask aspectMask;
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

        explicit ImageSubresourceLayers(vul::ImageAspectBitMask aspectMask,
                                        std::uint32_t mipLevel = 0,
                                        std::uint32_t baseArrayLayer = 0,
                                        std::uint32_t layerCount = 1);

        vul::ImageAspectBitMask aspectMask;
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
              VkImageType imageType,
              VkFormat format,
              VkExtent3D extent,
              uint32_t mipLevels,
              uint32_t arrayLayers,
              VkSampleCountFlagBits samples,
              VkImageTiling tiling);

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
        createMemoryBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                            vul::AccessFlagBitMask srcAccessMask,
                            vul::PipelineStageFlagBitMask dstStageMask,
                            vul::AccessFlagBitMask dstAccessMask,
                            vul::ImageLayout oldLayout,
                            vul::ImageLayout newLayout,
                            const ImageSubresourceRange &subresourceRange,
                            std::uint32_t srcQueueFamilyIndex = 0,
                            std::uint32_t dstQueueFamilyIndex = 0,
                            const void *pNext = nullptr) const;
        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createToTransferBarrier(vul::PipelineStageFlagBitMask srcStageMask,
                                vul::AccessFlagBitMask srcAccessMask,
                                const ImageSubresourceRange &subresourceRange,
                                vul::ImageLayout oldLayout = vul::ImageLayout::Undefined,
                                std::uint32_t srcQueueFamilyIndex = 0,
                                std::uint32_t dstQueueFamilyIndex = 0,
                                const void *pNext = nullptr) const;
        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createFromnTransferBarrier(vul::PipelineStageFlagBitMask dstStageMask,
                                   vul::AccessFlagBitMask dstAccessMask,
                                   vul::ImageLayout newLayout,
                                   const ImageSubresourceRange &subresourceRange,
                                   std::uint32_t srcQueueFamilyIndex = 0,
                                   std::uint32_t dstQueueFamilyIndex = 0,
                                   const void *pNext = nullptr) const;

        [[nodiscard]]
        VkImageMemoryBarrier2KHR
        createTransitionBarrier( vul::PipelineStageFlagBitMask dstStageMask,
                                 vul::AccessFlagBitMask dstAccessMask,
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
        vul::SampleCountBitMask getSamples() const;

        [[nodiscard]]
        vul::ImageTiling getImageTiling() const;


        [[nodiscard]]
        ExpectedResult<ImageView> createImageView(
                const vul::ImageSubresourceRange &subresourceRange,
                bool isCube = false,
                const VkComponentMapping &components = {},
                vul::ImageViewCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;

        [[nodiscard]]
        ExpectedResult<ImageView> createImageView(
                vul::ImageViewType viewType,
                const vul::ImageSubresourceRange &subresourceRange,
                const VkComponentMapping &components = {},
                vul::ImageViewCreateBitMask flags = {},
                const void *pNext = nullptr,
                const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        void *mapMemory();

        [[nodiscard]]
        void * getMappedMemory() const;

        void unmapMemory();

        template<typename T>
        void copyToMapped(const TempArrayProxy<T>& array){
            m_allocation.mappedCopyFrom(array);
        }

        [[nodiscard]]
        bool isMapped() const;

        void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        void
        invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        [[nodiscard]]
        vul::ImageViewType toImageViewType(const vul::ImageSubresourceRange &subresourceRange, bool isCube = false) const;
    private:
        vul::VmaAllocation m_allocation;
        VkImage m_handle = VK_NULL_HANDLE;
        VkImageType m_imageType;
        VkFormat m_format;
        VkExtent3D m_extent;
        uint32_t m_mipLevels;
        uint32_t m_arrayLayers;
        VkSampleCountFlagBits m_samples;
        VkImageTiling m_tiling;
    };

    [[nodiscard]]
    VkImageCreateInfo createSimpleImageInfo(vul::ImageType image_type, vul::Format format, VkExtent3D extent, vul::ImageUsageBitMask usage, std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling = vul::ImageTiling::Optimal);
    [[nodiscard]]
    VkImageCreateInfo createSimpleImageInfo(vul::ImageType image_type, vul::Format format, VkExtent3D extent, vul::ImageUsageBitMask usage, vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    VkImageCreateInfo createSimple1DImageInfo(vul::Format format, std::uint32_t extent, vul::ImageUsageBitMask usage, std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling = vul::ImageTiling::Optimal);
    [[nodiscard]]
    VkImageCreateInfo createSimple1DImageInfo(vul::Format format, std::uint32_t extent, vul::ImageUsageBitMask usage, vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    VkImageCreateInfo createSimple2DImageInfo(vul::Format format, VkExtent2D extent, vul::ImageUsageBitMask usage, std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling = vul::ImageTiling::Optimal);
    [[nodiscard]]
    VkImageCreateInfo createSimple2DImageInfo(vul::Format format, VkExtent2D extent, vul::ImageUsageBitMask usage, vul::ImageTiling tiling = vul::ImageTiling::Optimal);

    [[nodiscard]]
    VkImageCreateInfo createSimple3DImageInfo(vul::Format format, VkExtent3D extent, vul::ImageUsageBitMask usage, std::uint32_t mipLevels, std::uint32_t arrayLayers, vul::ImageTiling tiling = vul::ImageTiling::Optimal);
    [[nodiscard]]
    VkImageCreateInfo createSimple3DImageInfo(vul::Format format, VkExtent3D extent, vul::ImageUsageBitMask usage, vul::ImageTiling tiling = vul::ImageTiling::Optimal);



}
#endif //VULKANWORKSPACE_IMAGE_H
