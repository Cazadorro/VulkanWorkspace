//
// Created by Shae Bolt on 6/6/2021.
//

#include "vul/surface.h"
#include "vul/swapchain.h"
#include "vul/imageview.h"
#include "vul/device.h"
#include "vul/physicaldevice.h"
#include "vul/instance.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"
#include <range/v3/algorithm/find.hpp>

bool
vul::operator==(const vul::SurfaceFormat &lhs, const vul::SurfaceFormat &rhs) {
    return lhs.colorSpace == rhs.colorSpace && lhs.format == rhs.format;
}

bool
vul::operator!=(const vul::SurfaceFormat &lhs, const vul::SurfaceFormat &rhs) {
    return !(lhs == rhs);
}

vul::Surface::Surface(const Instance &instance, VkSurfaceKHR surface,
                      const VkAllocationCallbacks *pAllocator) : m_pInstance(
        &instance), m_pAllocator(pAllocator), m_handle(surface) {

}

vul::PresentModes
vul::Surface::getPresentModesFrom(vul::PhysicalDevice physicalDevice) const {
    std::uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.get(), m_handle,
                                              &presentModeCount,
                                              nullptr);
    std::vector<vul::PresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.get(), m_handle,
                                              &presentModeCount,
                                              reinterpret_cast<VkPresentModeKHR *>(presentModes.data()));
    return vul::PresentModes(presentModes);
}

vul::SurfaceFormats
vul::Surface::getFormatsFrom(vul::PhysicalDevice physicalDevice) const {
    std::uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.get(), m_handle,
                                         &formatCount,
                                         nullptr);
    std::vector<SurfaceFormat> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.get(), m_handle,
                                         &formatCount,
                                         reinterpret_cast<VkSurfaceFormatKHR *>(formats.data()));
    return vul::SurfaceFormats(formats);
}

vul::SurfaceCapabilities
vul::Surface::getCapabilitiesFrom(vul::PhysicalDevice physicalDevice) const {
    vul::SurfaceCapabilities capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physicalDevice.get(), m_handle,
            reinterpret_cast<VkSurfaceCapabilitiesKHR *>(&capabilities));
    return capabilities;
}

VkBool32 vul::Surface::isSupportedBy(vul::PhysicalDevice physicalDevice,
                                     uint32_t queueFamilyIndex) const {
    VkBool32 supportsPresentationToSurface = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.get(), queueFamilyIndex,
                                         m_handle,
                                         &supportsPresentationToSurface);
    return supportsPresentationToSurface;
}

vul::Surface::~Surface() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_pInstance->get(), m_handle, m_pAllocator);
    }
}

vul::Surface::Surface(vul::Surface &&rhs) noexcept : Surface(){
    using std::swap;
    swap(m_pInstance, rhs.m_pInstance);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::Surface &vul::Surface::operator=(vul::Surface &&rhs) noexcept {
    using std::swap;
    swap(m_pInstance, rhs.m_pInstance);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}



VkSurfaceKHR vul::Surface::get() const {
    return m_handle;
}

vul::Result vul::Surface::setObjectName(const std::string &string,
                                        const vul::Device &device) {
    return device.setObjectName(m_handle, string);
}


vul::SurfaceFormats::SurfaceFormats(
        std::vector<SurfaceFormat> surfaceFormats) : m_surfaceFormats(
        std::move(surfaceFormats)) {

}

bool vul::SurfaceFormats::isSupported(
        const SurfaceFormat &surfaceFormat) const {
    return ranges::find(m_surfaceFormats, surfaceFormat) !=
           m_surfaceFormats.end();
}

std::optional<vul::SurfaceFormat> vul::SurfaceFormats::getBestSupported(
        const std::vector<SurfaceFormat> &rankedSurfaceFormats) const {
    for (const auto &rankedFormat : rankedSurfaceFormats) {
        if (isSupported(rankedFormat)) {
            return rankedFormat;
        }
    }
    return std::nullopt;
}


vul::PresentModes::PresentModes(std::vector<vul::PresentModeKHR> presentModes)
        : m_presentModes(std::move(presentModes)) {

}

bool
vul::PresentModes::isSupported(const vul::PresentModeKHR &presentMode) const {
    return ranges::find(m_presentModes, presentMode) != m_presentModes.end();
}

std::optional<vul::PresentModeKHR> vul::PresentModes::getBestSupported(
        const std::vector<vul::PresentModeKHR> &rankedPresentModes) const {
    for (const auto &rankedPresentMode : rankedPresentModes) {
        if (isSupported(rankedPresentMode)) {
            return rankedPresentMode;
        }
    }
    return std::nullopt;
}





std::uint32_t vul::SurfaceCapabilities::calcSwapchainImageCount() const {
    std::uint32_t imageCount = minImageCount + 1;
    if (maxImageCount > 0 &&
        imageCount > maxImageCount) {
        imageCount = maxImageCount;
    }
    return imageCount;
}

VkExtent2D vul::SurfaceCapabilities::calcSwapchainExtent(
        const VkExtent2D &framebufferExtent) const {
    if (currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return currentExtent;
    } else {
        VkExtent2D extent;
        extent.width = std::max(minImageExtent.width,
                                std::min(
                                        maxImageExtent.width,
                                        framebufferExtent.width));
        extent.height = std::max(minImageExtent.height,
                                 std::min(
                                         maxImageExtent.height,
                                         framebufferExtent.height));
        return extent;
    }
}
