//
// Created by Bolt on 7/5/2023.
//

#include "pipelineinputassemblystatecreateinfo.h"

vul::PipelineInputAssemblyStateCreateInfo::operator VkPipelineInputAssemblyStateCreateInfo() const noexcept {
    return get();
}

VkPipelineInputAssemblyStateCreateInfo &vul::PipelineInputAssemblyStateCreateInfo::get() noexcept {
    return *reinterpret_cast<VkPipelineInputAssemblyStateCreateInfo *>(this);
}

const VkPipelineInputAssemblyStateCreateInfo &vul::PipelineInputAssemblyStateCreateInfo::get() const noexcept {
    return *reinterpret_cast<const VkPipelineInputAssemblyStateCreateInfo *>(this);
}

vul::PipelineInputAssemblyStateCreateInfo vul::PipelineInputAssemblyStateCreateInfo::fromTopology(
        vul::PrimitiveTopology topology) noexcept {
    PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.pNext = nullptr;
    inputAssemblyStateCreateInfo.flags = {};
    inputAssemblyStateCreateInfo.topology = topology;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = false;
    return inputAssemblyStateCreateInfo;
}

vul::PipelineInputAssemblyStateCreateInfo::PipelineInputAssemblyStateCreateInfo(
        const vul::PipelineInputAssemblyStateCreateInfo &rhs) noexcept:
        pNext(rhs.pNext), flags(rhs.flags), topology(rhs.topology), primitiveRestartEnable(rhs.primitiveRestartEnable) {

}
vul::PipelineInputAssemblyStateCreateInfo::PipelineInputAssemblyStateCreateInfo(
        vul::PipelineInputAssemblyStateCreateInfo &&rhs) noexcept:
        pNext(rhs.pNext), flags(rhs.flags), topology(rhs.topology), primitiveRestartEnable(rhs.primitiveRestartEnable) {

}

vul::PipelineInputAssemblyStateCreateInfo &
vul::PipelineInputAssemblyStateCreateInfo::operator=(const vul::PipelineInputAssemblyStateCreateInfo &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        flags = rhs.flags;
        topology = rhs.topology;
        primitiveRestartEnable = rhs.primitiveRestartEnable;
    }
    return *this;
}


vul::PipelineInputAssemblyStateCreateInfo::PipelineInputAssemblyStateCreateInfo() :
        pNext(nullptr), flags(0), topology(PrimitiveTopology{}), primitiveRestartEnable(false){
}

vul::PipelineInputAssemblyStateCreateInfo::~PipelineInputAssemblyStateCreateInfo() = default;