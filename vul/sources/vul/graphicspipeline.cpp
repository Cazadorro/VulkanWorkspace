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
#include "vul/bitmasks.h"
#include "vul/containerutils.h"


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

vul::GraphicsPipeline::GraphicsPipeline(vul::GraphicsPipeline &&rhs) noexcept {
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
    m_inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_multiSampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
}

void vul::GraphicsPipelineBuilder::setShaderCreateInfo(
        vul::PipelineVertexShaderStageCreateInfo vertexInfo,
        vul::PipelineFragmentShaderStageCreateInfo fragmentInfo) {
    m_shaderStages[0] = vertexInfo.get();
    m_shaderStages[1] = fragmentInfo.get();
}

void vul::GraphicsPipelineBuilder::setPrimitiveStateInfo(
        vul::PrimitiveTopology topology, VkBool32 primitiveRestartEnable,
        const void *pNext) {
    m_inputAssemblyInfo.pNext = pNext;
    m_inputAssemblyInfo.flags = {};
    m_inputAssemblyInfo.topology = vul::get(topology);
    m_inputAssemblyInfo.primitiveRestartEnable = primitiveRestartEnable;
}

void vul::GraphicsPipelineBuilder::setViewportState(
        const std::vector<VkViewport> &viewports,
        const std::vector<VkRect2D> &scissor) {
    m_viewports = viewports;
    m_scissors = scissor;
}

void vul::GraphicsPipelineBuilder::setViewportStateFromExtent(
        const VkExtent2D &extent) {
    m_viewports.clear();
    m_scissors.clear();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    m_viewports.push_back(viewport);
    m_scissors.push_back(scissor);
}

void vul::GraphicsPipelineBuilder::setRasterizationState(
        const VkPipelineRasterizationStateCreateInfo &rasterizer) {
    m_rasterizationState = rasterizer;
}

void vul::GraphicsPipelineBuilder::setDefaultRasterizationState() {
    m_rasterizationState.depthClampEnable = VK_FALSE;
    m_rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    m_rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
//    m_rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    m_rasterizationState.cullMode = VK_CULL_MODE_NONE;
    m_rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_rasterizationState.depthBiasEnable = VK_FALSE;
//    m_rasterizationState.depthBiasConstantFactor;
//    m_rasterizationState.depthBiasClamp;
//    m_rasterizationState.depthBiasSlopeFactor;
    m_rasterizationState.lineWidth = 1.0f;
}

void vul::GraphicsPipelineBuilder::setMultisampleState(
        const VkPipelineMultisampleStateCreateInfo &multisampling) {
    m_multiSampleState = multisampling;
}

void vul::GraphicsPipelineBuilder::setDefaultMultisampleState() {
    m_multiSampleState.pNext = nullptr;
    m_multiSampleState.flags = 0;
    m_multiSampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    m_multiSampleState.sampleShadingEnable = VK_FALSE;
    m_multiSampleState.minSampleShading = 0;
    m_multiSampleState.pSampleMask = nullptr;
    m_multiSampleState.alphaToCoverageEnable = VK_FALSE;
    m_multiSampleState.alphaToOneEnable = VK_FALSE;
}

void vul::GraphicsPipelineBuilder::setDepthStencilState(
        const VkPipelineDepthStencilStateCreateInfo &depthStencilState) {
    m_depthStencilState = depthStencilState;
}

void vul::GraphicsPipelineBuilder::setDefaultDepthStencilStateEnable() {
    m_depthStencilState.pNext = nullptr;
    m_depthStencilState.flags = {};
    m_depthStencilState.depthTestEnable = VK_TRUE;
    m_depthStencilState.depthWriteEnable = VK_TRUE;
    m_depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    m_depthStencilState.depthBoundsTestEnable = VK_FALSE;
    m_depthStencilState.stencilTestEnable = VK_FALSE;
//    m_depthStencilState.front;
//    m_depthStencilState.back;
//    m_depthStencilState.minDepthBounds;
//    m_depthStencilState.maxDepthBounds;
}

void vul::GraphicsPipelineBuilder::setBlendState(
        const std::vector<VkPipelineColorBlendAttachmentState> &blendAttachmentStates,
        const std::array<float, 4> &blendConstants) {
    m_blendAttachmentStates = blendAttachmentStates;
    m_blendConstants = blendConstants;
}

void vul::GraphicsPipelineBuilder::setBlendState(vul::LogicOp logicOp,
                                                 const std::vector<VkPipelineColorBlendAttachmentState> &blendAttachmentStates,
                                                 const std::array<float, 4> &blendConstants) {
    m_logicOp = logicOp;
    setBlendState(blendAttachmentStates, blendConstants);
}

void vul::GraphicsPipelineBuilder::setDefaultBlendState() {
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    m_blendAttachmentStates.push_back(colorBlendAttachment);
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



void vul::GraphicsPipelineBuilder::setFlags(vul::PipelineCreateBitMask flags) {
    m_flags = flags.to_underlying();
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
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = {};
    vertexInputInfo.vertexBindingDescriptionCount = inputBindingDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = inputBindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = m_viewports.size();
    viewportState.pViewports = m_viewports.data();
    viewportState.scissorCount = m_scissors.size();
    viewportState.pScissors = m_scissors.data();


    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    if (m_logicOp.has_value()) {
        colorBlending.logicOpEnable = VK_TRUE;
        colorBlending.logicOp = vul::get(m_logicOp.value());
    } else {
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
    }
    colorBlending.attachmentCount = m_blendAttachmentStates.size();
    colorBlending.pAttachments = m_blendAttachmentStates.data();
    colorBlending.blendConstants[0] = m_blendConstants[0];
    colorBlending.blendConstants[1] = m_blendConstants[1];
    colorBlending.blendConstants[2] = m_blendConstants[2];
    colorBlending.blendConstants[3] = m_blendConstants[3];

    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.pNext = nullptr;
    dynamicStateInfo.flags = {};
    dynamicStateInfo.dynamicStateCount = m_dynamicStates.size();
    dynamicStateInfo.pDynamicStates = reinterpret_cast<const VkDynamicState *>(m_dynamicStates.data());

    VkPipelineTessellationStateCreateInfo tesselationStateInfo = {};
    tesselationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tesselationStateInfo.pNext = nullptr;
    tesselationStateInfo.flags = {};
    tesselationStateInfo.patchControlPoints = m_patchControlPoints;
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.flags = m_flags;
    pipelineInfo.stageCount = m_shaderStages.size();
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizationState;
    pipelineInfo.pMultisampleState = &m_multiSampleState;
    pipelineInfo.pDepthStencilState = &m_depthStencilState;
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
