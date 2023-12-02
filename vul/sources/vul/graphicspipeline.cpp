//
// Created by Shae Bolt on 8/24/2021.
//

#include "vul/graphicspipeline.h"
#include "vul/renderpass.h"
#include "vul/pipelinecache.h"
#include "vul/pipelinelayout.h"
#include "vul/shadermodule.h"
#include "vul/vertexinputstate.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/enums.h"
#include "vul/containerutils.h"
#include "vul/vkstructutils.h"
#include <uul/enumflags.h>

vul::GraphicsPipeline::GraphicsPipeline(const vul::Device &device,
                                        VkPipeline handle,
                                        const VkAllocationCallbacks *pAllocator)
        : m_pDevice(
        &device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkPipeline vul::GraphicsPipeline::get() const {
    return m_handle;
}

vul::GraphicsPipeline::~GraphicsPipeline() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::GraphicsPipeline::GraphicsPipeline(vul::GraphicsPipeline &&rhs) noexcept : GraphicsPipeline(){
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::GraphicsPipeline &
vul::GraphicsPipeline::operator=(vul::GraphicsPipeline &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::GraphicsPipeline::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::GraphicsPipelineBuilder::GraphicsPipelineBuilder(const vul::Device &device,
                                                      const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator) {
}

void vul::GraphicsPipelineBuilder::setShaderCreateInfo(
        vul::PipelineVertexShaderStageCreateInfo vertexInfo,
        vul::PipelineFragmentShaderStageCreateInfo fragmentInfo) {
    m_shaderStages[0] = vertexInfo.get();
    m_shaderStages[1] = fragmentInfo.get();
}

void vul::GraphicsPipelineBuilder::setPrimitiveStateInfo(const PipelineInputAssemblyStateCreateInfo& inputAssembly) {
   m_inputAssemblyInfo = inputAssembly;
}

void vul::GraphicsPipelineBuilder::setViewportState(
        const std::vector<Viewport> &viewports,
        const std::vector<Rect2D> &scissor) {
    m_viewports = viewports;
    m_scissors = scissor;
}

void vul::GraphicsPipelineBuilder::setViewportStateFromExtent(
        const Extent2D &extent) {
    setViewportState({Viewport(extent)}, {Rect2D(extent)});
}

void vul::GraphicsPipelineBuilder::setRasterizationState(
        const PipelineRasterizationStateCreateInfo &rasterizer) {
    m_rasterizationState = rasterizer;
}


void vul::GraphicsPipelineBuilder::setMultisampleState(
        const PipelineMultisampleStateCreateInfo &multisampling) {
    m_multiSampleState = multisampling;
}

void vul::GraphicsPipelineBuilder::setDepthStencilState(
        const PipelineDepthStencilStateCreateInfo &depthStencilState) {
    m_depthStencilState = depthStencilState;
}

void vul::GraphicsPipelineBuilder::setBlendState(
        const std::vector<vul::PipelineColorBlendAttachmentState> &blendAttachmentStates,
        const std::array<float, 4> &blendConstants) {
    m_blendAttachmentStates = blendAttachmentStates;
    m_blendConstants = blendConstants;
}

void vul::GraphicsPipelineBuilder::setBlendState(vul::LogicOp logicOp,
                                                 const std::vector<vul::PipelineColorBlendAttachmentState> &blendAttachmentStates,
                                                 const std::array<float, 4> &blendConstants) {
    m_logicOp = logicOp;
    setBlendState(blendAttachmentStates, blendConstants);
}

void vul::GraphicsPipelineBuilder::setDefaultBlendState() {
    m_blendAttachmentStates.push_back(vul::PipelineColorBlendAttachmentState::disableBlendRGBA());
}

void vul::GraphicsPipelineBuilder::setDynamicState(
        const std::vector<vul::DynamicState> &dynamicStates) {
    m_dynamicStates = dynamicStates;
}

void vul::GraphicsPipelineBuilder::setPipelineLayout(
        const vul::PipelineLayout &pipelineLayout) {
    m_layout = pipelineLayout.get();
}

void
vul::GraphicsPipelineBuilder::setRenderPass(const vul::RenderPass &renderpass,
                                            std::uint32_t subpass) {
    m_renderPass = renderpass.get();
    m_subpass = subpass;
}

void vul::GraphicsPipelineBuilder::setBasePipeline(
        const vul::GraphicsPipeline &basePipeline,
        std::int32_t basePipelineIndex) {
    m_basePipelineHandle = basePipeline.get();
    m_basePipelineIndex = basePipelineIndex;
}

void
vul::GraphicsPipelineBuilder::setTesselationState(uint32_t patchControlPoints) {
    m_patchControlPoints = patchControlPoints;
}



void vul::GraphicsPipelineBuilder::setFlags(uul::EnumFlags<vul::PipelineCreateFlagBits> flags) {
    m_flags = flags;
}

vul::ExpectedResult<vul::GraphicsPipeline>
vul::GraphicsPipelineBuilder::create(const PipelineCache &pipelineCache) const {
    return create(pipelineCache.get());
}

vul::ExpectedResult<vul::GraphicsPipeline>
vul::GraphicsPipelineBuilder::create() const {
    return create(VK_NULL_HANDLE);
}

vul::ExpectedResult<vul::GraphicsPipeline>
vul::GraphicsPipelineBuilder::create(VkPipelineCache pipelineCache) const {

    std::vector<VkVertexInputBindingDescription> inputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    for (const auto&[key, value]: m_bindingAttributeDescriptions) {
        inputBindingDescriptions.push_back(value.bindingDescription);
        vul::extend(attributeDescriptions, value.attributeDescriptions);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    if(m_bindingAttributeDescriptions.size() != 0){
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = {};
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<std::uint32_t>(inputBindingDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = inputBindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    }else{
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    }


    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = static_cast<std::uint32_t>(m_viewports.size());
    viewportState.pViewports = reinterpret_cast<const VkViewport*>(m_viewports.data());
    viewportState.scissorCount = static_cast<std::uint32_t>(m_scissors.size());
    viewportState.pScissors = reinterpret_cast<const VkRect2D*>(m_scissors.data());


    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    if (m_logicOp.has_value()) {
        colorBlending.logicOpEnable = VK_TRUE;
        colorBlending.logicOp = vul::get(m_logicOp.value());
    } else {
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
    }
    colorBlending.attachmentCount = static_cast<std::uint32_t>(m_blendAttachmentStates.size());
    colorBlending.pAttachments = reinterpret_cast<const VkPipelineColorBlendAttachmentState*>(m_blendAttachmentStates.data());
    colorBlending.blendConstants[0] = m_blendConstants[0];
    colorBlending.blendConstants[1] = m_blendConstants[1];
    colorBlending.blendConstants[2] = m_blendConstants[2];
    colorBlending.blendConstants[3] = m_blendConstants[3];

    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.pNext = nullptr;
    dynamicStateInfo.flags = {};
    dynamicStateInfo.dynamicStateCount = static_cast<std::uint32_t>(m_dynamicStates.size());
    dynamicStateInfo.pDynamicStates = reinterpret_cast<const VkDynamicState *>(m_dynamicStates.data());

    VkPipelineTessellationStateCreateInfo tesselationStateInfo = {};
    tesselationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tesselationStateInfo.pNext = nullptr;
    tesselationStateInfo.flags = {};
    tesselationStateInfo.patchControlPoints = m_patchControlPoints;
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.flags = m_flags.to_underlying();
    pipelineInfo.stageCount = static_cast<std::uint32_t>(m_shaderStages.size());
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_inputAssemblyInfo.get();
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizationState.get();
    pipelineInfo.pMultisampleState = &m_multiSampleState.get();
    pipelineInfo.pDepthStencilState = &m_depthStencilState.get();
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    if (m_patchControlPoints != 0) {
        pipelineInfo.pTessellationState = &tesselationStateInfo;
    }
    pipelineInfo.layout = m_layout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = m_subpass;
    pipelineInfo.basePipelineHandle = m_basePipelineHandle;
    pipelineInfo.basePipelineIndex = m_basePipelineIndex;

    VkPipeline graphicsPipeline;
    auto result = static_cast<Result>(vkCreateGraphicsPipelines(
            m_pDevice->get(), pipelineCache, 1, &pipelineInfo, m_pAllocator,
            &graphicsPipeline));

    return {result,
            GraphicsPipeline(*m_pDevice, graphicsPipeline, m_pAllocator)};

}

