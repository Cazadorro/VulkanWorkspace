//
// Created by Shae Bolt on 8/31/2021.
//

#include "framebuffer.h"
#include "vul/renderpass.h"
#include "vul/imageview.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/temparrayproxy.h"
#include <uul/enumflags.h>
#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>



vul::Framebuffer::Framebuffer(const vul::Device &device, VkFramebuffer handle,
                              const VkAllocationCallbacks *pAllocator): m_pDevice(
        &device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkFramebuffer vul::Framebuffer::get() const {
    return m_handle;
}

vul::Framebuffer::~Framebuffer() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::Framebuffer::Framebuffer(vul::Framebuffer &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::Framebuffer &vul::Framebuffer::operator=(vul::Framebuffer &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::Framebuffer::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::Framebuffer::Framebuffer() = default;


vul::FramebufferBuilder::FramebufferBuilder(const vul::Device &device,
                                            const VkAllocationCallbacks *pAllocator) : m_pDevice(&device), m_pAllocator(pAllocator){
    m_createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
}

void vul::FramebufferBuilder::setCreateFlags(uul::EnumFlags<vul::SamplerCreateFlagBits> flags) {
    m_createInfo.flags = flags.to_underlying();
}

void vul::FramebufferBuilder::setRenderPass(const vul::RenderPass &renderPass) {
    m_createInfo.renderPass = renderPass.get();
}

void vul::FramebufferBuilder::setAttachments(
        const vul::TempArrayProxy<const vul::ImageView> &attachments) {
    m_attachments = attachments | ranges::views::transform([](auto& attachment){return attachment.get();}) | ranges::to<std::vector>();
    m_createInfo.attachmentCount = static_cast<std::uint32_t>(m_attachments.size());
    m_createInfo.pAttachments = m_attachments.data();
}

void vul::FramebufferBuilder::setAttachments(
        const vul::TempArrayProxy<std::reference_wrapper<const ImageView>> &attachments) {
    m_attachments = attachments | ranges::views::transform([](auto& attachment){return attachment.get().get();}) | ranges::to<std::vector>();
    m_createInfo.attachmentCount = static_cast<std::uint32_t>(m_attachments.size());
    m_createInfo.pAttachments = m_attachments.data();
}

void vul::FramebufferBuilder::setAttachments(
        const vul::TempArrayProxy<const vul::ImageView *> &attachments) {
    m_attachments = attachments | ranges::views::transform([](auto& attachment){return attachment->get();}) | ranges::to<std::vector>();
    m_createInfo.attachmentCount = static_cast<std::uint32_t>(m_attachments.size());
    m_createInfo.pAttachments = m_attachments.data();
}

void vul::FramebufferBuilder::setDimensions(std::uint32_t width,
                                            std::uint32_t height,
                                            std::uint32_t layers) {
    m_createInfo.width = width;
    m_createInfo.height = height;
    m_createInfo.layers = layers;
}

void vul::FramebufferBuilder::setNext(const void *pNext) {
    m_createInfo.pNext = pNext;
}
vul::ExpectedResult<vul::Framebuffer> vul::FramebufferBuilder::create() const {
    VkFramebuffer framebuffer;
    auto result = static_cast<Result>(vkCreateFramebuffer(m_pDevice->get(), &m_createInfo, m_pAllocator, &framebuffer));
    return {result, Framebuffer(*m_pDevice, framebuffer, m_pAllocator)};
}

void vul::FramebufferBuilder::setDimensions(VkExtent2D extent,
                                            std::uint32_t layers) {
    setDimensions(extent.width, extent.height, layers);
}
