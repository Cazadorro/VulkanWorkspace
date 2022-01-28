//
// Created by Shae Bolt on 8/17/2021.
//

#include "vul/swapchain.h"
#include "vul/semaphore.h"
#include "vul/surface.h"
#include "vul/imageview.h"
#include "vul/queue.h"
#include "vul/device.h"
#include "vul/expectedresult.h"


vul::SwapchainBuilder::SwapchainBuilder(const vul::Device &device,
                                        const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator) {
    m_createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    m_createInfo.imageArrayLayers = 1;
    m_createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    m_createInfo.clipped = VK_TRUE;
}

vul::SwapchainBuilder &vul::SwapchainBuilder::pNext(const void *value) {
    m_createInfo.pNext = value;
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::flags(vul::SwapchainCreateFlagBitMask value) {
    m_createInfo.flags = value.to_underlying();
    return *this;
}

vul::SwapchainBuilder &vul::SwapchainBuilder::minImageCount(uint32_t value) {
    m_createInfo.minImageCount = value;
    return *this;
}

vul::SwapchainBuilder &vul::SwapchainBuilder::imageFormat(vul::Format value) {
    m_createInfo.imageFormat = static_cast<VkFormat>(value);
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::imageColorSpace(vul::ColorSpaceKHR value) {
    m_createInfo.imageColorSpace = static_cast<VkColorSpaceKHR>(value);
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::surfaceFormat(const vul::SurfaceFormat &value) {
    return imageFormat(value.format).imageColorSpace(value.colorSpace);
}

vul::SwapchainBuilder &vul::SwapchainBuilder::imageExtent(VkExtent2D value) {
    m_createInfo.imageExtent = value;
    return *this;
}

vul::SwapchainBuilder &vul::SwapchainBuilder::imageArrayLayers(uint32_t value) {
    m_createInfo.imageArrayLayers = value;
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::imageUsage(vul::ImageUsageBitMask value) {
    //always use as color attachment, otherwise quite pointless.
    m_createInfo.imageUsage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | value.to_underlying();
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::imageSharingMode(vul::SharingMode value) {
    m_createInfo.imageSharingMode = static_cast<VkSharingMode>(value);
    return *this;
}

vul::SwapchainBuilder &vul::SwapchainBuilder::queueFamilyIndices(
        const gsl::span<std::uint32_t> &value) {
    m_createInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(value.size());
    m_createInfo.pQueueFamilyIndices = value.data();
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::preTransform(vul::SurfaceTransformFlagBitMask value) {
    //the types of the enum bits and the actual flag types... aren't the same...
    m_createInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(value.to_underlying());
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::compositeAlpha(vul::CompositeAlphaFlagBitMask value) {
    m_createInfo.compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(value.to_underlying());
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::presentMode(vul::PresentModeKHR value) {
    m_createInfo.presentMode = static_cast<VkPresentModeKHR>(value);
    return *this;
}

vul::SwapchainBuilder &vul::SwapchainBuilder::clipped(VkBool32 value) {
    m_createInfo.clipped = value;
    return *this;
}

vul::SwapchainBuilder &
vul::SwapchainBuilder::oldSwapchain(const vul::Swapchain &value) {
    m_createInfo.oldSwapchain = value.get();
    return *this;
}

vul::ExpectedResult<vul::Swapchain>
vul::SwapchainBuilder::create(const vul::Surface &surface) const {
    auto createInfo = m_createInfo;
    createInfo.surface = surface.get();
    VkSwapchainKHR swapchain;
    auto result = static_cast<Result>(vkCreateSwapchainKHR(m_pDevice->get(),
                                                           &createInfo,
                                                           m_pAllocator,
                                                           &swapchain));
    if (result != Result::Success) {
        return {result, {}};
    }

    std::uint32_t imageCount;
    vkGetSwapchainImagesKHR(m_pDevice->get(), swapchain, &imageCount, nullptr);
    std::vector<VkImage> swapChainImages(imageCount);
    result = static_cast<Result>(vkGetSwapchainImagesKHR(m_pDevice->get(),
                                                         swapchain, &imageCount,
                                                         swapChainImages.data()));

    if (result != Result::Success) {
        return {result, {}};
    }
    std::vector<ImageView> imageViews;
    imageViews.reserve(swapChainImages.size());
    ImageViewBuilder viewBuilder(*m_pDevice, m_pAllocator);
    SubresourceRange subresourceRange;
    subresourceRange.aspectMask = vul::ImageAspectFlagBits::ColorBit;
    viewBuilder.viewType(ImageViewType::_2D)
            .format(static_cast<Format>(createInfo.imageFormat))
            .subresourceRange(subresourceRange);

    for (const auto swapchainImage : swapChainImages) {
        auto[viewResult, view] = viewBuilder.create(swapchainImage);
        if (viewResult != Result::Success) {
            return {result, {}};
        }
        imageViews.push_back(std::move(view));
    }
    return {result, Swapchain(*m_pDevice, swapchain, m_createInfo.imageExtent,
                              static_cast<Format>(m_createInfo.imageFormat),
                              swapChainImages,
                              std::move(imageViews),
                              m_pAllocator)};
}


vul::Swapchain::Swapchain(const vul::Device &device, VkSwapchainKHR handle,
                          VkExtent2D extent, vul::Format format,
                          std::vector<VkImage> images,
                          std::vector<ImageView> imageViews,
                          const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator), m_handle(handle),
          m_extent(extent),
          m_format(format), m_images(std::move(images)),
          m_imageViews(std::move(imageViews)) {

}

VkSwapchainKHR vul::Swapchain::get() const {
    return m_handle;
}

vul::Swapchain::~Swapchain() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::Swapchain::Swapchain(vul::Swapchain &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_extent, rhs.m_extent);
    swap(m_format, rhs.m_format);
    swap(m_images, rhs.m_images);
    swap(m_imageViews, rhs.m_imageViews);
}

vul::Swapchain &vul::Swapchain::operator=(vul::Swapchain &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_extent, rhs.m_extent);
    swap(m_format, rhs.m_format);
    swap(m_images, rhs.m_images);
    swap(m_imageViews, rhs.m_imageViews);
    return *this;
}

VkExtent2D vul::Swapchain::getExtent() const {
    return m_extent;
}

VkExtent3D vul::Swapchain::getExtent3D() const {
    return VkExtent3D{m_extent.width, m_extent.height, 1};
}

vul::Format vul::Swapchain::getFormat() const {
    return m_format;
}

std::size_t vul::Swapchain::getImageCount() const {
    return m_images.size();
}

vul::Result vul::Swapchain::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::ExpectedResult<std::uint32_t> vul::Swapchain::acquireNextImage(
        const BinarySemaphore& presentationFinishedSemaphore,
        std::uint64_t timeout_ns, VkFence fence) const{
    std::uint32_t imageIndex;
    auto result = static_cast<Result>(vkAcquireNextImageKHR(m_pDevice->get(), m_handle, timeout_ns, presentationFinishedSemaphore.get(), fence, &imageIndex));
    return {result, std::uint32_t{imageIndex}};
}

vul::Result vul::Swapchain::present(const vul::Queue &presentQueue,
                                    const BinarySemaphore& renderingFinishedSemaphore,
                                    std::uint32_t swapchainImageIndex) const{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &(renderingFinishedSemaphore.get());

    VkSwapchainKHR swapChains[] = {m_handle};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pResults = nullptr;

    presentInfo.pImageIndices = &swapchainImageIndex;
    return static_cast<Result>(vkQueuePresentKHR(presentQueue.get(), &presentInfo));
}

const std::vector<vul::ImageView>& vul::Swapchain::getImageViews() const {
    return m_imageViews;
}

const std::vector<VkImage> &vul::Swapchain::getImages() const {
    return m_images;
}
