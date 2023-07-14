//
// Created by Bolt on 7/6/2023.
//

#ifndef VULKANWORKSPACE_RAYTRACE_BITFIELD_RENDERER_H
#define VULKANWORKSPACE_RAYTRACE_BITFIELD_RENDERER_H


#include <vul/image.h>
#include <vul/imageview.h>
#include <vul/graphicspipeline.h>
#include <vul/computepipeline.h>
#include <vul/pipelinelayout.h>
#include <vul/descriptorsetlayout.h>
#include <vul/descriptorset.h>
#include <vul/deviceaddress.h>
#include <vul/temparrayproxy.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace vul{




    struct DescriptorTypeCount;
    class Device;
    class Swapchain;
    class RenderPass;
    class RaytraceBitfieldRenderer{
    public:
        struct UniformBufferObject{
            glm::uvec2 u_resolution;
            float u_fov;
            float u_focus_distance;
        };
        struct alignas(8) PushConstant{
            glm::vec3 u_camera_origin;
            std::uint32_t u_frame_idx;
            glm::vec3 u_camera_rotation;
            float u_time;
            vul::DeviceAddress u_bitmask;
            std::uint64_t u_padding;
        };
        static_assert(sizeof(PushConstant) == 48);
        explicit RaytraceBitfieldRenderer(const vul::Device &device, std::size_t swapchainSize);
        [[nodiscard]]
        const PipelineLayout& getPipelineLayout() const;
        [[nodiscard]]
        const ComputePipeline& getPipeline() const;
        [[nodiscard]]
        const DescriptorSetLayout& getDescriptorSetLayout() const;
        [[nodiscard]]
        const DescriptorSetLayoutBuilder& getDescriptorSetLayoutBuilder() const;
        [[nodiscard]]
        const std::vector<DescriptorTypeCount>& getDescriptorTypeCounts() const;
        private:
        vul::DescriptorSetLayoutBuilder m_dsetLayoutBuilder;
        std::vector<vul::DescriptorTypeCount> m_descriptorTypeCounts;
        vul::DescriptorSetLayout m_dsetLayout;
        vul::PipelineLayout m_pipelineLayout;
        vul::ComputePipeline m_pipeline;
    };

    class RaytraceBitfieldDisplayRenderer{
    public:


        explicit RaytraceBitfieldDisplayRenderer(const vul::Device &device,
                                                 const Swapchain& swapchain,
                                                 const RenderPass& renderPass);
        [[nodiscard]]
        const PipelineLayout& getPipelineLayout() const;
        [[nodiscard]]
        const GraphicsPipeline& getPipeline() const;
        [[nodiscard]]
        const DescriptorSetLayout& getDescriptorSetLayout() const;
        [[nodiscard]]
        const DescriptorSetLayoutBuilder& getDescriptorSetLayoutBuilder() const;
        [[nodiscard]]
        const std::vector<vul::DescriptorTypeCount>& getDescriptorTypeCounts() const;

    private:
        vul::DescriptorSetLayoutBuilder m_dsetLayoutBuilder;
        std::vector<vul::DescriptorTypeCount> m_descriptorTypeCounts;
        vul::DescriptorSetLayout m_dsetLayout;
        vul::PipelineLayout m_pipelineLayout;
        vul::GraphicsPipeline m_pipeline;
    };


}
#endif //VULKANWORKSPACE_RAYTRACE_BITFIELD_RENDERER_H
