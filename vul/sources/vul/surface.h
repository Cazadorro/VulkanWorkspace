//
// Created by Shae Bolt on 6/6/2021.
//

#ifndef VULKANWORKSPACE_SURFACE_H
#define VULKANWORKSPACE_SURFACE_H

#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include "vul/bitmasks.h"
#include <vulkan/vulkan.h>
#include <gsl/span>
#include <vector>
#include <optional>
#include <memory>

namespace vul {
    template<typename T>
    class ExpectedResult;
    class Device;
    class ImageView;
    class Swapchain;
    class SwapchainBuilder;

    class SurfaceFormat {
    public:
        vul::Format format;
        vul::ColorSpaceKHR colorSpace;
    };

    [[nodiscard]]
    bool operator==(const SurfaceFormat &lhs, const SurfaceFormat &rhs);

    [[nodiscard]]
    bool operator!=(const SurfaceFormat &lhs, const SurfaceFormat &rhs);

    class Instance;

    class PhysicalDevice;

    class SurfaceFormats {
    public:
        explicit SurfaceFormats(std::vector<SurfaceFormat> surfaceFormats);

        [[nodiscard]]
        bool isSupported(const SurfaceFormat &surfaceFormat) const;

        [[nodiscard]]
        std::optional<SurfaceFormat> getBestSupported(
                const std::vector<SurfaceFormat> &rankedSurfaceFormats) const;

    private:
        std::vector<SurfaceFormat> m_surfaceFormats;
    };

    class PresentModes {
    public:
        explicit PresentModes(std::vector<vul::PresentModeKHR> presentModes);

        [[nodiscard]]
        bool isSupported(const vul::PresentModeKHR &presentMode) const;

        [[nodiscard]]
        std::optional<vul::PresentModeKHR> getBestSupported(
                const std::vector<vul::PresentModeKHR> &rankedPresentModes) const;

    private:
        std::vector<vul::PresentModeKHR> m_presentModes;
    };

    class SurfaceCapabilities {
    public:
        std::uint32_t minImageCount;
        std::uint32_t maxImageCount;
        VkExtent2D currentExtent;
        VkExtent2D minImageExtent;
        VkExtent2D maxImageExtent;
        std::uint32_t maxImageArrayLayers;
        vul::SurfaceTransformFlagBitMask supportedTransforms;
        vul::SurfaceTransformFlagBitsKHR currentTransform;
        vul::CompositeAlphaFlagBitMask supportedCompositeAlpha;
        vul::ImageUsageBitMask supportedUsageFlags;
        [[nodiscard]]
        std::uint32_t calcSwapchainImageCount() const;
        [[nodiscard]]
        VkExtent2D calcSwapchainExtent(const VkExtent2D &framebufferExtent) const;
    private:
    };




    class Surface {
    public:
        Surface() = default;
        [[nodiscard]]
        PresentModes getPresentModesFrom(PhysicalDevice physicalDevice) const;

        [[nodiscard]]
        SurfaceFormats getFormatsFrom(PhysicalDevice physicalDevice) const;

        [[nodiscard]]
        SurfaceCapabilities getCapabilitiesFrom(PhysicalDevice physicalDevice) const;

        [[nodiscard]]
        VkBool32 isSupportedBy(PhysicalDevice physicalDevice,
                               uint32_t queueFamilyIndex) const;

        //want formats type
        //want surface present mode type
        //want capabilities type
        explicit Surface(const Instance &instance, VkSurfaceKHR surface,
                         const VkAllocationCallbacks *pAllocator = nullptr);

        ~Surface();

        Surface(Surface &&rhs) noexcept;

//        was noexcept?
        Surface &operator=(Surface &&rhs) noexcept;

        Surface(const Surface &rhs) = delete;

        Surface &operator=(const Surface &rhs) = delete;

        [[nodiscard]]
        bool hasSwapchain() const;

        [[nodiscard]]
        const Swapchain *getSwapchain() const;

        Result createSwapchain(const SwapchainBuilder &builder);

        //TODO need to account for old swapchain when doing other swapchain things (probably put some members on swapchain that tell if images are acquired/still in flight)
        Result resizeSwapchain(const SwapchainBuilder &builder,
                               const VkExtent2D &extent);

        [[nodiscard]]
        VkSurfaceKHR get() const;

        Result
        setObjectName(const std::string &string, const vul::Device &device);

    private:
        const Instance *m_pInstance = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkSurfaceKHR m_handle = VK_NULL_HANDLE;
        std::unique_ptr<Swapchain> m_pSwapchain;
    };


}


#endif //VULKANWORKSPACE_SURFACE_H
