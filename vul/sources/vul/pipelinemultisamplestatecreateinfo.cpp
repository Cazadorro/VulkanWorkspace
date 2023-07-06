//
// Created by Bolt on 7/5/2023.
//

#include "pipelinemultisamplestatecreateinfo.h"

vul::PipelineMultisampleStateCreateInfo::PipelineMultisampleStateCreateInfo() :
        pNext(nullptr),
        flags(0),
        rasterizationSamples(SampleCountFlagBits{}),
        sampleShadingEnable(false),
        minSampleShading(0),
        pSampleMask(nullptr),
        alphaToCoverageEnable(false),
        alphaToOneEnable(false){
}

vul::PipelineMultisampleStateCreateInfo::PipelineMultisampleStateCreateInfo(
        const vul::PipelineMultisampleStateCreateInfo &rhs) noexcept: pNext(rhs.pNext),
                                                                      flags(rhs.flags),
                                                                      rasterizationSamples(rhs.rasterizationSamples),
                                                                      sampleShadingEnable(rhs.sampleShadingEnable),
                                                                      minSampleShading(rhs.minSampleShading),
                                                                      pSampleMask(rhs.pSampleMask),
                                                                      alphaToCoverageEnable(rhs.alphaToCoverageEnable),
                                                                      alphaToOneEnable(rhs.alphaToOneEnable) {

}

vul::PipelineMultisampleStateCreateInfo::PipelineMultisampleStateCreateInfo(
        vul::PipelineMultisampleStateCreateInfo &&rhs) noexcept: pNext(rhs.pNext),
                                                                 flags(rhs.flags),
                                                                 rasterizationSamples(rhs.rasterizationSamples),
                                                                 sampleShadingEnable(rhs.sampleShadingEnable),
                                                                 minSampleShading(rhs.minSampleShading),
                                                                 pSampleMask(rhs.pSampleMask),
                                                                 alphaToCoverageEnable(rhs.alphaToCoverageEnable),
                                                                 alphaToOneEnable(rhs.alphaToOneEnable) {

}

vul::PipelineMultisampleStateCreateInfo &
vul::PipelineMultisampleStateCreateInfo::operator=(const vul::PipelineMultisampleStateCreateInfo &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        flags = rhs.flags;
        rasterizationSamples = rhs.rasterizationSamples;
        sampleShadingEnable = rhs.sampleShadingEnable;
        minSampleShading = rhs.minSampleShading;
        pSampleMask = rhs.pSampleMask;
        alphaToCoverageEnable = rhs.alphaToCoverageEnable;
        alphaToOneEnable = rhs.alphaToOneEnable;
    }
    return *this;
}

vul::PipelineMultisampleStateCreateInfo::~PipelineMultisampleStateCreateInfo() =default;

vul::PipelineMultisampleStateCreateInfo::operator VkPipelineMultisampleStateCreateInfo() const noexcept {
    return get();
}

VkPipelineMultisampleStateCreateInfo &vul::PipelineMultisampleStateCreateInfo::get() noexcept {
    return *reinterpret_cast<VkPipelineMultisampleStateCreateInfo *>(this);
}

const VkPipelineMultisampleStateCreateInfo &vul::PipelineMultisampleStateCreateInfo::get() const noexcept {
    return *reinterpret_cast<const VkPipelineMultisampleStateCreateInfo *>(this);
}

vul::PipelineMultisampleStateCreateInfo
vul::PipelineMultisampleStateCreateInfo::fromSampleCount(vul::SampleCountFlagBits sampleCount) noexcept {
    PipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};

    multisampleStateCreateInfo.pNext = nullptr;
    multisampleStateCreateInfo.flags = {};
    multisampleStateCreateInfo.rasterizationSamples = sampleCount;
    multisampleStateCreateInfo.sampleShadingEnable = false;
    multisampleStateCreateInfo.minSampleShading = 0;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = false;
    multisampleStateCreateInfo.alphaToOneEnable = false;
    return multisampleStateCreateInfo;
}

