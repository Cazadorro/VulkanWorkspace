//
// Created by Bolt on 7/5/2023.
//

#include "pipelinerasterizationstatecreateinfo.h"

vul::PipelineRasterizationStateCreateInfo::PipelineRasterizationStateCreateInfo() : pNext(nullptr),
                                                                                    flags(0),
                                                                                    depthClampEnable(false),
                                                                                    rasterizerDiscardEnable(false),
                                                                                    polygonMode(PolygonMode{}),
                                                                                    cullMode(0),
                                                                                    frontFace(FrontFace{}),
                                                                                    depthBiasEnable(false),
                                                                                    depthBiasConstantFactor(0),
                                                                                    depthBiasClamp(0),
                                                                                    depthBiasSlopeFactor(0),
                                                                                    lineWidth(0.0){
}

vul::PipelineRasterizationStateCreateInfo::PipelineRasterizationStateCreateInfo(
        const vul::PipelineRasterizationStateCreateInfo &rhs) noexcept: pNext(rhs.pNext),
                                                                        flags(rhs.flags),
                                                                        depthClampEnable(rhs.depthClampEnable),
                                                                        rasterizerDiscardEnable(rhs.rasterizerDiscardEnable),
                                                                        polygonMode(rhs.polygonMode),
                                                                        cullMode(rhs.cullMode),
                                                                        frontFace(rhs.frontFace),
                                                                        depthBiasEnable(rhs.depthBiasEnable),
                                                                        depthBiasConstantFactor(rhs.depthBiasConstantFactor),
                                                                        depthBiasClamp(rhs.depthBiasClamp),
                                                                        depthBiasSlopeFactor(rhs.depthBiasSlopeFactor),
                                                                        lineWidth(rhs.lineWidth) {
    
}

vul::PipelineRasterizationStateCreateInfo::PipelineRasterizationStateCreateInfo(
        vul::PipelineRasterizationStateCreateInfo &&rhs) noexcept :pNext(rhs.pNext),
                                                                   flags(rhs.flags),
                                                                   depthClampEnable(rhs.depthClampEnable),
                                                                   rasterizerDiscardEnable(rhs.rasterizerDiscardEnable),
                                                                   polygonMode(rhs.polygonMode),
                                                                   cullMode(rhs.cullMode),
                                                                   frontFace(rhs.frontFace),
                                                                   depthBiasEnable(rhs.depthBiasEnable),
                                                                   depthBiasConstantFactor(rhs.depthBiasConstantFactor),
                                                                   depthBiasClamp(rhs.depthBiasClamp),
                                                                   depthBiasSlopeFactor(rhs.depthBiasSlopeFactor),
                                                                   lineWidth(rhs.lineWidth) {

}

vul::PipelineRasterizationStateCreateInfo &
vul::PipelineRasterizationStateCreateInfo::operator=(const vul::PipelineRasterizationStateCreateInfo &rhs) noexcept {
    if (this != &rhs) {
        pNext=rhs.pNext;
        flags=rhs.flags;
        depthClampEnable=rhs.depthClampEnable;
        rasterizerDiscardEnable=rhs.rasterizerDiscardEnable;
        polygonMode=rhs.polygonMode;
        cullMode=rhs.cullMode;;
        frontFace=rhs.frontFace;
        depthBiasEnable=rhs.depthBiasEnable;
        depthBiasConstantFactor=rhs.depthBiasConstantFactor;
        depthBiasClamp=rhs.depthBiasClamp;
        depthBiasSlopeFactor=rhs.depthBiasSlopeFactor;
        lineWidth = rhs.lineWidth;
    }
    return *this;
}

vul::PipelineRasterizationStateCreateInfo::~PipelineRasterizationStateCreateInfo() = default;

vul::PipelineRasterizationStateCreateInfo::operator VkPipelineRasterizationStateCreateInfo() const noexcept {
    return get();
}

VkPipelineRasterizationStateCreateInfo &vul::PipelineRasterizationStateCreateInfo::get() noexcept {
    return *reinterpret_cast<VkPipelineRasterizationStateCreateInfo *>(this);
}

const VkPipelineRasterizationStateCreateInfo &vul::PipelineRasterizationStateCreateInfo::get() const noexcept {
    return *reinterpret_cast<const VkPipelineRasterizationStateCreateInfo *>(this);
}

vul::PipelineRasterizationStateCreateInfo
vul::PipelineRasterizationStateCreateInfo::fromCullMode(uul::EnumFlags<CullModeFlagBits> cullMode) noexcept {
    PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = {};
    rasterizationStateCreateInfo.depthClampEnable = false;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = false;
    rasterizationStateCreateInfo.polygonMode = PolygonMode::Fill;
    rasterizationStateCreateInfo.cullMode = cullMode;
    rasterizationStateCreateInfo.frontFace = FrontFace::CounterClockwise;
    rasterizationStateCreateInfo.depthBiasEnable = false;
//    rasterizationStateCreateInfo.depthBiasConstantFactor;
//    rasterizationStateCreateInfo.depthBiasClamp;
//    rasterizationStateCreateInfo.depthBiasSlopeFactor;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    return rasterizationStateCreateInfo;
}
