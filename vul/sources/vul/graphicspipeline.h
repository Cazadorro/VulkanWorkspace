//
// Created by Shae Bolt on 8/24/2021.
//

#ifndef VULKANWORKSPACE_GRAPHICSPIPELINE_H
#define VULKANWORKSPACE_GRAPHICSPIPELINE_H

#include "vul/vertexinputstate.h"
#include "vul/bitmasksfwd.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <unordered_map>

namespace vul {

    template<typename T>
    class ExpectedResult;

    class Device;

    class PipelineLayout;

    class RenderPass;

    class PipelineCache;

    class GraphicsPipeline {
    public:
        GraphicsPipeline() = default;

        GraphicsPipeline(const Device &device,
                         VkPipeline handle,
                         const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkPipeline get() const;

        ~GraphicsPipeline();

        GraphicsPipeline(GraphicsPipeline &&rhs) noexcept;

//        was noexcept?
        GraphicsPipeline &operator=(GraphicsPipeline &&rhs) noexcept;

        GraphicsPipeline(const GraphicsPipeline &rhs) = delete;

        GraphicsPipeline &operator=(GraphicsPipeline &rhs) = delete;

        Result setObjectName(const std::string &string);

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkPipeline m_handle = VK_NULL_HANDLE;
    };

    class PipelineVertexShaderStageCreateInfo;
    class PipelineFragmentShaderStageCreateInfo;
    class GraphicsPipelineBuilder {
    public:

        explicit GraphicsPipelineBuilder(const Device &device,
                                         const VkAllocationCallbacks *pAllocator = nullptr);

        void setFlags(vul::PipelineCreateBitMask flags);

        void setShaderCreateInfo(
                PipelineVertexShaderStageCreateInfo vertexInfo,
                PipelineFragmentShaderStageCreateInfo fragmentInfo);
//        std::uint32_t binding = 0, vul::VertexInputRate inputRate = vul::VertexInputRate::Vertex

        template<typename T>
        void setVertexBinding(std::uint32_t binding = 0,
                              vul::VertexInputRate inputRate = vul::VertexInputRate::Vertex) {
            auto attributeDescriptions = createVertexAttributeDescriptions<T>(
                    binding);
            m_bindingAttributeDescriptions[binding] = InputBindingAttributes{
                    createVertexBindingDescription<T>(binding, inputRate),
                    std::vector(attributeDescriptions.begin(), attributeDescriptions.end())};
        }

        void setPrimitiveStateInfo(vul::PrimitiveTopology topology,
                                   VkBool32 primitiveRestartEnable = VK_FALSE,
                                   const void *pNext = nullptr);

        void setViewportState(const std::vector<VkViewport> &viewports,
                              const std::vector<VkRect2D> &scissor);

        void setViewportStateFromExtent(const VkExtent2D &extent);

        void setRasterizationState(
                const VkPipelineRasterizationStateCreateInfo &rasterizer);

        void setDefaultRasterizationState();

        void setMultisampleState(
                const VkPipelineMultisampleStateCreateInfo &multisampling);

        void setDefaultMultisampleState();

        void setDepthStencilState(
                const VkPipelineDepthStencilStateCreateInfo &depthStencilState);

        void setDefaultDepthStencilStateEnable();

        void setBlendState(
                const std::vector<VkPipelineColorBlendAttachmentState> &blendAttachmentStates,
                const std::array<float, 4> &blendConstants = {0.0f, 0.0f, 0.0f,
                                                              0.0f});

        void setBlendState(vul::LogicOp logicOp,
                           const std::vector<VkPipelineColorBlendAttachmentState> &blendAttachmentStates,
                           const std::array<float, 4> &blendConstants = {0.0f,
                                                                         0.0f,
                                                                         0.0f,
                                                                         0.0f});

        void setDefaultBlendState();

        void
        setDynamicState(const std::vector<vul::DynamicState> &dynamicStates);

        void setPipelineLayout(const PipelineLayout &pipelineLayout);

        void setRenderPass(const RenderPass &renderpass, std::uint32_t subpass);

        void setBasePipeline(const GraphicsPipeline &basePipeline,
                             std::int32_t basePipelineIndex);

        void setTesselationState(uint32_t patchControlPoints);

        [[nodiscard]]
        ExpectedResult<GraphicsPipeline>
        create(VkPipelineCache pipelineCache) const;
        [[nodiscard]]
        ExpectedResult<GraphicsPipeline>
        create(const PipelineCache &pipelineCache) const;
        [[nodiscard]]
        ExpectedResult<GraphicsPipeline>
        create() const;

    private:

        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkPipelineCreateFlags m_flags = {};
        VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyInfo = {};
        std::array<VkPipelineShaderStageCreateInfo, 2> m_shaderStages;
        struct InputBindingAttributes {
            VkVertexInputBindingDescription bindingDescription = {};
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        };
        std::unordered_map<std::uint32_t, InputBindingAttributes> m_bindingAttributeDescriptions;
        std::vector<VkViewport> m_viewports;
        std::vector<VkRect2D> m_scissors;
        VkPipelineRasterizationStateCreateInfo m_rasterizationState = {};
        VkPipelineDepthStencilStateCreateInfo m_depthStencilState = {};
        VkPipelineMultisampleStateCreateInfo m_multiSampleState = {};
        std::vector<VkPipelineColorBlendAttachmentState> m_blendAttachmentStates;
        std::array<float, 4> m_blendConstants = {0.0f, 0.0f, 0.0f, 0.0f};
        std::optional<vul::LogicOp> m_logicOp;
        std::vector<vul::DynamicState> m_dynamicStates;
        VkPipelineLayout m_layout = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        std::uint32_t m_subpass = 0;
        VkPipeline m_basePipelineHandle = VK_NULL_HANDLE;
        std::int32_t m_basePipelineIndex = 0;
        std::uint32_t m_patchControlPoints = 0;
    };
}
#endif //VULKANWORKSPACE_GRAPHICSPIPELINE_H
