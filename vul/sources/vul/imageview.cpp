//
// Created by Shae Bolt on 6/11/2021.
//

#include "imageview.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/bitmasks.h"
#include "vul/enums.h"
#include "vul/sampler.h"

vul::ImageViewBuilder::ImageViewBuilder(const vul::Device &device,
                                        const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator) {
    m_createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
}

vul::ImageViewBuilder &vul::ImageViewBuilder::pNext(const void *value) {
    m_createInfo.pNext = value;
    return *this;
}

vul::ImageViewBuilder &
vul::ImageViewBuilder::flags(vul::ImageViewCreateBitMask value) {
    m_createInfo.flags = value.to_underlying();
    return *this;
}

vul::ImageViewBuilder &vul::ImageViewBuilder::image(VkImage value) {
    m_createInfo.image = value;
    return *this;
}

vul::ImageViewBuilder &
vul::ImageViewBuilder::viewType(vul::ImageViewType value) {
    m_createInfo.viewType = static_cast<VkImageViewType>(value);
    return *this;
}

vul::ImageViewBuilder &vul::ImageViewBuilder::format(vul::Format value) {
    m_createInfo.format = static_cast<VkFormat>(value);
    return *this;
}

vul::ImageViewBuilder &
vul::ImageViewBuilder::components(VkComponentMapping value) {
    m_createInfo.components = value;
    return *this;
}

vul::ImageViewBuilder &
vul::ImageViewBuilder::subresourceRange(SubresourceRange value) {
    m_createInfo.subresourceRange = value;
    return *this;
}

vul::ExpectedResult<vul::ImageView>
vul::ImageViewBuilder::create() const {
    VkImageView imageView;
    auto result = static_cast<Result>(vkCreateImageView(m_pDevice->get(), &m_createInfo,
                                    m_pAllocator, &imageView));
    return {result, ImageView(*m_pDevice, imageView, m_pAllocator)};
}

vul::ExpectedResult<vul::ImageView>
vul::ImageViewBuilder::create(VkImage image) const {
    auto createInfo = m_createInfo;
    createInfo.image = image;
    VkImageView imageView;
    auto result = static_cast<Result>(vkCreateImageView(m_pDevice->get(), &createInfo,
                                                        m_pAllocator, &imageView));
    return {result, ImageView(*m_pDevice, imageView, m_pAllocator)};
}

vul::ImageView::ImageView(const vul::Device &device, VkImageView handle,
                          const VkAllocationCallbacks *pAllocator) : m_pDevice(&device), m_handle(handle), m_pAllocator(pAllocator) {

}

vul::ImageView::ImageView(vul::ImageView &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::ImageView &vul::ImageView::operator=(vul::ImageView &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::ImageView::~ImageView() {
    if(m_handle != VK_NULL_HANDLE) {
        vkDestroyImageView(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

VkImageView vul::ImageView::get() const {
    return m_handle;
}

vul::Result vul::ImageView::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

VkDescriptorImageInfo
vul::ImageView::createDescriptorInfo(const vul::Sampler &sampler,
                                     vul::ImageLayout layout) const {
    return createDescriptorInfo(sampler.get(), layout);
}

VkDescriptorImageInfo vul::ImageView::createStorageWriteInfo() const {
    return createDescriptorInfo(VK_NULL_HANDLE, vul::ImageLayout::General);
}

VkDescriptorImageInfo vul::ImageView::createDescriptorInfo(VkSampler sampler,
                                                           vul::ImageLayout layout) const {
    return {sampler, m_handle, vul::get(layout)};
}

vul::SubresourceRange::operator VkImageSubresourceRange() const {
    return *reinterpret_cast<const VkImageSubresourceRange*>(this);
}
