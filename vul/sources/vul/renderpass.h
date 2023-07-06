//
// Created by Shae Bolt on 8/18/2021.
//

#ifndef VULKANWORKSPACE_RENDERPASS_H
#define VULKANWORKSPACE_RENDERPASS_H


#include "vul/enums.h"
#include "vul/temparrayproxy.h"
#include <uul/enumflagsfwd.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace vul {

    //color, depth stencil, and resolve only options for color passes then
    //only, first, next, last.
    //(write) similarly only VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL and VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL make sense in references
    //(read) VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL


    //attachment sampled/written later
    //attachment only used in current renderpass
    //present attachment
    //depth attachment
    //color attachment

    //https://www.reddit.com/r/vulkan/comments/k9l9rd/did_something_change_in_the_spec_vk_image_layout/
    //unless separateDepthStencilLayouts  enabled, only depth-stencil layouts in attachment references
    class AttachmentDescription {
    public:
        AttachmentDescription(vul::Format format,
                              vul::ImageLayout initialLayout,
                              vul::ImageLayout finalLayout,
                              vul::AttachmentLoadOp loadOp,
                              vul::AttachmentStoreOp storeOp,
                              vul::AttachmentLoadOp stencilLoadOp,
                              vul::AttachmentStoreOp stencilStoreOp,
                              vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                              uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription PresentTemp(vul::Format format,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription PresentLast(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription ColorFirst(vul::Format format,
                                                vul::ImageLayout finalLayout,
                                                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription ColorNext(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::ImageLayout finalLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthStencilFirst(vul::Format format,
                                                       vul::ImageLayout finalLayout,
                                                       vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                       uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthStencilNext(vul::Format format,
                                                      vul::ImageLayout initialLayout,
                                                      vul::ImageLayout finalLayout,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthFirst(vul::Format format,
                                                vul::ImageLayout finalLayout,
                                                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthNext(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::ImageLayout finalLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription StencilFirst(vul::Format format,
                                                  vul::ImageLayout finalLayout,
                                                  vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                  uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription StencilNext(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::ImageLayout finalLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription ColorTemp(vul::Format format,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription ColorLast(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthStencilTemp(vul::Format format,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthStencilLast(vul::Format format,
                                                      vul::ImageLayout initialLayout,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthTemp(vul::Format format,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthReadOnly(vul::Format format,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription DepthLast(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription StencilTemp(vul::Format format,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        static AttachmentDescription StencilLast(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 uul::EnumFlags<vul::AttachmentDescriptionFlagBits> flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;


        [[nodiscard]]
        bool isColor() const;

        [[nodiscard]]
        bool isDepthStencil() const;

        [[nodiscard]]
        bool isDepth() const;

        [[nodiscard]]
        bool isStencil() const;


    private:
        VkAttachmentDescription m_attachmentDescription;
    };


    //auto swapchainAttachment = PresentAttachmentDescription::only(swapchainFormat);
    //auto positionAttachment = TemporaryColorAttachmentDescription::only(positionFormat);
    //auto normalAttachment = TemporaryColorAttachmentDescription::only(normalFormat);
    //auto albedoAttachment = TemporaryColorAttachmentDescription::only(albedoFormat);
    //auto depthAttachment = DepthStencilAttachmentDescription::only(depthFormat);

    //use array of references?
    //auto subpassCount =3;
    //SubpassGraph graph({swapchainAttachment, positionAttachment, normalAttachment, albedoAttachment, depthAttachment}, subpassCount);
    //auto insert before depend
    //graph.subpass(0).writes(swapchainAttachment) // compares with *reference* found in array fed in.
    //                .writes(positionAttachment)
    //                .writes(normalAttachment)
    //                .writes(albedoAttachment)
    //                .writes(depthAttachment);
    // . resolves()
    //graph.subpass(1).writes(swapchainAttachment)
    //                .reads(positionAttachment)
    //                .reads(normalAttachment)
    //                .reads(albedoAttachment)
    //                .writes(depthAttachment)
    //                .dependsReadAfterWrite({0}); //depends on subpass index.
    //                //.dependsWriteAfterRead({0,1...) // write after read WAR
    //                //.dependsReadAfterWrite({0,1...) // read after write RAW
    //                //.dependsWriteAfterWrite({0,1...) // write after write WAW
    //
    //graph.subpass(2).writes(swapchainAttachment)
    //                .reads(positionAttachment)
    //                .writes(depthAttachment)
    //                .dependsReadAfterWrite({1});
    //                //.dependsWriteAfterRead({0,1...)
    //                //.dependsReadAfterWrite({0,1...)
    //                //.dependsWriteAfterWrite({0,1...)
    //auto insert after depend.

    //graph.subpass(2).reset() // clears all currently set data
    //auto renderPass = graph.createRenderPass();
    //auto& graphi

    struct AttachmentReference{
        std::uint32_t attachment = 0;
        vul::ImageLayout layout = {};
        AttachmentReference() = default;
        AttachmentReference(std::uint32_t attachment, vul::ImageLayout layout);
    };
    template<typename T>
    class ExpectedResult;
    class Device;

    class RenderPass{
    public:
        RenderPass() = default;

        RenderPass(const Device &device, VkRenderPass handle,
                  std::uint32_t subpassCount,
                  const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkRenderPass get() const;

        ~RenderPass();

        RenderPass(RenderPass &&rhs) noexcept;

//        was noexcept?
        RenderPass &operator=(RenderPass &&rhs) noexcept;

        RenderPass(const RenderPass &rhs) = delete;

        RenderPass &operator=(const RenderPass &rhs) = delete;

        Result setObjectName(const std::string &string);

        [[nodiscard]]
        std::uint32_t getSubpassCount() const;
    private:
        const Device *m_pDevice = nullptr;
        VkRenderPass m_handle = VK_NULL_HANDLE;
        std::uint32_t m_subpassCount = 0;
        const VkAllocationCallbacks *m_pAllocator = nullptr;

    };

    class SubpassGraph;
    class SubpassNode{

    public:
        explicit SubpassNode(SubpassGraph& parentGraph, std::uint32_t subpassIndex);
        std::uint32_t SubpassExternal = VK_SUBPASS_EXTERNAL;
        //TODO set depth writes instead? not sure what the point of not having a seperate write is.
        SubpassNode& setWrites(const gsl::span<const std::uint32_t>& attachmentIndices);
        SubpassNode& setReads(const gsl::span<const std::uint32_t>& attachmentIndices);
        SubpassNode& setWrites(const gsl::span<const std::uint32_t>& attachmentIndices, const gsl::span<const std::uint32_t>& resolveIndices);
        //preserve serves unknown usage,maybe simulataneous use outside subpass?
        SubpassNode& setPreserve(const gsl::span<const std::uint32_t>& attachmentIndices);
        SubpassNode& setDependsWriteAfterRead(const gsl::span<const std::uint32_t>& subpassIndices);
        SubpassNode& setDependsReadAfterWrite(const gsl::span<const std::uint32_t>& subpassIndices);
        SubpassNode& setDependsWriteAfterWrite(const gsl::span<const std::uint32_t>& subpassIndices);

        SubpassNode& setWrites(const std::initializer_list<const std::uint32_t>& attachmentIndices);
        SubpassNode& setReads(const std::initializer_list<const std::uint32_t>& attachmentIndices);
        SubpassNode& setWrites(const std::initializer_list<const std::uint32_t>& attachmentIndices, const std::initializer_list<const std::uint32_t>& resolveIndices);
        SubpassNode& setPreserve(const std::initializer_list<const std::uint32_t>& attachmentIndices);
        SubpassNode& setDependsWriteAfterRead(const std::initializer_list<const std::uint32_t>& subpassIndices);
        SubpassNode& setDependsReadAfterWrite(const std::initializer_list<const std::uint32_t>& subpassIndices);
        SubpassNode& setDependsWriteAfterWrite(const std::initializer_list<const std::uint32_t>& subpassIndices);


        //TODO PipelineStage2BitMask should be PipelineStageBitMask2 or something, conflicts with
        // PipelineStageBitMask
        //TODO same with Access2BitMask AccessBitMask
        //set subpass depend on external

        SubpassNode& setPreDependExternal(uul::EnumFlags<vul::PipelineStageFlagBits> srcStageMask,
                                    uul::EnumFlags<vul::PipelineStageFlagBits> dstStageMask,
                                    uul::EnumFlags<vul::AccessFlagBits> srcAccessMask,
                                    uul::EnumFlags<vul::AccessFlagBits> dstAccessMask);

        //set external dependency that depends on *this subpass*
        SubpassNode& setPostDependExternal(uul::EnumFlags<vul::PipelineStageFlagBits> srcStageMask,
                                           uul::EnumFlags<vul::PipelineStageFlagBits> dstStageMask,
                                           uul::EnumFlags<vul::AccessFlagBits> srcAccessMask,
                                           uul::EnumFlags<vul::AccessFlagBits> dstAccessMask);

        void reset();
        //TODO didn't want them to be public, looks like I have to have them be public...
        SubpassNode(const SubpassNode& subpass_node) = default;
        SubpassNode(SubpassNode&& subpass_node) = default;
        SubpassNode& operator=(const SubpassNode& subpass_node) = default;
        SubpassNode& operator=(SubpassNode&& subpass_node) = default;
    private:

        friend class SubpassGraph;
        friend class std::vector<SubpassNode>;



        [[nodiscard]]
        VkSubpassDescription createDescription(uul::EnumFlags<vul::SubpassDescriptionFlagBits> flags = {}) const;
        [[nodiscard]]
        std::vector<VkSubpassDependency> createSubpassDependencies() const;
        SubpassGraph* m_parentGraph = nullptr;
        std::uint32_t m_subpassIndex  =0;
        std::vector<std::uint32_t> m_warDependencies;
        std::vector<std::uint32_t> m_rawDependencies;
        std::vector<std::uint32_t> m_wawDependencies;
        std::vector<AttachmentReference> m_colorAttachmentWriteReferences;
        std::vector<AttachmentReference> m_inputAttachmentReferences;
        std::vector<AttachmentReference> m_resolveAttachmentReferences;
        std::vector<std::uint32_t> m_preserveAttachmentReferences;
        std::optional<AttachmentReference> m_depthAttachmentWriteReference;
        std::optional<VkSubpassDependency> m_preExternalDependency;
        std::optional<VkSubpassDependency> m_postExternalDependency;
    };



    class SubpassGraph {
    public:
        SubpassGraph(TempArrayProxy<const AttachmentDescription> attachmentDescriptions, std::uint32_t subpassCount);
        SubpassNode& subpassAt(std::uint32_t i);
        ExpectedResult<RenderPass> createRenderPass(const Device &device, const VkAllocationCallbacks *pAllocator = nullptr);
    private:
        friend class SubpassNode;
        [[nodiscard]]
        const AttachmentDescription& getAttachmentAt(std::uint32_t i) const;
        [[nodiscard]]
        std::size_t getAttachmentCount() const;
        std::vector<AttachmentDescription> m_attachmentDescriptions;
        std::vector<SubpassNode> m_subpassNodes;

    };
}


#endif //VULKANWORKSPACE_RENDERPASS_H
