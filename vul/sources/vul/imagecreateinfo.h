//
// Created by Bolt on 7/8/2023.
//

#ifndef VULKANWORKSPACE_IMAGECREATEINFO_H
#define VULKANWORKSPACE_IMAGECREATEINFO_H

#include "vul/enums.h"
#include "vul/vkstructutils.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {
    class ImageCreateInfo {
    private:
        const vul::StructureType sType = vul::StructureType::ImageCreateInfo;
    public:
        const void *pNext;
        uul::EnumFlags<ImageCreateFlagBits> flags;
        vul::ImageType imageType;
        vul::Format format;
        vul::Extent3D extent;
        std::uint32_t mipLevels;
        std::uint32_t arrayLayers;
        vul::SampleCountFlagBits samples;
        vul::ImageTiling tiling;
        uul::EnumFlags<ImageUsageFlagBits> usage;
        vul::SharingMode sharingMode;
        std::uint32_t queueFamilyIndexCount;
        const std::uint32_t *pQueueFamilyIndices;
        vul::ImageLayout initialLayout;

        ImageCreateInfo();

        ImageCreateInfo(const void *pNext,
                        uul::EnumFlags<ImageCreateFlagBits> flags,
                        vul::ImageType imageType,
                        vul::Format format,
                        vul::Extent3D extent,
                        std::uint32_t mipLevels,
                        std::uint32_t arrayLayers,
                        vul::SampleCountFlagBits samples,
                        vul::ImageTiling tiling,
                        uul::EnumFlags<ImageUsageFlagBits> usage,
                        vul::SharingMode sharingMode,
                        std::uint32_t queueFamilyIndexCount,
                        const std::uint32_t *pQueueFamilyIndices,
                        vul::ImageLayout initialLayout);


        ImageCreateInfo(const ImageCreateInfo &rhs) noexcept;

        ImageCreateInfo(ImageCreateInfo &&rhs) noexcept;

        ImageCreateInfo &operator=(const ImageCreateInfo &rhs) noexcept;

        ~ImageCreateInfo();

        operator VkImageCreateInfo() const noexcept;

        [[nodiscard]]
        VkImageCreateInfo &get() noexcept;

        [[nodiscard]]
        const VkImageCreateInfo &get() const noexcept;


        void setExtent(std::uint32_t extent);
        void setExtent(vul::Extent2D extent);
        void setExtent(vul::Extent3D extent);
        void setTransferSrc();

        void setTransferDst();

        [[nodiscard]]
        static ImageCreateInfo _1D(uul::EnumFlags<ImageUsageFlagBits> usage,
                                   std::uint32_t extent,
                                   vul::Format format,
                                   std::uint32_t mipLevels = 1,
                                   std::uint32_t arrayLayers = 1,
                                   vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                                   vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                   uul::EnumFlags<ImageCreateFlagBits> flags = {},
                                   vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                                   std::uint32_t queueFamilyIndexCount = 0,
                                   const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo _2D(uul::EnumFlags<ImageUsageFlagBits> usage,
                                   vul::Extent2D extent,
                                   vul::Format format,
                                   std::uint32_t mipLevels = 1,
                                   std::uint32_t arrayLayers = 1,
                                   vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                                   vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                   uul::EnumFlags<ImageCreateFlagBits> flags = {},
                                   vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                                   std::uint32_t queueFamilyIndexCount = 0,
                                   const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo _3D(uul::EnumFlags<ImageUsageFlagBits> usage,
                                   vul::Extent3D extent,
                                   vul::Format format,
                                   std::uint32_t mipLevels = 1,
                                   std::uint32_t arrayLayers = 1,
                                   vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                                   vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                   uul::EnumFlags<ImageCreateFlagBits> flags = {},
                                   vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                                   std::uint32_t queueFamilyIndexCount = 0,
                                   const std::uint32_t *pQueueFamilyIndices = nullptr);


        //By definition, texture must be sampled and transfer destination (have to upload data)
        [[nodiscard]]
        static ImageCreateInfo texture1D(
                std::uint32_t extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo texture2D(
                vul::Extent2D extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo texture3D(
                vul::Extent3D extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo sampledStorage1D(
                std::uint32_t extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        //Storage images that aren't sampled kind of defeat the purpose of storage images (unless you're transferring)
        [[nodiscard]]
        static ImageCreateInfo sampledStorage2D(
                vul::Extent2D extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo sampledStorage3D(
                vul::Extent3D extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo colorAttachment2D(
                vul::Extent2D extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo sampledColorAttachment2D(
                vul::Extent2D extent,
                vul::Format format,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo depthStencilAttachment2D(
                vul::Extent2D extent,
                vul::Format format = vul::Format::D24UnormS8Uint,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

        [[nodiscard]]
        static ImageCreateInfo sampledDepthStencilAttachment2D(
                vul::Extent2D extent,
                vul::Format format = vul::Format::D24UnormS8Uint,
                std::uint32_t mipLevels = 1,
                std::uint32_t arrayLayers = 1,
                vul::ImageTiling tiling = vul::ImageTiling::Optimal,
                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                uul::EnumFlags<ImageCreateFlagBits> flags = {},
                vul::SharingMode sharingMode = vul::SharingMode::Exclusive,
                std::uint32_t queueFamilyIndexCount = 0,
                const std::uint32_t *pQueueFamilyIndices = nullptr);

    };
}
#endif //VULKANWORKSPACE_IMAGECREATEINFO_H
