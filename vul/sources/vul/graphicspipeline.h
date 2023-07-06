//
// Created by Shae Bolt on 8/24/2021.
//

#ifndef VULKANWORKSPACE_GRAPHICSPIPELINE_H
#define VULKANWORKSPACE_GRAPHICSPIPELINE_H

#include "vul/vertexinputstate.h"
#include "vul/pipelineinputassemblystatecreateinfo.h"
#include "vul/pipelinecolorblendattachmentstate.h"
#include "vul/pipelinedepthstencilstatecreateinfo.h"
#include "vul/pipelinerasterizationstatecreateinfo.h"
#include "vul/pipelinemultisamplestatecreateinfo.h"
#include <uul/enumflagsfwd.h>
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

    struct Viewport;

    struct Rect2D;

    struct Extent2D;

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

        GraphicsPipeline &operator=(const GraphicsPipeline &rhs) = delete;

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

        void setFlags(uul::EnumFlags<vul::PipelineCreateFlagBits> flags);

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

        void setPrimitiveStateInfo(const PipelineInputAssemblyStateCreateInfo &inputAssembly =
                vul::PipelineInputAssemblyStateCreateInfo::fromTopology(vul::PrimitiveTopology::TriangleList)
        );

        void setViewportState(const std::vector<Viewport> &viewports, const std::vector<Rect2D> &scissor);

        void setViewportStateFromExtent(const Extent2D &extent);
        void setRasterizationState(
                const PipelineRasterizationStateCreateInfo &rasterizer =
                vul::PipelineRasterizationStateCreateInfo::fromCullMode(vul::CullModeFlagBits::BackBit)
        );


        void setMultisampleState(
                const PipelineMultisampleStateCreateInfo &multisampling =
                vul::PipelineMultisampleStateCreateInfo::fromSampleCount(vul::SampleCountFlagBits::_1Bit)
        );

        void setDepthStencilState(
                const PipelineDepthStencilStateCreateInfo &depthStencilState =
                vul::PipelineDepthStencilStateCreateInfo::fromCompareOp(vul::CompareOp::Less)
        );

        void setBlendState(
                const std::vector<vul::PipelineColorBlendAttachmentState> &blendAttachmentStates,
                const std::array<float, 4> &blendConstants = {0.0f, 0.0f, 0.0f,
                                                              0.0f});

        void setBlendState(vul::LogicOp logicOp,
                           const std::vector<vul::PipelineColorBlendAttachmentState> &blendAttachmentStates,
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
        uul::EnumFlags<vul::PipelineCreateFlagBits> m_flags = {};
        PipelineInputAssemblyStateCreateInfo m_inputAssemblyInfo = {};
        std::array<VkPipelineShaderStageCreateInfo, 2> m_shaderStages;
        struct InputBindingAttributes {
            VkVertexInputBindingDescription bindingDescription = {};
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        };
        std::unordered_map<std::uint32_t, InputBindingAttributes> m_bindingAttributeDescriptions;
        std::vector<Viewport> m_viewports;
        std::vector<Rect2D> m_scissors;
        PipelineRasterizationStateCreateInfo m_rasterizationState = {};
        PipelineDepthStencilStateCreateInfo m_depthStencilState = {};
        PipelineMultisampleStateCreateInfo m_multiSampleState = {};
        std::vector<vul::PipelineColorBlendAttachmentState> m_blendAttachmentStates;
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
