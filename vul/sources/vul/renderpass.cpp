//
// Created by Shae Bolt on 8/18/2021.
//

#include "renderpass.h"
#include "vul/colorformatutils.h"
#include "vul/vkassert.h"
#include <fmt/core.h>
#include <array>


vul::ColorAttachmentDescription::ColorAttachmentDescription(vul::Format format,
                                                            vul::ImageLayout initialLayout,
                                                            vul::ImageLayout finalLayout,
                                                            vul::AttachmentLoadOp loadOp,
                                                            vul::AttachmentStoreOp storeOp,
                                                            vul::SampleCountFlagBits samples,
                                                            vul::AttachmentDescriptionBitMask flags)
        :
        m_attachmentDescription{
                static_cast<VkAttachmentDescriptionFlags>(flags),
                static_cast<VkFormat>(format),
                static_cast<VkSampleCountFlagBits>(samples),
                static_cast<VkAttachmentLoadOp>(loadOp),
                static_cast<VkAttachmentStoreOp>(storeOp),
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                static_cast<VkImageLayout>(initialLayout),
                static_cast<VkImageLayout>(finalLayout)} {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
}


vul::ColorAttachmentDescription
vul::ColorAttachmentDescription::first(vul::Format format,
                                       vul::ImageLayout finalLayout,
                                       vul::SampleCountFlagBits samples,
                                       vul::AttachmentDescriptionBitMask flags) {
    return ColorAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            finalLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::ColorAttachmentDescription
vul::ColorAttachmentDescription::only(vul::Format format,
                                      vul::ImageLayout finalLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    return ColorAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            finalLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::ColorAttachmentDescription
vul::ColorAttachmentDescription::next(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::ImageLayout finalLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    return vul::ColorAttachmentDescription(vul::Format::R8g8b8Uscaled,
                                           vul::ImageLayout::DepthAttachmentStencilReadOnlyOptimal,
                                           vul::ImageLayout::DepthAttachmentStencilReadOnlyOptimal,
                                           vul::AttachmentLoadOp::Load,
                                           vul::AttachmentStoreOp::DontCare);
}

vul::ColorAttachmentDescription
vul::ColorAttachmentDescription::last(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    return vul::ColorAttachmentDescription(vul::Format::R8g8b8Uscaled,
                                           vul::ImageLayout::DepthAttachmentStencilReadOnlyOptimal,
                                           vul::ImageLayout::DepthAttachmentStencilReadOnlyOptimal,
                                           vul::AttachmentLoadOp::Load,
                                           vul::AttachmentStoreOp::DontCare);
}

const VkAttachmentDescription &vul::ColorAttachmentDescription::get() const {
    return m_attachmentDescription;
}

vul::PresentAttachmentDescription::PresentAttachmentDescription(
        vul::Format format, vul::ImageLayout initialLayout,
        vul::AttachmentLoadOp loadOp, vul::AttachmentStoreOp storeOp,
        vul::SampleCountFlagBits samples,
        vul::AttachmentDescriptionBitMask flags) : m_attachmentDescription{
        static_cast<VkAttachmentDescriptionFlags>(flags),
        static_cast<VkFormat>(format),
        static_cast<VkSampleCountFlagBits>(samples),
        static_cast<VkAttachmentLoadOp>(loadOp),
        static_cast<VkAttachmentStoreOp>(storeOp),
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        static_cast<VkImageLayout>(initialLayout),
        static_cast<VkImageLayout>(vul::ImageLayout::PresentSrcKhr)} {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
}

vul::PresentAttachmentDescription
vul::PresentAttachmentDescription::only(vul::Format format,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    return PresentAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::PresentAttachmentDescription
vul::PresentAttachmentDescription::last(vul::Format format,
                                        vul::ImageLayout initialLayout,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    return PresentAttachmentDescription(
            format,
            initialLayout,
            vul::AttachmentLoadOp::Load,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

const VkAttachmentDescription &vul::PresentAttachmentDescription::get() const {
    return m_attachmentDescription;
}

vul::UsedColorAttachmentDescription::UsedColorAttachmentDescription(
        vul::Format format, vul::ImageLayout initialLayout,
        vul::ImageLayout finalLayout,
        vul::AttachmentLoadOp loadOp,
        vul::AttachmentStoreOp storeOp, vul::SampleCountFlagBits samples,
        vul::AttachmentDescriptionBitMask flags) : m_attachmentDescription{
        static_cast<VkAttachmentDescriptionFlags>(flags),
        static_cast<VkFormat>(format),
        static_cast<VkSampleCountFlagBits>(samples),
        static_cast<VkAttachmentLoadOp>(loadOp),
        static_cast<VkAttachmentStoreOp>(storeOp),
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        static_cast<VkImageLayout>(initialLayout),
        static_cast<VkImageLayout>(finalLayout)} {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
}

vul::UsedColorAttachmentDescription
vul::UsedColorAttachmentDescription::only(vul::Format format,
                                          vul::ImageLayout finalLayout,
                                          vul::SampleCountFlagBits samples,
                                          vul::AttachmentDescriptionBitMask flags) {
    return UsedColorAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            finalLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::UsedColorAttachmentDescription
vul::UsedColorAttachmentDescription::last(vul::Format format,
                                          vul::ImageLayout initialLayout,
                                          vul::ImageLayout finalLayout,
                                          vul::SampleCountFlagBits samples,
                                          vul::AttachmentDescriptionBitMask flags) {

    return UsedColorAttachmentDescription(
            format,
            initialLayout,
            finalLayout,
            vul::AttachmentLoadOp::Load,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

const VkAttachmentDescription &
vul::UsedColorAttachmentDescription::get() const {
    return m_attachmentDescription;
}

vul::UsedDepthStencilAttachmentDescription::UsedDepthStencilAttachmentDescription(
        vul::Format format, vul::ImageLayout initialLayout,
        vul::ImageLayout finalLayout,
         vul::AttachmentLoadOp loadOp,
        vul::AttachmentStoreOp storeOp, vul::AttachmentLoadOp stencilLoadOp,
        vul::AttachmentStoreOp stencilStoreOp, vul::SampleCountFlagBits samples,
        vul::AttachmentDescriptionBitMask flags) : m_attachmentDescription{
        static_cast<VkAttachmentDescriptionFlags>(flags),
        static_cast<VkFormat>(format),
        static_cast<VkSampleCountFlagBits>(samples),
        static_cast<VkAttachmentLoadOp>(loadOp),
        static_cast<VkAttachmentStoreOp>(storeOp),
        static_cast<VkAttachmentLoadOp>(stencilLoadOp),
        static_cast<VkAttachmentStoreOp>(stencilStoreOp),
        static_cast<VkImageLayout>(initialLayout),
        static_cast<VkImageLayout>(finalLayout)} {
    VUL_ASSERT(vul::isDepthStencilFormat(format), fmt::format("Expected depth stencil format, found: {}", vul::to_string(format)).c_str());
}


vul::UsedDepthStencilAttachmentDescription
vul::UsedDepthStencilAttachmentDescription::only(vul::Format format,
                                                 vul::ImageLayout finalLayout,
                                                 vul::SampleCountFlagBits samples,
                                                 vul::AttachmentDescriptionBitMask flags) {
    return UsedDepthStencilAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            finalLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::UsedDepthStencilAttachmentDescription
vul::UsedDepthStencilAttachmentDescription::last(vul::Format format,
                                                 vul::ImageLayout finalLayout,
                                                 vul::ImageLayout initialLayout,
                                                 vul::SampleCountFlagBits samples,
                                                 vul::AttachmentDescriptionBitMask flags) {
    return UsedDepthStencilAttachmentDescription(
            format,
            initialLayout,
            finalLayout,
            vul::AttachmentLoadOp::Load,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::Load,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::UsedDepthStencilAttachmentDescription
vul::UsedDepthStencilAttachmentDescription::onlyDepth(vul::Format format,
                                                      vul::ImageLayout finalLayout,
                                                      vul::SampleCountFlagBits samples,
                                                      vul::AttachmentDescriptionBitMask flags) {
    return UsedDepthStencilAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            finalLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::DontCare,
            samples,
            flags
    );
}

vul::UsedDepthStencilAttachmentDescription
vul::UsedDepthStencilAttachmentDescription::lastDepth(vul::Format format,
                                                      vul::ImageLayout finalLayout,
                                                      vul::ImageLayout initialLayout,
                                                      vul::SampleCountFlagBits samples,
                                                      vul::AttachmentDescriptionBitMask flags) {
    return UsedDepthStencilAttachmentDescription(
            format,
            initialLayout,
            finalLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::DontCare,
            samples,
            flags
    );
}

const VkAttachmentDescription &
vul::UsedDepthStencilAttachmentDescription::get() const {
    return m_attachmentDescription;
}

vul::DepthStencilAttachmentDescription::DepthStencilAttachmentDescription(
        vul::Format format, vul::ImageLayout initialLayout,
        vul::AttachmentLoadOp loadOp, vul::AttachmentStoreOp storeOp,
        vul::AttachmentLoadOp stencilLoadOp,
        vul::AttachmentStoreOp stencilStoreOp, vul::SampleCountFlagBits samples,
        vul::AttachmentDescriptionBitMask flags): m_attachmentDescription{
        static_cast<VkAttachmentDescriptionFlags>(flags),
        static_cast<VkFormat>(format),
        static_cast<VkSampleCountFlagBits>(samples),
        static_cast<VkAttachmentLoadOp>(loadOp),
        static_cast<VkAttachmentStoreOp>(storeOp),
        static_cast<VkAttachmentLoadOp>(stencilLoadOp),
        static_cast<VkAttachmentStoreOp>(stencilStoreOp),
        static_cast<VkImageLayout>(initialLayout),
        static_cast<VkImageLayout>(vul::ImageLayout::DepthStencilAttachmentOptimal)} {
    VUL_ASSERT(vul::isDepthStencilFormat(format), fmt::format("Expected depth stencil format, found: {}", vul::to_string(format)).c_str());
}

vul::DepthStencilAttachmentDescription
vul::DepthStencilAttachmentDescription::only(vul::Format format,
                                             vul::SampleCountFlagBits samples,
                                             vul::AttachmentDescriptionBitMask flags) {
    return DepthStencilAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::DepthStencilAttachmentDescription
vul::DepthStencilAttachmentDescription::last(vul::Format format,
                                             vul::ImageLayout initialLayout,
                                             vul::SampleCountFlagBits samples,
                                             vul::AttachmentDescriptionBitMask flags) {
    return DepthStencilAttachmentDescription(
            format,
            initialLayout,
            vul::AttachmentLoadOp::Load,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::Load,
            vul::AttachmentStoreOp::Store,
            samples,
            flags
    );
}

vul::DepthStencilAttachmentDescription
vul::DepthStencilAttachmentDescription::onlyDepth(vul::Format format,
                                                  vul::SampleCountFlagBits samples,
                                                  vul::AttachmentDescriptionBitMask flags) {
    return DepthStencilAttachmentDescription(
            format,
            vul::ImageLayout::Undefined,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::DontCare,
            samples,
            flags
    );
}

vul::DepthStencilAttachmentDescription
vul::DepthStencilAttachmentDescription::lastDepth(vul::Format format,
                                                  vul::ImageLayout initialLayout,
                                                  vul::SampleCountFlagBits samples,
                                                  vul::AttachmentDescriptionBitMask flags) {
    return DepthStencilAttachmentDescription(
            format,
            initialLayout,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::DontCare,
            samples,
            flags
    );
}

const VkAttachmentDescription &
vul::DepthStencilAttachmentDescription::get() const {
    return m_attachmentDescription;
}

vul::TemporaryColorAttachmentDescription::TemporaryColorAttachmentDescription(
        vul::Format format, vul::ImageLayout initialLayout,
        vul::AttachmentLoadOp loadOp,vul::SampleCountFlagBits samples,
        vul::AttachmentDescriptionBitMask flags) : m_attachmentDescription{
        static_cast<VkAttachmentDescriptionFlags>(flags),
        static_cast<VkFormat>(format),
        static_cast<VkSampleCountFlagBits>(samples),
        static_cast<VkAttachmentLoadOp>(loadOp),
        static_cast<VkAttachmentStoreOp>(vul::AttachmentStoreOp::DontCare),
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        static_cast<VkImageLayout>(initialLayout),
        static_cast<VkImageLayout>(vul::ImageLayout::ColorAttachmentOptimal)} {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());

}

vul::TemporaryColorAttachmentDescription
vul::TemporaryColorAttachmentDescription::only(vul::Format format,
                                               vul::SampleCountFlagBits samples,
                                               vul::AttachmentDescriptionBitMask flags) {
    return vul::TemporaryColorAttachmentDescription(format,
                                                    vul::ImageLayout::Undefined,
                                                    vul::AttachmentLoadOp::Clear,
                                                    samples,
                                                    flags);
}

vul::TemporaryColorAttachmentDescription
vul::TemporaryColorAttachmentDescription::last(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::SampleCountFlagBits samples,
                                               vul::AttachmentDescriptionBitMask flags) {
    return vul::TemporaryColorAttachmentDescription(format,
                                                    initialLayout,
                                                    vul::AttachmentLoadOp::Clear,
                                                    samples,
                                                    flags);
}

const VkAttachmentDescription &
vul::TemporaryColorAttachmentDescription::get() const {
    return m_attachmentDescription;
}

vul::AttachmentDescription::AttachmentDescription(vul::Format format,
                                                  vul::ImageLayout initialLayout,
                                                  vul::ImageLayout finalLayout,
                                                  vul::AttachmentLoadOp loadOp,
                                                  vul::AttachmentStoreOp storeOp,
                                                  vul::AttachmentLoadOp stencilLoadOp,
                                                  vul::AttachmentStoreOp stencilStoreOp,
                                                  vul::SampleCountFlagBits samples,
                                                  vul::AttachmentDescriptionBitMask flags) :
        m_attachmentDescription{
        static_cast<VkAttachmentDescriptionFlags>(flags),
        static_cast<VkFormat>(format),
        static_cast<VkSampleCountFlagBits>(samples),
        static_cast<VkAttachmentLoadOp>(loadOp),
        static_cast<VkAttachmentStoreOp>(storeOp),
        static_cast<VkAttachmentLoadOp>(stencilLoadOp),
        static_cast<VkAttachmentStoreOp>(stencilStoreOp),
        static_cast<VkImageLayout>(initialLayout),
        static_cast<VkImageLayout>(finalLayout)} {

}

vul::AttachmentDescription
vul::AttachmentDescription::PresentTemp(vul::Format format,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      vul::ImageLayout::PresentSrcKhr,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::PresentLast(vul::Format format,
                                        vul::ImageLayout initialLayout,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      vul::ImageLayout::PresentSrcKhr,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::ColorFirst(vul::Format format,
                                       vul::ImageLayout finalLayout,
                                       vul::SampleCountFlagBits samples,
                                       vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      finalLayout,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::ColorNext(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::ImageLayout finalLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      finalLayout,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthStencilFirst(vul::Format format,
                                              vul::ImageLayout finalLayout,
                                              vul::SampleCountFlagBits samples,
                                              vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthStencilFormat(format), fmt::format("Expected depth stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      finalLayout,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthStencilNext(vul::Format format,
                                             vul::ImageLayout initialLayout,
                                             vul::ImageLayout finalLayout,
                                             vul::SampleCountFlagBits samples,
                                             vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthStencilFormat(format), fmt::format("Expected depth stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      finalLayout,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthFirst(vul::Format format,
                                       vul::ImageLayout finalLayout,
                                       vul::SampleCountFlagBits samples,
                                       vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthFormat(format), fmt::format("Expected depth format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      finalLayout,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthNext(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::ImageLayout finalLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthFormat(format), fmt::format("Expected depth format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      finalLayout,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::StencilFirst(vul::Format format,
                                         vul::ImageLayout finalLayout,
                                         vul::SampleCountFlagBits samples,
                                         vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isStencilFormat(format), fmt::format("Expected stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      finalLayout,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::StencilNext(vul::Format format,
                                        vul::ImageLayout initialLayout,
                                        vul::ImageLayout finalLayout,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isStencilFormat(format), fmt::format("Expected stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      finalLayout,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::ColorTemp(vul::Format format,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      vul::ImageLayout::ColorAttachmentOptimal,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::ColorLast(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isColorFormat(format), fmt::format("Expected color format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      vul::ImageLayout::ColorAttachmentOptimal,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthStencilTemp(vul::Format format,
                                             vul::SampleCountFlagBits samples,
                                             vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthStencilFormat(format), fmt::format("Expected depth stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      vul::ImageLayout::DepthStencilAttachmentOptimal,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthStencilLast(vul::Format format,
                                             vul::ImageLayout initialLayout,
                                             vul::SampleCountFlagBits samples,
                                             vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthStencilFormat(format), fmt::format("Expected depth stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      vul::ImageLayout::DepthStencilAttachmentOptimal,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthTemp(vul::Format format,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthFormat(format), fmt::format("Expected depth format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      vul::ImageLayout::DepthStencilAttachmentOptimal,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::DepthLast(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::SampleCountFlagBits samples,
                                      vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isDepthFormat(format), fmt::format("Expected depth format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      vul::ImageLayout::DepthStencilAttachmentOptimal,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::StencilTemp(vul::Format format,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isStencilFormat(format), fmt::format("Expected stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      vul::ImageLayout::Undefined,
                                      vul::ImageLayout::DepthStencilAttachmentOptimal,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      vul::AttachmentLoadOp::Clear,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

vul::AttachmentDescription
vul::AttachmentDescription::StencilLast(vul::Format format,
                                        vul::ImageLayout initialLayout,
                                        vul::SampleCountFlagBits samples,
                                        vul::AttachmentDescriptionBitMask flags) {
    VUL_ASSERT(vul::isStencilFormat(format), fmt::format("Expected stencil format, found: {}", vul::to_string(format)).c_str());
    return vul::AttachmentDescription(format,
                                      initialLayout,
                                      vul::ImageLayout::DepthStencilAttachmentOptimal,
                                      vul::AttachmentLoadOp::DontCare,
                                      vul::AttachmentStoreOp::DontCare,
                                      vul::AttachmentLoadOp::Load,
                                      vul::AttachmentStoreOp::Store,
                                      samples,
                                      flags);
}

const VkAttachmentDescription &vul::AttachmentDescription::get() const {
    return m_attachmentDescription;
}

bool vul::AttachmentDescription::isColor() const {
    return vul::isColorFormat(static_cast<vul::Format>(m_attachmentDescription.format));
}

bool vul::AttachmentDescription::isDepthStencil() const {
    return vul::isDepthStencilFormat(static_cast<vul::Format>(m_attachmentDescription.format));
}

bool vul::AttachmentDescription::isDepth() const {
    return vul::isDepthFormat(static_cast<vul::Format>(m_attachmentDescription.format));
}

bool vul::AttachmentDescription::isStencil() const {
    return vul::isStencilFormat(static_cast<vul::Format>(m_attachmentDescription.format)
}
