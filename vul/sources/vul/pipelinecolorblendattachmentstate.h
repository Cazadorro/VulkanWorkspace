//
// Created by Bolt on 7/5/2023.
//

#ifndef VULKANWORKSPACE_PIPELINECOLORBLENDATTACHMENTSTATE_H
#define VULKANWORKSPACE_PIPELINECOLORBLENDATTACHMENTSTATE_H

#include "vul/enums.h"
#include "vul/bool32.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
namespace vul{

    struct PipelineColorBlendAttachmentState{
        Bool32                                      blendEnable = false;
        BlendFactor                                 srcColorBlendFactor = {};
        BlendFactor                                 dstColorBlendFactor = {};
        BlendOp                                     colorBlendOp = {};
        BlendFactor                                 srcAlphaBlendFactor = {};
        BlendFactor                                 dstAlphaBlendFactor = {};
        BlendOp                                     alphaBlendOp = {};
        uul::EnumFlags<vul::ColorComponentFlagBits> colorWriteMask = {};

        operator VkPipelineColorBlendAttachmentState() const noexcept;
        [[nodiscard]]
        VkPipelineColorBlendAttachmentState &get() noexcept;
        [[nodiscard]]
        const VkPipelineColorBlendAttachmentState &get() const noexcept;

        [[nodiscard]]
        static PipelineColorBlendAttachmentState disableBlendRGBA() noexcept;
    };
}

#endif //VULKANWORKSPACE_PIPELINECOLORBLENDATTACHMENTSTATE_H
