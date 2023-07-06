//
// Created by Bolt on 7/5/2023.
//

#include "pipelinedepthstencilstatecreateinfo.h"

vul::StencilOpState::operator VkStencilOpState() const noexcept {
    return get();
}

VkStencilOpState &vul::StencilOpState::get() noexcept {
    return *reinterpret_cast<VkStencilOpState *>(this);
}

const VkStencilOpState &vul::StencilOpState::get() const noexcept {
    return *reinterpret_cast<const VkStencilOpState *>(this);
}

vul::PipelineDepthStencilStateCreateInfo::PipelineDepthStencilStateCreateInfo() : pNext(nullptr),
                                                                                  flags(0),
                                                                                  depthTestEnable(false),
                                                                                  depthWriteEnable(false),
                                                                                  depthCompareOp(CompareOp{}),
                                                                                  depthBoundsTestEnable(false),
                                                                                  stencilTestEnable(false),
                                                                                  front(StencilOpState{}),
                                                                                  back(StencilOpState{}),
                                                                                  minDepthBounds(0),
                                                                                  maxDepthBounds(0){
}

vul::PipelineDepthStencilStateCreateInfo::PipelineDepthStencilStateCreateInfo(
        const vul::PipelineDepthStencilStateCreateInfo &rhs) noexcept:
        pNext(rhs.pNext),
        flags(rhs.flags),
        depthTestEnable(rhs.depthTestEnable),
        depthWriteEnable(rhs.depthWriteEnable),
        depthCompareOp(rhs.depthCompareOp),
        depthBoundsTestEnable(rhs.depthBoundsTestEnable),
        stencilTestEnable(rhs.stencilTestEnable),
        front(rhs.front),
        back(rhs.back),
        minDepthBounds(rhs.minDepthBounds),
        maxDepthBounds(rhs.maxDepthBounds) {
}

vul::PipelineDepthStencilStateCreateInfo::PipelineDepthStencilStateCreateInfo(
        vul::PipelineDepthStencilStateCreateInfo &&rhs) noexcept:
        pNext(rhs.pNext),
        flags(rhs.flags),
        depthTestEnable(rhs.depthTestEnable),
        depthWriteEnable(rhs.depthWriteEnable),
        depthCompareOp(rhs.depthCompareOp),
        depthBoundsTestEnable(rhs.depthBoundsTestEnable),
        stencilTestEnable(rhs.stencilTestEnable),
        front(rhs.front),
        back(rhs.back),
        minDepthBounds(rhs.minDepthBounds),
        maxDepthBounds(rhs.maxDepthBounds) {
}

vul::PipelineDepthStencilStateCreateInfo &
vul::PipelineDepthStencilStateCreateInfo::operator=(const vul::PipelineDepthStencilStateCreateInfo &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        flags = rhs.flags;
        depthTestEnable = rhs.depthTestEnable;
        depthWriteEnable = rhs.depthWriteEnable;
        depthCompareOp = rhs.depthCompareOp;
        depthBoundsTestEnable = rhs.depthBoundsTestEnable;
        stencilTestEnable = rhs.stencilTestEnable;
        front = rhs.front;
        back = rhs.back;
        minDepthBounds = rhs.minDepthBounds;
        maxDepthBounds = rhs.maxDepthBounds;
    }
    return *this;
}

vul::PipelineDepthStencilStateCreateInfo::~PipelineDepthStencilStateCreateInfo() = default;

vul::PipelineDepthStencilStateCreateInfo::operator VkPipelineDepthStencilStateCreateInfo() const noexcept {
    return get();
}

VkPipelineDepthStencilStateCreateInfo &vul::PipelineDepthStencilStateCreateInfo::get() noexcept {
    return *reinterpret_cast<VkPipelineDepthStencilStateCreateInfo *>(this);
}

const VkPipelineDepthStencilStateCreateInfo &vul::PipelineDepthStencilStateCreateInfo::get() const noexcept {
    return *reinterpret_cast<const VkPipelineDepthStencilStateCreateInfo *>(this);
}

vul::PipelineDepthStencilStateCreateInfo
vul::PipelineDepthStencilStateCreateInfo::fromCompareOp(vul::CompareOp compareOp) noexcept {
    PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.pNext = nullptr;
    depthStencilStateCreateInfo.flags = {};
    depthStencilStateCreateInfo.depthTestEnable = true;
    depthStencilStateCreateInfo.depthWriteEnable = true;
    depthStencilStateCreateInfo.depthCompareOp = compareOp;
    depthStencilStateCreateInfo.depthBoundsTestEnable = false;
    depthStencilStateCreateInfo.stencilTestEnable = false;
//    depthStencilStateCreateInfo.front;
//    depthStencilStateCreateInfo.back;
//    depthStencilStateCreateInfo.minDepthBounds;
//    depthStencilStateCreateInfo.maxDepthBounds;
    return depthStencilStateCreateInfo;
}
