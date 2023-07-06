//
// Created by Shae Bolt on 8/18/2021.
//

#include "renderpass.h"
#include "vul/device.h"
#include "vul/colorformatutils.h"
#include "vul/enums.h"
#include "vul/expectedresult.h"
#include "vul/containerutils.h"
#include <uul/enumflags.h>
#include <uul/assert.h>
#include <fmt/core.h>
#include <range/v3/view/enumerate.hpp>
#include <array>
#include <unordered_map>

vul::AttachmentDescription::AttachmentDescription(vul::Format format,
                                                  vul::ImageLayout initialLayout,
                                                  vul::ImageLayout finalLayout,
                                                  vul::AttachmentLoadOp loadOp,
                                                  vul::AttachmentStoreOp storeOp,
                                                  vul::AttachmentLoadOp stencilLoadOp,
                                                  vul::AttachmentStoreOp stencilStoreOp,
                                                  vul::SampleCountFlagBits samples,
                                                  uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags)
        :
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
                                        uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isColorFormat(format),
               fmt::format("Expected color format, found: {}",
                           vul::to_string(format)).c_str());
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
                                        uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isColorFormat(format),
               fmt::format("Expected color format, found: {}",
                           vul::to_string(format)).c_str());
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
                                       uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isColorFormat(format),
               fmt::format("Expected color format, found: {}",
                           vul::to_string(format)).c_str());
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
                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isColorFormat(format),
               fmt::format("Expected color format, found: {}",
                           vul::to_string(format)).c_str());
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
                                              uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthStencilFormat(format),
               fmt::format("Expected depth stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                             uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthStencilFormat(format),
               fmt::format("Expected depth stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                       uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthFormat(format),
               fmt::format("Expected depth format, found: {}",
                           vul::to_string(format)).c_str());
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
                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthFormat(format),
               fmt::format("Expected depth format, found: {}",
                           vul::to_string(format)).c_str());
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
                                         uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isStencilFormat(format),
               fmt::format("Expected stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                        uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isStencilFormat(format),
               fmt::format("Expected stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isColorFormat(format),
               fmt::format("Expected color format, found: {}",
                           vul::to_string(format)).c_str());
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
                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isColorFormat(format),
               fmt::format("Expected color format, found: {}",
                           vul::to_string(format)).c_str());
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
                                             uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthStencilFormat(format),
               fmt::format("Expected depth stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                             uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthStencilFormat(format),
               fmt::format("Expected depth stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthFormat(format),
               fmt::format("Expected depth format, found: {}",
                           vul::to_string(format)).c_str());
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
vul::AttachmentDescription::DepthReadOnly(vul::Format format, vul::SampleCountFlagBits samples,
                                          uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthFormat(format),
               fmt::format("Expected depth format, found: {}",
                           vul::to_string(format)).c_str());
    return {format,
            vul::ImageLayout::Undefined,
            vul::ImageLayout::DepthStencilReadOnlyOptimal,
            vul::AttachmentLoadOp::Clear,
            vul::AttachmentStoreOp::Store,
            vul::AttachmentLoadOp::DontCare,
            vul::AttachmentStoreOp::DontCare,
            samples,
            flags};
}


vul::AttachmentDescription
vul::AttachmentDescription::DepthLast(vul::Format format,
                                      vul::ImageLayout initialLayout,
                                      vul::SampleCountFlagBits samples,
                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isDepthFormat(format),
               fmt::format("Expected depth format, found: {}",
                           vul::to_string(format)).c_str());
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
                                        uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isStencilFormat(format),
               fmt::format("Expected stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
                                        uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags) {
    UUL_ASSERT(vul::isStencilFormat(format),
               fmt::format("Expected stencil format, found: {}",
                           vul::to_string(format)).c_str());
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
    return vul::isColorFormat(
            static_cast<vul::Format>(m_attachmentDescription.format));
}

bool vul::AttachmentDescription::isDepthStencil() const {
    return vul::isDepthStencilFormat(
            static_cast<vul::Format>(m_attachmentDescription.format));
}

bool vul::AttachmentDescription::isDepth() const {
    return vul::isDepthFormat(
            static_cast<vul::Format>(m_attachmentDescription.format));
}

bool vul::AttachmentDescription::isStencil() const {
    return vul::isStencilFormat(
            static_cast<vul::Format>(m_attachmentDescription.format));
}


vul::SubpassNode &vul::SubpassNode::setWrites(
        const gsl::span<const std::uint32_t> &attachmentIndices) {
    m_colorAttachmentWriteReferences.clear();
    //check for zero, possible only writes to depth attachment.
    m_colorAttachmentWriteReferences.reserve(
            attachmentIndices.empty() ? 0 : attachmentIndices.size() - 1);
    m_depthAttachmentWriteReference.reset();
    for (auto index: attachmentIndices) {
        if (m_parentGraph->getAttachmentAt(index).isColor()) {
            m_colorAttachmentWriteReferences.emplace_back(index,
                                                          vul::ImageLayout::ColorAttachmentOptimal);
        } else if (!m_depthAttachmentWriteReference.has_value()) {
            m_depthAttachmentWriteReference = {index,
                                               vul::ImageLayout::DepthStencilAttachmentOptimal};
        } else {
            UUL_ASSERT(
                    m_depthAttachmentWriteReference.value().attachment == index,
                    "Can't write to multiple depth stencils at once");
        }
    }
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setReads(
        const gsl::span<const std::uint32_t> &attachmentIndices) {
    m_inputAttachmentReferences.clear();
    m_inputAttachmentReferences.reserve(attachmentIndices.size());
    for (auto index: attachmentIndices) {
        m_inputAttachmentReferences.emplace_back(index,
                                                 vul::ImageLayout::ShaderReadOnlyOptimal);
    }
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setWrites(
        const gsl::span<const std::uint32_t> &attachmentIndices,
        const gsl::span<const std::uint32_t> &resolveIndices) {
    setWrites(attachmentIndices);
    m_resolveAttachmentReferences.clear();
    m_resolveAttachmentReferences.reserve(
            m_colorAttachmentWriteReferences.size());
    for (std::size_t i = 0; i < m_colorAttachmentWriteReferences.size(); ++i) {
        m_resolveAttachmentReferences.emplace_back(VK_ATTACHMENT_UNUSED,
                                                   vul::ImageLayout::ColorAttachmentOptimal);
    }
    for (auto index: resolveIndices) {
        UUL_ASSERT(m_parentGraph->getAttachmentAt(index).isColor(),
                   "Attachment must be color to resolve");
        m_resolveAttachmentReferences[index] = AttachmentReference(index,
                                                                   vul::ImageLayout::ColorAttachmentOptimal);
    }
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setPreserve(
        const gsl::span<const std::uint32_t> &attachmentIndices) {
    m_preserveAttachmentReferences = std::vector<std::uint32_t>(
            attachmentIndices.begin(), attachmentIndices.end());
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setDependsWriteAfterRead(
        const gsl::span<const std::uint32_t> &subpassIndices) {
    m_warDependencies = std::vector<std::uint32_t>(subpassIndices.begin(),
                                                   subpassIndices.end());
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setDependsReadAfterWrite(
        const gsl::span<const std::uint32_t> &subpassIndices) {
    m_rawDependencies = std::vector<std::uint32_t>(subpassIndices.begin(),
                                                   subpassIndices.end());
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setDependsWriteAfterWrite(
        const gsl::span<const std::uint32_t> &subpassIndices) {
    m_wawDependencies = std::vector<std::uint32_t>(subpassIndices.begin(),
                                                   subpassIndices.end());
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setWrites(
        const std::initializer_list<const std::uint32_t> &attachmentIndices) {
    return setWrites(gsl::make_span(std::data(attachmentIndices),
                                    attachmentIndices.size()));
}

vul::SubpassNode &vul::SubpassNode::setReads(
        const std::initializer_list<const std::uint32_t> &attachmentIndices) {
    return setReads(gsl::make_span(std::data(attachmentIndices),
                                   attachmentIndices.size()));
}

vul::SubpassNode &vul::SubpassNode::setWrites(
        const std::initializer_list<const std::uint32_t> &attachmentIndices,
        const std::initializer_list<const std::uint32_t> &resolveIndices) {
    return setWrites(gsl::make_span(std::data(attachmentIndices),
                                    attachmentIndices.size()),
                     gsl::make_span(std::data(resolveIndices),
                                    resolveIndices.size()));
}


vul::SubpassNode &vul::SubpassNode::setPreserve(
        const std::initializer_list<const std::uint32_t> &attachmentIndices) {
    return setPreserve(gsl::make_span(std::data(attachmentIndices),
                                      attachmentIndices.size()));
}


vul::SubpassNode &vul::SubpassNode::setDependsWriteAfterRead(
        const std::initializer_list<const std::uint32_t> &subpassIndices) {
    return setDependsWriteAfterRead(
            gsl::make_span(std::data(subpassIndices), subpassIndices.size()));
}

vul::SubpassNode &vul::SubpassNode::setDependsReadAfterWrite(
        const std::initializer_list<const std::uint32_t> &subpassIndices) {
    return setDependsReadAfterWrite(
            gsl::make_span(std::data(subpassIndices), subpassIndices.size()));
}

vul::SubpassNode &vul::SubpassNode::setDependsWriteAfterWrite(
        const std::initializer_list<const std::uint32_t> &subpassIndices) {
    return setDependsWriteAfterWrite(
            gsl::make_span(std::data(subpassIndices), subpassIndices.size()));
}

void vul::SubpassNode::reset() {
    m_warDependencies.clear();
    m_rawDependencies.clear();
    m_wawDependencies.clear();
    m_colorAttachmentWriteReferences.clear();
    m_inputAttachmentReferences.clear();
    m_resolveAttachmentReferences.clear();
    m_depthAttachmentWriteReference.reset();
    m_preExternalDependency.reset();
    m_postExternalDependency.reset();
}


vul::SubpassNode::SubpassNode(SubpassGraph &parentGraph,
                              std::uint32_t subpassIndex) : m_parentGraph(
        &parentGraph), m_subpassIndex(subpassIndex) {

}

VkSubpassDescription vul::SubpassNode::createDescription(
        uul::EnumFlags<vul::SubpassDescriptionFlagBits> flags) const {
    VkSubpassDescription subpassDescription = {};
    subpassDescription.flags = flags.to_underlying();
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount = static_cast<std::uint32_t>(m_inputAttachmentReferences.size());
    subpassDescription.pInputAttachments = reinterpret_cast<const VkAttachmentReference *>(m_inputAttachmentReferences.data());
    subpassDescription.colorAttachmentCount = static_cast<std::uint32_t>(m_colorAttachmentWriteReferences.size());
    subpassDescription.pColorAttachments = reinterpret_cast<const VkAttachmentReference *>(m_colorAttachmentWriteReferences.data());
    subpassDescription.pResolveAttachments = reinterpret_cast<const VkAttachmentReference *>(m_resolveAttachmentReferences.data());
    if (m_depthAttachmentWriteReference.has_value()) {
        subpassDescription.pDepthStencilAttachment = reinterpret_cast<const VkAttachmentReference *>(&m_depthAttachmentWriteReference.value());
    } else {
        subpassDescription.pDepthStencilAttachment = nullptr;
    }
    subpassDescription.preserveAttachmentCount = static_cast<std::uint32_t>(m_preserveAttachmentReferences.size());
    subpassDescription.pPreserveAttachments = m_preserveAttachmentReferences.data();
    return subpassDescription;
}

std::vector<VkSubpassDependency>
    vul::SubpassNode::createSubpassDependencies() const {

    std::unordered_map<std::uint32_t, VkSubpassDependency> dependencies;

    for (const auto &dependencyIndex: m_warDependencies) {
        if (vul::contains(dependencies, dependencyIndex)) {
            VkSubpassDependency dependency = {};
            dependency.srcSubpass = dependencyIndex;
            dependency.dstSubpass = m_subpassIndex;
            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies[dependencyIndex] = dependency;
        }
        auto &otherSubpass = m_parentGraph->subpassAt(dependencyIndex);
        if (m_depthAttachmentWriteReference.has_value()) {
            if (vul::contains_if(otherSubpass.m_inputAttachmentReferences,
                                 [attachment = m_depthAttachmentWriteReference.value()](
                                         auto attachmentReference) {
                                     return attachmentReference.attachment ==
                                            attachment.attachment;
                                 })) {
                //these are the possible stages where depth is still being written used/
                dependencies[dependencyIndex].dstStageMask |=
                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                dependencies[dependencyIndex].dstAccessMask |=
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            }
        }
        if (!otherSubpass.m_colorAttachmentWriteReferences.empty()) {
            if (vul::contains_any_if(
                    otherSubpass.m_colorAttachmentWriteReferences,
                    m_colorAttachmentWriteReferences,
                    [](auto lhs, auto rhs) {
                        return lhs.attachment == rhs.attachment;
                    })) {
                dependencies[dependencyIndex].dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[dependencyIndex].dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
        }
        dependencies[dependencyIndex].srcStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[dependencyIndex].srcAccessMask |= VK_ACCESS_SHADER_READ_BIT;
    }

    for (const auto &dependencyIndex: m_rawDependencies) {
        if (vul::contains(dependencies, dependencyIndex)) {
            VkSubpassDependency dependency = {};
            dependency.srcSubpass = dependencyIndex;
            dependency.dstSubpass = m_subpassIndex;
            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies[dependencyIndex] = dependency;
        }
        auto &otherSubpass = m_parentGraph->subpassAt(dependencyIndex);
        if (otherSubpass.m_depthAttachmentWriteReference.has_value()) {
            if (vul::contains_if(m_inputAttachmentReferences,
                                 [attachment = otherSubpass.m_depthAttachmentWriteReference.value()](
                                         auto attachmentReference) {
                                     return attachmentReference.attachment ==
                                            attachment.attachment;
                                 })) {
                //these are the possible stages where depth is still being written used/
                dependencies[dependencyIndex].srcStageMask |=
                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                dependencies[dependencyIndex].srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            }
        }
        if (!otherSubpass.m_colorAttachmentWriteReferences.empty()) {
            if (vul::contains_any_if(
                    otherSubpass.m_colorAttachmentWriteReferences,
                    m_colorAttachmentWriteReferences,
                    [](auto lhs, auto rhs) {
                        return lhs.attachment == rhs.attachment;
                    })) {
                dependencies[dependencyIndex].srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[dependencyIndex].srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
        }
        dependencies[dependencyIndex].dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[dependencyIndex].dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
    }

    for (const auto &dependencyIndex: m_warDependencies) {
        if (vul::contains(dependencies, dependencyIndex)) {
            VkSubpassDependency dependency = {};
            dependency.srcSubpass = dependencyIndex;
            dependency.dstSubpass = m_subpassIndex;
            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            dependencies[dependencyIndex] = dependency;
        }
        auto &otherSubpass = m_parentGraph->subpassAt(dependencyIndex);
        if (m_depthAttachmentWriteReference.has_value() &&
            otherSubpass.m_depthAttachmentWriteReference.has_value()) {
            if (m_depthAttachmentWriteReference.value().attachment ==
                otherSubpass.m_depthAttachmentWriteReference.value().attachment) {
                //these are the possible stages where depth is still being written used/
                dependencies[dependencyIndex].srcStageMask |=
                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                dependencies[dependencyIndex].srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dependencies[dependencyIndex].dstStageMask |=
                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                dependencies[dependencyIndex].dstAccessMask |=
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            }
        }
        if (!otherSubpass.m_colorAttachmentWriteReferences.empty()) {
            if (vul::contains_any_if(
                    otherSubpass.m_colorAttachmentWriteReferences,
                    m_colorAttachmentWriteReferences,
                    [](auto lhs, auto rhs) {
                        return lhs.attachment == rhs.attachment;
                    })) {
                dependencies[dependencyIndex].srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[dependencyIndex].srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[dependencyIndex].dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[dependencyIndex].dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
        }
    }
    std::vector<VkSubpassDependency> subpassDependencies;
    subpassDependencies.reserve(dependencies.size());
    for (auto &[key, dependency]: dependencies) {
        subpassDependencies.push_back(dependency);
    }
    if (m_preExternalDependency.has_value()) {
        subpassDependencies.push_back(m_preExternalDependency.value());
    }
    if (m_postExternalDependency.has_value()) {
        subpassDependencies.push_back(m_postExternalDependency.value());
    }
    return subpassDependencies;
}

vul::SubpassNode &vul::SubpassNode::setPreDependExternal(
        uul::EnumFlags<vul::PipelineStageFlagBits> srcStageMask,
        uul::EnumFlags<vul::PipelineStageFlagBits> dstStageMask,
        uul::EnumFlags<vul::AccessFlagBits> srcAccessMask,
        uul::EnumFlags<vul::AccessFlagBits> dstAccessMask) {
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = m_subpassIndex;
    dependency.srcStageMask = srcStageMask.to_underlying();
    dependency.dstStageMask = dstStageMask.to_underlying();
    dependency.srcAccessMask = srcAccessMask.to_underlying();
    dependency.dstAccessMask = dstAccessMask.to_underlying();
    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    m_preExternalDependency = dependency;
    return *this;
}

vul::SubpassNode &vul::SubpassNode::setPostDependExternal(
        uul::EnumFlags<vul::PipelineStageFlagBits> srcStageMask,
        uul::EnumFlags<vul::PipelineStageFlagBits> dstStageMask,
        uul::EnumFlags<vul::AccessFlagBits> srcAccessMask,
        uul::EnumFlags<vul::AccessFlagBits> dstAccessMask) {
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = m_subpassIndex;
    dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = srcStageMask.to_underlying();
    dependency.dstStageMask = dstStageMask.to_underlying();
    dependency.srcAccessMask = srcAccessMask.to_underlying();
    dependency.dstAccessMask = dstAccessMask.to_underlying();
    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    m_postExternalDependency = dependency;
    return *this;
}


vul::AttachmentReference::AttachmentReference(std::uint32_t attachment_t,
                                              vul::ImageLayout layout_t)
        : attachment(attachment_t), layout(layout_t) {

}


vul::SubpassGraph::SubpassGraph(
        TempArrayProxy<const AttachmentDescription> attachmentDescriptions,
        std::uint32_t subpassCount) : m_attachmentDescriptions(
        attachmentDescriptions.begin(), attachmentDescriptions.end()) {
    //m_subpassNodes.reserve(subpassCount);
    for (std::uint32_t i = 0; i < subpassCount; ++i) {
        m_subpassNodes.emplace_back(*this, i);
    }
}

vul::SubpassNode &vul::SubpassGraph::subpassAt(std::uint32_t i) {
    return m_subpassNodes.at(i);
}

vul::ExpectedResult<vul::RenderPass>
vul::SubpassGraph::createRenderPass(const Device &device,
                                    const VkAllocationCallbacks *pAllocator) {
    std::vector<VkSubpassDescription> subpassDescriptions;
    std::vector<VkSubpassDependency> subpassDependencies;
    subpassDescriptions.reserve(m_subpassNodes.size());
    for (auto [subpassIndex, subpassNode]: ranges::views::enumerate(
            m_subpassNodes)) {
        subpassDescriptions.push_back(subpassNode.createDescription());
        vul::extend(subpassDependencies,
                    subpassNode.createSubpassDependencies());
    }

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pNext = nullptr;
    render_pass_info.flags = 0;
    //2 attachments from said array
    render_pass_info.attachmentCount = static_cast<std::uint32_t>(m_attachmentDescriptions.size());
    render_pass_info.pAttachments = reinterpret_cast<const VkAttachmentDescription *>(m_attachmentDescriptions.data());
    render_pass_info.subpassCount = static_cast<std::uint32_t>(subpassDescriptions.size());
    render_pass_info.pSubpasses = subpassDescriptions.data();
    render_pass_info.dependencyCount = static_cast<std::uint32_t>(subpassDependencies.size());
    render_pass_info.pDependencies = subpassDependencies.data();

    VkRenderPass renderPass;
    auto result = static_cast<Result>(
            vkCreateRenderPass(device.get(),
                               &render_pass_info,
                               pAllocator,
                               &renderPass));
    return {result, RenderPass(device, renderPass,
                               static_cast<std::uint32_t>(subpassDescriptions.size()),
                               pAllocator)};
}

const vul::AttachmentDescription &
vul::SubpassGraph::getAttachmentAt(std::uint32_t i) const {
    return m_attachmentDescriptions.at(i);
}

std::size_t vul::SubpassGraph::getAttachmentCount() const {
    return m_attachmentDescriptions.size();
}

vul::RenderPass::RenderPass(const vul::Device &device, VkRenderPass handle,
                            std::uint32_t subpassCount,
                            const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_handle(handle), m_subpassCount(subpassCount),
          m_pAllocator(pAllocator) {

}

VkRenderPass vul::RenderPass::get() const {
    return m_handle;
}

vul::RenderPass::~RenderPass() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::RenderPass::RenderPass(vul::RenderPass &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::RenderPass &vul::RenderPass::operator=(vul::RenderPass &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::RenderPass::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

std::uint32_t vul::RenderPass::getSubpassCount() const {
    return m_subpassCount;
}
