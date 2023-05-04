//
// Created by Shae Bolt on 8/17/2021.
//

#ifndef VULKANWORKSPACE_SWAPCHAIN_H
#define VULKANWORKSPACE_SWAPCHAIN_H

#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include "vul/vkstructutils.h"
#include <gsl/span>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <cstdint>
namespace vul {

    template<typename T>
    class ExpectedResult;
    class Device;
    class ImageView;
    class SurfaceFormat;
    class Surface;
    class Swapchain;
    class Queue;
    class BinarySemaphore;
    struct Extent2D;
    struct Extent3D;

    //TODO stuff swaqchain builder into surface or into swapchain?
    class SwapchainBuilder {
    public:
        explicit SwapchainBuilder(const Device &device,
                                  const VkAllocationCallbacks *pAllocator = nullptr);

        SwapchainBuilder &pNext(const void *value);

        SwapchainBuilder &flags(vul::SwapchainCreateFlagBitMask value);

        SwapchainBuilder &minImageCount(uint32_t value);

        SwapchainBuilder &imageFormat(vul::Format value);

        SwapchainBuilder &imageColorSpace(vul::ColorSpaceKHR value);

        SwapchainBuilder &surfaceFormat(const vul::SurfaceFormat &value);

        SwapchainBuilder &imageExtent(VkExtent2D value);

        SwapchainBuilder &imageArrayLayers(uint32_t value);

        SwapchainBuilder &imageUsage(vul::ImageUsageBitMask value);

        SwapchainBuilder &imageSharingMode(vul::SharingMode value);

        SwapchainBuilder &
        queueFamilyIndices(const gsl::span<std::uint32_t> &value);

        SwapchainBuilder &preTransform(vul::SurfaceTransformFlagBitMask value);

        SwapchainBuilder &compositeAlpha(vul::CompositeAlphaFlagBitMask value);

        SwapchainBuilder &presentMode(vul::PresentModeKHR value);

        SwapchainBuilder &clipped(VkBool32 value);

        SwapchainBuilder &oldSwapchain(const Swapchain &value);

        [[nodiscard]]
        ExpectedResult<Swapchain> create(const Surface &surface) const;

        [[nodiscard]]
        ExpectedResult<Swapchain>  resize(Swapchain &swapchain, const VkExtent2D &extent) const;


    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkSwapchainCreateInfoKHR m_createInfo = {};
    };

    class Swapchain {
    public:
        Swapchain() = default;

        Swapchain(const Device &device, const Surface& surface,
                  VkSwapchainKHR handle,
                  VkExtent2D extent,
                  vul::Format format,
                  std::vector<VkImage> images,
                  std::vector<ImageView> imageViews,
                  const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkSwapchainKHR get() const;

        ~Swapchain();

        Swapchain(Swapchain &&rhs) noexcept;

//        was noexcept?
        Swapchain &operator=(Swapchain &&rhs) noexcept;

        Swapchain(const Swapchain &rhs) = delete;

        Swapchain &operator=(const Swapchain &rhs) = delete;

        [[nodiscard]]
        vul::Extent2D getExtent() const;

        [[nodiscard]]
        vul::Extent3D getExtent3D() const;

        [[nodiscard]]
        vul::Format getFormat() const;

        [[nodiscard]]
        std::size_t getImageCount() const;

        Result setObjectName(const std::string &string);


        [[nodiscard]]
        ExpectedResult<std::uint32_t> acquireNextImage(
                const BinarySemaphore& presentationFinishedSemaphore,
                //std::numeric_limits<std::uint64_t>::max() not used for compiletime reasons.
                //cstdint has to be included anyway, which is where UINT64_MAX is located.
                std::uint64_t timeout_ns = UINT64_MAX,
                VkFence fence = VK_NULL_HANDLE) const;

        //not no-discard, can still do things.
        Result present(const Queue& presentQueue, const BinarySemaphore& renderingFinishedSemaphore,
                       std::uint32_t swapchainImageIndex) const;

        [[nodiscard]]
        const std::vector<ImageView>& getImageViews() const;
        [[nodiscard]]
        const std::vector<VkImage>& getImages() const;

        [[nodiscard]]
        const Surface& getSurface() const;

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        const Surface *m_pSurface = nullptr;
        VkSwapchainKHR m_handle = VK_NULL_HANDLE;
        vul::Extent2D m_extent = {};
        vul::Format m_format = {};
        std::vector<VkImage> m_images;
        std::vector<ImageView> m_imageViews;
        std::vector<VkSemaphore> m_acquireNextImageSemaphore;
        //TODO need ImageView2D and Image2D arrays.
        //But do we need Images? and Image views? we get images from swapchain, but we have no clue how made
        //May need to create "Swapchain image view" class, or have an ImageView interface?
        //ImageView may only need allocator and device, so maybe we just stick with those things instead of attaching Image to it
        //Maybe keep no extra data instead of format/aspect/mip etc...?  Keep that in image or not at all?
    };


}


#endif //VULKANWORKSPACE_SWAPCHAIN_H
