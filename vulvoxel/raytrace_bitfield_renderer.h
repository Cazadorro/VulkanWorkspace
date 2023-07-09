//
// Created by Bolt on 7/6/2023.
//

#ifndef VULKANWORKSPACE_RAYTRACE_BITFIELD_RENDERER_H
#define VULKANWORKSPACE_RAYTRACE_BITFIELD_RENDERER_H


#include <vul/image.h>
#include <vul/imageview.h>
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




    class Device;
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
        const PipelineLayout& get_pipeline_layout() const;
        [[nodiscard]]
        const ComputePipeline& get_pipeline() const;
        [[nodiscard]]
        const DescriptorSetLayout& get_descriptor_set_layout() const;
        [[nodiscard]]
        const DescriptorSetLayoutBuilder& get_descriptor_set_layout_builder() const;
        [[nodiscard]]
        DescriptorSetUpdateBuilder create_descriptor_set_update_builder() const;
        private:
        vul::DescriptorSetLayoutBuilder m_dset_layout_builder;
        vul::DescriptorSetUpdateBuilder m_dset_updater;
        vul::DescriptorSetLayout m_dset_layout;
        vul::PipelineLayout m_pipeline_layout;
        vul::ComputePipeline m_pipeline;
    };

    class RaytraceBitfieldDisplayRenderer{
    public:
        struct alignas(8) PushConstant{
            std::uint32_t u_frame_idx;
        };
        explicit RaytraceBitfieldDisplayRenderer(const vul::Device &device);
        [[nodiscard]]
        const PipelineLayout& getPipelineLayout() const;
        [[nodiscard]]
        const ComputePipeline& getPipeline() const;
        [[nodiscard]]
        const DescriptorSetLayout& getDescriptorSetLayout() const;
        [[nodiscard]]
        const DescriptorSetLayoutBuilder& getDescriptorSetLayoutBuilder() const;
        [[nodiscard]]
        DescriptorSetUpdateBuilder createDescriptorSetUpdateBuilder() const;
        [[nodiscard]]
        std::vector<VkWriteDescriptorSet> createDescriptorWrites(
                const VkDescriptorSet& descriptorSet,
                const VkDescriptorBufferInfo& uniformInfo,
                const VkDescriptorImageInfo& storageImageInfo) const;

    private:
        vul::DescriptorSetLayoutBuilder m_dsetLayoutBuilder;
        vul::DescriptorSetUpdateBuilder m_dsetUpdateBuilder;
        vul::DescriptorSetLayout m_dsetLayout;
        vul::PipelineLayout m_pipelineLayout;
        vul::ComputePipeline m_pipeline;
    };


}
#endif //VULKANWORKSPACE_RAYTRACE_BITFIELD_RENDERER_H
