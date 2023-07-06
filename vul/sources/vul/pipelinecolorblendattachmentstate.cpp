//
// Created by Bolt on 7/5/2023.
//

#include "pipelinecolorblendattachmentstate.h"

vul::PipelineColorBlendAttachmentState::operator VkPipelineColorBlendAttachmentState() const noexcept {
    return get();
}

VkPipelineColorBlendAttachmentState &vul::PipelineColorBlendAttachmentState::get() noexcept {
    return *reinterpret_cast<VkPipelineColorBlendAttachmentState *>(this);
}

const VkPipelineColorBlendAttachmentState &vul::PipelineColorBlendAttachmentState::get() const noexcept {
    return *reinterpret_cast<const VkPipelineColorBlendAttachmentState *>(this);
}

vul::PipelineColorBlendAttachmentState vul::PipelineColorBlendAttachmentState::disableBlendRGBA() noexcept {
    PipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    using enum vul::ColorComponentFlagBits;
    colorBlendAttachmentState.colorWriteMask = RBit | GBit | BBit | ABit;
    return colorBlendAttachmentState;
}
