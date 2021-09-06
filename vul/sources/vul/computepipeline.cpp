//
// Created by Shae Bolt on 8/30/2021.
//

#include "computepipeline.h"
#include "vul/pipelinecache.h"
#include "vul/pipelinelayout.h"
#include "vul/shadermodule.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"
#include "vul/bitmasks.h"
#include "vul/containerutils.h"

vul::ComputePipeline::ComputePipeline(const vul::Device &device,
                                      VkPipeline handle,
                                      const VkAllocationCallbacks *pAllocator)
        : m_pDevice(
        &device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkPipeline vul::ComputePipeline::get() const {
    return m_handle;
}

vul::ComputePipeline::~ComputePipeline() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::ComputePipeline::ComputePipeline(vul::ComputePipeline &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::ComputePipeline &
vul::ComputePipeline::operator=(vul::ComputePipeline &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::ComputePipeline::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::ComputePipelineBuilder::ComputePipelineBuilder(const vul::Device &device,
                                                    const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator) {

}

void vul::ComputePipelineBuilder::setFlags(vul::PipelineCreateBitMask flags) {
    m_flags = flags.to_underlying();
}

void vul::ComputePipelineBuilder::setShaderCreateInfo(
        vul::PipelineComputeShaderStageCreateInfo computeInfo) {
    m_computeStage = computeInfo.get();
}

void vul::ComputePipelineBuilder::setPipelineLayout(
        const vul::PipelineLayout &pipelineLayout) {
    m_layout = pipelineLayout.get();
}


void vul::ComputePipelineBuilder::setBasePipeline(
        const vul::ComputePipeline &basePipeline,
        std::int32_t basePipelineIndex) {
    m_basePipelineHandle = basePipeline.get();
    m_basePipelineIndex = basePipelineIndex;
}

vul::ExpectedResult<vul::ComputePipeline> vul::ComputePipelineBuilder::create(
        const vul::PipelineCache &pipelineCache) const {
    return create(pipelineCache.get());
}

vul::ExpectedResult<vul::ComputePipeline>
vul::ComputePipelineBuilder::create() const {
    return create(VK_NULL_HANDLE);
}

vul::ExpectedResult<vul::ComputePipeline>
vul::ComputePipelineBuilder::create(VkPipelineCache pipelineCache) const {
    VkComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.pNext = nullptr;
    computePipelineCreateInfo.flags = m_flags;
    computePipelineCreateInfo.stage = m_computeStage;
    computePipelineCreateInfo.layout = m_layout;
    computePipelineCreateInfo.basePipelineHandle = m_basePipelineHandle;
    computePipelineCreateInfo.basePipelineIndex = m_basePipelineIndex;

    VkPipeline pipeline;
    auto result = static_cast<Result>(vkCreateComputePipelines(m_pDevice->get(), pipelineCache, 1, &computePipelineCreateInfo, m_pAllocator, &pipeline));
    return {result, ComputePipeline(*m_pDevice, pipeline, m_pAllocator)};
}
