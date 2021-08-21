//
// Created by Shae Bolt on 8/18/2021.
//

#ifndef VULKANWORKSPACE_RENDERPASS_H
#define VULKANWORKSPACE_RENDERPASS_H

#include "vul/bitmasks.h"
#include "vul/enums.h"

#include <vulkan/vulkan.h>
#include <gsl/span>

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
                              vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription PresentTemp(vul::Format format,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription PresentLast(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription ColorFirst(vul::Format format,
                                                vul::ImageLayout finalLayout,
                                                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription ColorNext(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::ImageLayout finalLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthStencilFirst(vul::Format format,
                                                       vul::ImageLayout finalLayout,
                                                       vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                       vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthStencilNext(vul::Format format,
                                                      vul::ImageLayout initialLayout,
                                                      vul::ImageLayout finalLayout,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthFirst(vul::Format format,
                                                vul::ImageLayout finalLayout,
                                                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthNext(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::ImageLayout finalLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription StencilFirst(vul::Format format,
                                                  vul::ImageLayout finalLayout,
                                                  vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                  vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription StencilNext(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::ImageLayout finalLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription ColorTemp(vul::Format format,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription ColorLast(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthStencilTemp(vul::Format format,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthStencilLast(vul::Format format,
                                                      vul::ImageLayout initialLayout,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthTemp(vul::Format format,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription DepthLast(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription StencilTemp(vul::Format format,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

        static AttachmentDescription StencilLast(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

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


    //attachment description that **will be** presented.
    class PresentAttachmentDescription {
    public:
        PresentAttachmentDescription(vul::Format format,
                                     vul::ImageLayout initialLayout,
                                     vul::AttachmentLoadOp loadOp,
                                     vul::AttachmentStoreOp storeOp,
                                     vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                     vul::AttachmentDescriptionBitMask flags = {});

        static PresentAttachmentDescription only(vul::Format format,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

        static PresentAttachmentDescription last(vul::Format format,
                                                 vul::ImageLayout initialLayout,
                                                 vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                 vul::AttachmentDescriptionBitMask flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;

    private:
        VkAttachmentDescription m_attachmentDescription;
    };

    //color attachment description that **will be** used as ie storage image read/write and/or sampled.
    class UsedColorAttachmentDescription {
    public:
        UsedColorAttachmentDescription(vul::Format format,
                                       vul::ImageLayout initialLayout,
                                       vul::ImageLayout finalLayout,
                                       vul::AttachmentLoadOp loadOp,
                                       vul::AttachmentStoreOp storeOp,
                                       vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                       vul::AttachmentDescriptionBitMask flags = {});

        static UsedColorAttachmentDescription only(vul::Format format,
                                                   vul::ImageLayout finalLayout,
                                                   vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                   vul::AttachmentDescriptionBitMask flags = {});

        static UsedColorAttachmentDescription last(vul::Format format,
                                                   vul::ImageLayout initialLayout,
                                                   vul::ImageLayout finalLayout,
                                                   vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                   vul::AttachmentDescriptionBitMask flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;

    private:
        VkAttachmentDescription m_attachmentDescription;
    };

    //depth stencil attachment description that **will be** used as ie storage image read/write and/or sampled.
    class UsedDepthStencilAttachmentDescription {
    public:
        UsedDepthStencilAttachmentDescription(vul::Format format,
                                              vul::ImageLayout initialLayout,
                                              vul::ImageLayout finalLayout,
                                              vul::AttachmentLoadOp loadOp,
                                              vul::AttachmentStoreOp storeOp,
                                              vul::AttachmentLoadOp stencilLoadOp,
                                              vul::AttachmentStoreOp stencilStoreOp,
                                              vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                              vul::AttachmentDescriptionBitMask flags = {});

        static UsedDepthStencilAttachmentDescription only(vul::Format format,
                                                          vul::ImageLayout finalLayout,
                                                          vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                          vul::AttachmentDescriptionBitMask flags = {});

        static UsedDepthStencilAttachmentDescription last(vul::Format format,
                                                          vul::ImageLayout initialLayout,
                                                          vul::ImageLayout finalLayout,
                                                          vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                          vul::AttachmentDescriptionBitMask flags = {});

        static UsedDepthStencilAttachmentDescription
        onlyDepth(vul::Format format,
                  vul::ImageLayout finalLayout,
                  vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                  vul::AttachmentDescriptionBitMask flags = {});

        static UsedDepthStencilAttachmentDescription
        lastDepth(vul::Format format,
                  vul::ImageLayout initialLayout,
                  vul::ImageLayout finalLayout,
                  vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                  vul::AttachmentDescriptionBitMask flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;

    private:
        VkAttachmentDescription m_attachmentDescription;
    };

    class DepthStencilAttachmentDescription {
    public:
        DepthStencilAttachmentDescription(vul::Format format,
                                          vul::ImageLayout initialLayout,
                                          vul::AttachmentLoadOp loadOp,
                                          vul::AttachmentStoreOp storeOp,
                                          vul::AttachmentLoadOp stencilLoadOp,
                                          vul::AttachmentStoreOp stencilStoreOp,
                                          vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                          vul::AttachmentDescriptionBitMask flags = {});

        static DepthStencilAttachmentDescription only(vul::Format format,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      vul::AttachmentDescriptionBitMask flags = {});

        static DepthStencilAttachmentDescription last(vul::Format format,
                                                      vul::ImageLayout initialLayout,
                                                      vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                      vul::AttachmentDescriptionBitMask flags = {});

        static DepthStencilAttachmentDescription onlyDepth(vul::Format format,
                                                           vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                           vul::AttachmentDescriptionBitMask flags = {});

        static DepthStencilAttachmentDescription lastDepth(vul::Format format,
                                                           vul::ImageLayout initialLayout,
                                                           vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                           vul::AttachmentDescriptionBitMask flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;

    private:
        VkAttachmentDescription m_attachmentDescription;
    };


    //Attachment Description meant to be used *only* within the current renderpass.
    class TemporaryColorAttachmentDescription {
    public:
        TemporaryColorAttachmentDescription(vul::Format format,
                                            vul::ImageLayout initialLayout,
                                            vul::AttachmentLoadOp loadOp,
                                            vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                            vul::AttachmentDescriptionBitMask flags = {});

        static TemporaryColorAttachmentDescription only(vul::Format format,
                                                        vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                        vul::AttachmentDescriptionBitMask flags = {});

        static TemporaryColorAttachmentDescription last(vul::Format format,
                                                        vul::ImageLayout initialLayout,
                                                        vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                        vul::AttachmentDescriptionBitMask flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;

    private:
        VkAttachmentDescription m_attachmentDescription;
    };


    class ColorAttachmentDescription {
    public:
        ColorAttachmentDescription(vul::Format format,
                                   vul::ImageLayout initialLayout,
                                   vul::ImageLayout finalLayout,
                                   vul::AttachmentLoadOp loadOp,
                                   vul::AttachmentStoreOp storeOp,
                                   vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                   vul::AttachmentDescriptionBitMask flags = {});

        //TODO get rid of?
        static ColorAttachmentDescription first(vul::Format format,
                                                vul::ImageLayout finalLayout = vul::ImageLayout::ColorAttachmentOptimal,
                                                vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                                vul::AttachmentDescriptionBitMask flags = {});

        static ColorAttachmentDescription only(vul::Format format,
                                               vul::ImageLayout finalLayout = vul::ImageLayout::ColorAttachmentOptimal,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        //TODO get rid of?
        static ColorAttachmentDescription next(vul::Format format,
                                               vul::ImageLayout initialLayout,
                                               vul::ImageLayout finalLayout = vul::ImageLayout::ColorAttachmentOptimal,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        static ColorAttachmentDescription last(vul::Format format,
                                               vul::ImageLayout initialLayout = vul::ImageLayout::ColorAttachmentOptimal,
                                               vul::SampleCountFlagBits samples = vul::SampleCountFlagBits::_1Bit,
                                               vul::AttachmentDescriptionBitMask flags = {});

        [[nodiscard]]
        const VkAttachmentDescription &get() const;

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


    class SubpassGraph {
    public:
        SubpassGraph(gsl::span<int &> x);

    private:
    };

    class SubpassNode {
    public:

        void read(std::uint32_t attachment_index);

        void write(std::uint32_t attachment_index);

        void resolve(std::uint32_t attachment_index);

        void write_dependency(std::uint32_t attachment_index,
                              std::uint32_t subpass_index);

        void read_dependency(std::uint32_t attachment_index,
                             std::uint32_t subpass_index);

        void resolve_dependency(std::uint32_t attachment_index,
                                std::uint32_t subpass_index);

    private:
    };


    //https://www.reddit.com/r/vulkan/comments/k9l9rd/did_something_change_in_the_spec_vk_image_layout/
    //unless separateDepthStencilLayouts  enabled, only depth-stencil layouts in attachment references

    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment,
                                                              depthAttachment};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void init_default_renderpass() {
        //we define an attachment description for our main color image
        //the attachment is loaded as "clear" when renderpass start
        //the attachment is stored when renderpass ends
        //the attachment layout starts as "undefined", and transitions to "Present" so its possible to display it
        //we dont care about stencil, and dont use multisampling

        VkAttachmentDescription color_attachment = {};
        color_attachment.format = _swachainImageFormat;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depth_attachment = {};
        // Depth attachment
        depth_attachment.flags = 0;
        depth_attachment.format = _depthFormat;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref = {};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        //we are going to create 1 subpass, which is the minimum you can do
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;
        //hook the depth attachment into the subpass
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        //1 dependency, which is from "outside" into the subpass. And we can read or write color
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency2 dependency;

        //array of 2 attachments, one for the color, and other for depth
        VkAttachmentDescription attachments[2] = {color_attachment,
                                                  depth_attachment};

        VkRenderPassCreateInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        //2 attachments from said array
        render_pass_info.attachmentCount = 2;
        render_pass_info.pAttachments = &attachments[0];
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;
        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies = &dependency;

        VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr,
                                    &_renderPass));

        _mainDeletionQueue.push_function([=]() {
            vkDestroyRenderPass(_device, _renderPass, nullptr);
        });
    }

    class RenderPass {
    public:
    private:
    };
}


#endif //VULKANWORKSPACE_RENDERPASS_H
