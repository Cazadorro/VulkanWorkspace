//
// Created by Shae Bolt on 8/24/2021.
//

#ifndef VULKANWORKSPACE_GRAPHICSPIPELINE_H
#define VULKANWORKSPACE_GRAPHICSPIPELINE_H
#include "vul/vertexinputstate.h"
#include "vul/shadermodule.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

namespace vul{

//    VkPipelineRasterizationStateCreateInfo rasterizationState = vks::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
//    VkPipelineColorBlendAttachmentState blendAttachmentState = vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
//    VkPipelineColorBlendStateCreateInfo colorBlendState = vks::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
//    VkPipelineDepthStencilStateCreateInfo depthStencilState = vks::initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
//    VkPipelineViewportStateCreateInfo viewportState = vks::initializers::pipelineViewportStateCreateInfo(1, 1, 0);
//    VkPipelineMultisampleStateCreateInfo multisampleState = vks::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
//    std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
//    VkPipelineDynamicStateCreateInfo dynamicState = vks::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
//    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;
//
//    // Final fullscreen pass pipeline
//    VkGraphicsPipelineCreateInfo pipelineCI = vks::initializers::pipelineCreateInfo(pipelineLayouts.offscreen, renderPass, 0);
//    pipelineCI.pInputAssemblyState = &inputAssemblyState;
//    pipelineCI.pRasterizationState = &rasterizationState;
//    pipelineCI.pColorBlendState = &colorBlendState;
//    pipelineCI.pMultisampleState = &multisampleState;
//    pipelineCI.pViewportState = &viewportState;
//    pipelineCI.pDepthStencilState = &depthStencilState;
//    pipelineCI.pDynamicState = &dynamicState;
//    pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
//    pipelineCI.pStages = shaderStages.data();
//    pipelineCI.subpass = 0;
//    pipelineCI.pVertexInputState = vkglTF::Vertex::getPipelineVertexInputState({vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Color, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV});
//
//    std::array<VkPipelineColorBlendAttachmentState, 4> blendAttachmentStates = {
//            vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE),
//            vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE),
//            vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE),
//            vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE)
//    };
//
//    colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
//    colorBlendState.pAttachments = blendAttachmentStates.data();
//
//    // Offscreen scene rendering pipeline
//    shaderStages[0] = loadShader(getShadersPath() + "subpasses/gbuffer.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
//    shaderStages[1] = loadShader(getShadersPath() + "subpasses/gbuffer.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
//    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.offscreen));
    class GraphicsPipelineBuilder{
    public:
        void setShaderCreateInfo(
                PipelineVertexShaderStageCreateInfo vertexInfo,
                PipelineFragmentShaderStageCreateInfo fragmentInfo);
//        std::uint32_t binding = 0, vul::VertexInputRate inputRate = vul::VertexInputRate::Vertex
        void clearBindings();

        template<typename T>
        void addBinding(std::uint32_t binding = 0, vul::VertexInputRate inputRate = vul::VertexInputRate::Vertex){
            m_bindingDescriptions.push_back(createVertexBindingDescription<T>(binding, inputRate));
            auto attributeDescriptions = createVertexAttributeDescriptions<T>(binding);
            m_attributeDescriptions.insert(m_attributeDescriptions.end(), attributeDescriptions.begin(), m_attributeDescriptions.end());
        }

        void setPrimitiveStateInfo(vul::PrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE, const void* pNext = nullptr);


        void setViewportState(const std::vector<VkViewport>& viewports, const std::vector<VkViewport>& scissor);
        void setDefaultViewportState();
        void setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizer);
        void setDefaultRasterizationState();
        void setMultisampleState(const VkPipelineMultisampleStateCreateInfo & multisampling);
        void setDefaultMultisampleState();
        void setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo  & depthStencilState);
        void setDefaultDepthStencilState();
        void setBlendState(const std::vector<VkPipelineColorBlendAttachmentState>& blendAttachmentStates, const std::array<float,4>& blendConstants = {0.0f,0.0f,0.0f,0.0f});
        void setBlendState(vul::LogicOp logicOp, const std::vector<VkPipelineColorBlendAttachmentState>& blendAttachmentStates, const std::array<float,4>& blendConstants = {0.0f,0.0f,0.0f,0.0f});
        void setDefaultBlendState();

        //TODO pipeline layout which needs descriptor set layouts.
        //TODO add dynamic states

        //TODO VkRenderPass      renderPass;
        //TODO uint32_t          subpass;
        //TODO VkPipeline        basePipelineHandle;
        //TODO int32_t           basePipelineIndex;

        //TODO    const VkPipelineTessellationStateCreateInfo*     pTessellationState;


    private:
        VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyInfo;
        std::array<VkPipelineShaderStageCreateInfo,2> m_shaderStages;
        std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
        std::vector<VkViewport> m_viewports;
        std::vector<VkViewport> m_scissors;
        VkPipelineRasterizationStateCreateInfo m_rasterizationState;
        VkPipelineDepthStencilStateCreateInfo m_depthStencilState;
        std::vector<VkPipelineColorBlendAttachmentState>& m_blendAttachmentStates;
        std::optional<vul::LogicOp> m_logicOp;
    };
}
#endif //VULKANWORKSPACE_GRAPHICSPIPELINE_H
