//
// Created by Bolt on 7/7/2023.
//

#ifndef VULKANWORKSPACE_IMAGEMEMORYBARRIER_H
#define VULKANWORKSPACE_IMAGEMEMORYBARRIER_H

#include "vul/image.h"
#include "vul/enums.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {
    class ImageMemoryBarrier {
    private:
        const vul::StructureType sType = vul::StructureType::ImageMemoryBarrier2;
    public:
        const void *pNext;
        uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask;
        uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask;
        uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask;
        uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask;
        vul::ImageLayout oldLayout;
        vul::ImageLayout newLayout;
        std::uint32_t srcQueueFamilyIndex;
        std::uint32_t dstQueueFamilyIndex;
        VkImage image;
        vul::ImageSubresourceRange subresourceRange;

        ImageMemoryBarrier();

        ImageMemoryBarrier(const void *pNext,
                           uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                           uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                           uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                           uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                           vul::ImageLayout oldLayout,
                           vul::ImageLayout newLayout,
                           std::uint32_t srcQueueFamilyIndex,
                           std::uint32_t dstQueueFamilyIndex,
                           VkImage image,
                           vul::ImageSubresourceRange subresourceRange);


        ImageMemoryBarrier(const ImageMemoryBarrier &rhs) noexcept;

        ImageMemoryBarrier(ImageMemoryBarrier &&rhs) noexcept;

        ImageMemoryBarrier &operator=(const ImageMemoryBarrier &rhs) noexcept;

        ~ImageMemoryBarrier();

        operator VkImageMemoryBarrier2() const noexcept;

        [[nodiscard]]
        VkImageMemoryBarrier2 &get() noexcept;

        [[nodiscard]]
        const VkImageMemoryBarrier2 &get() const noexcept;

        void setImage(const Image& image);
        [[nodiscard]]
        static ImageMemoryBarrier fromImage(
                const vul::Image &image,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> srcAccessMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout oldLayout,
                vul::ImageLayout newLayout,
                std::uint32_t srcQueueFamilyIndex = 0,
                std::uint32_t dstQueueFamilyIndex = 0,
                const void *pNext = nullptr
        );

        [[nodiscard]]
        static ImageMemoryBarrier layoutTransition(
                const vul::Image &image,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout oldLayout,
                vul::ImageLayout newLayout);

        [[nodiscard]]
        static ImageMemoryBarrier unfilledLayoutTransition(
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout oldLayout,
                vul::ImageLayout newLayout);


        [[nodiscard]]
        static ImageMemoryBarrier readAfterWrite(
                const vul::Image &image,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                vul::ImageLayout oldLayout,
                vul::ImageLayout newLayout
        );

        static ImageMemoryBarrier WriteAfterRead(
                const vul::Image &image,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                vul::ImageLayout oldLayout,
                vul::ImageLayout newLayout
        );

        [[nodiscard]]
        static ImageMemoryBarrier readWriteAfterReadWrite(
                const vul::Image &image,
                uul::EnumFlags<vul::PipelineStageFlagBits2> srcStageMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2> dstStageMask,
                vul::ImageLayout oldLayout,
                vul::ImageLayout newLayout
        );

        [[nodiscard]]
        static ImageMemoryBarrier computeToComputeRAW(
                const vul::Image &image,
                vul::ImageLayout oldLayout = vul::ImageLayout::General,
                vul::ImageLayout newLayout = vul::ImageLayout::ReadOnlyOptimal
        );

        [[nodiscard]]
        static ImageMemoryBarrier computeToComputeRWARW(
                const vul::Image &image,
                vul::ImageLayout oldLayout = vul::ImageLayout::General,
                vul::ImageLayout newLayout = vul::ImageLayout::General
        );

        [[nodiscard]]
        static ImageMemoryBarrier computeToFragmentRAW(
                const vul::Image &image,
                vul::ImageLayout oldLayout = vul::ImageLayout::General,
                vul::ImageLayout newLayout = vul::ImageLayout::ReadOnlyOptimal
        );

        [[nodiscard]]
        static ImageMemoryBarrier fragmentToComputeRAW(
                const vul::Image &image,
                vul::ImageLayout oldLayout = vul::ImageLayout::AttachmentOptimal,
                vul::ImageLayout newLayout = vul::ImageLayout::ReadOnlyOptimal
        );
    };
}
#endif //VULKANWORKSPACE_IMAGEMEMORYBARRIER_H
