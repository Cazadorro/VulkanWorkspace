//
// Created by Shae Bolt on 6/11/2021.
//

#ifndef VULKANWORKSPACE_IMAGEVIEW_H
#define VULKANWORKSPACE_IMAGEVIEW_H
#include "vul/bitmasks.h"
#include "vul/enumsfwd.h"
#include "image.h"

#include <vulkan/vulkan.h>
#include <cstdint>
namespace vul {
    template<typename T>
    class ExpectedResult;
    class Device;

//        VkImageViewCreateInfo viewInfo = {};
//    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//    viewInfo.image = image;
//    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//    viewInfo.format = format;
//    viewInfo.subresourceRange.aspectMask = aspectFlags;
//    viewInfo.subresourceRange.baseMipLevel = 0;
//    viewInfo.subresourceRange.levelCount = 1;
//    viewInfo.subresourceRange.baseArrayLayer = 0;
//    viewInfo.subresourceRange.layerCount = 1;
//    viewInfo.components = components;
//
//    VkImageView imageView;
//    VUL_EXCEPT_RESULT(
//            vkCreateImageView(device, &viewInfo, nullptr, &imageView));
//    return vul::make_handle(device, imageView);
    class Sampler;
    class ImageView {
    public:
        ImageView() = default;
        ImageView(const Device& device, VkImageView handle, const VkAllocationCallbacks *pAllocator = nullptr);
        ImageView(ImageView &&rhs) noexcept;
//        was noexcept?
        ImageView &operator=(ImageView &&rhs) noexcept;

        ImageView(const ImageView &rhs) = delete;

        ImageView &operator=(ImageView &rhs) = delete;
        ~ImageView();

        [[nodiscard]]
        VkImageView get() const;

        Result setObjectName(const std::string &string);

        [[nodiscard]]
        VkDescriptorImageInfo createDescriptorInfo(VkSampler sampler, vul::ImageLayout layout) const;
        [[nodiscard]]
        VkDescriptorImageInfo createDescriptorInfo(const Sampler& sampler, vul::ImageLayout layout) const;
        [[nodiscard]]
        VkDescriptorImageInfo createStorageWriteInfo() const;
    private:
        const Device* m_pDevice = nullptr;
        VkImageView m_handle = VK_NULL_HANDLE;
        const VkAllocationCallbacks *m_pAllocator = nullptr;

    };


    class SubresourceRange{
    public:
        vul::ImageAspectBitMask aspectMask;
        std::uint32_t baseMipLevel = 0;
        std::uint32_t levelCount = 1;
        std::uint32_t baseArrayLayer = 0;
        std::uint32_t layerCount = 1;
        [[nodiscard]]
        operator VkImageSubresourceRange () const;
    private:
    };
    class ImageViewBuilder{
    public:
        explicit ImageViewBuilder(const Device& device, const VkAllocationCallbacks *pAllocator = nullptr);
       // ImageViewBuilder& sType(VkStructureType value);
        ImageViewBuilder& pNext(const void* value);
        ImageViewBuilder& flags(vul::ImageViewCreateBitMask value);
        ImageViewBuilder& image(VkImage value);
        ImageViewBuilder& viewType(vul::ImageViewType value);
        ImageViewBuilder& format(vul::Format value);
        ImageViewBuilder& components(VkComponentMapping value);
        ImageViewBuilder& subresourceRange(SubresourceRange value);

        [[nodiscard]]
        ExpectedResult<ImageView> create() const;
        [[nodiscard]]
        ExpectedResult<ImageView> create(VkImage image) const;
    private:
        const Device* m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkImageViewCreateInfo m_createInfo = {};

    };


}


#endif //VULKANWORKSPACE_IMAGEVIEW_H
