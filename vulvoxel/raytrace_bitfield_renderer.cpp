//
// Created by Bolt on 7/6/2023.
//

#include "raytrace_bitfield_renderer.h"
#include <vul/writedescriptorset.h>
#include <vul/pushconstantrange.h>
#include <vul/swapchain.h>
#include <vul/renderpass.h>
#include <vul/device.h>
#include <vul/enums.h>
#include <vul/temparrayproxy.h>
#include <range/v3/range/conversion.hpp>


vul::RaytraceBitfieldRenderer::RaytraceBitfieldRenderer(const vul::Device &device, std::size_t swapchainSize) {
    //TODO initially used swapchain size amounts of "bindless" images, but that doesn't actually help us
    //when we use a seperate command buffer for each frame
    m_dsetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device)
            .setFlags(vul::DescriptorSetLayoutCreateFlagBits::UpdateAfterBindPoolBit)
            .setBindings({vul::UniformBufferBinding(0, vul::ShaderStageFlagBits::ComputeBit),
                          vul::StorageImageBinding(1, vul::ShaderStageFlagBits::ComputeBit)},
                         {vul::DescriptorBindingFlagBits::UpdateAfterBindBit}
            );

    m_descriptorTypeCounts = m_dsetLayoutBuilder.createDescriptorTypeCounts();
    m_dsetLayout = m_dsetLayoutBuilder.create().assertValue();
    m_pipelineLayout = device.createPipelineLayout(
            m_dsetLayout,
            vul::PushConstantRange::create<PushConstant>(
                    vul::ShaderStageFlagBits::ComputeBit)).assertValue();
    auto pipeline_builder = vul::ComputePipelineBuilder(device);
    auto compute_shader = device.createShaderModule("spirv/raytrace_bitfield.comp.spv").assertValue();
    pipeline_builder.setShaderCreateInfo(compute_shader.createComputeStageInfo());
    pipeline_builder.setPipelineLayout(m_pipelineLayout);
    m_pipeline = pipeline_builder.create().assertValue();
}

const vul::PipelineLayout &vul::RaytraceBitfieldRenderer::getPipelineLayout() const {
    return m_pipelineLayout;
}

const vul::ComputePipeline &vul::RaytraceBitfieldRenderer::getPipeline() const {
    return m_pipeline;
}

const vul::DescriptorSetLayout &vul::RaytraceBitfieldRenderer::getDescriptorSetLayout() const {
    return m_dsetLayout;
}

const vul::DescriptorSetLayoutBuilder &vul::RaytraceBitfieldRenderer::getDescriptorSetLayoutBuilder() const {
    return m_dsetLayoutBuilder;
}

const std::vector<vul::DescriptorTypeCount>& vul::RaytraceBitfieldRenderer::getDescriptorTypeCounts() const {
    return m_descriptorTypeCounts;
}

vul::RaytraceBitfieldDisplayRenderer::RaytraceBitfieldDisplayRenderer(const vul::Device &device, const Swapchain& swapchain, const RenderPass& renderPass)
{

    m_dsetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    m_dsetLayoutBuilder.setFlags(vul::DescriptorSetLayoutCreateFlagBits::UpdateAfterBindPoolBit);
    m_dsetLayoutBuilder.setBindings(
            {vul::CombinedSamplerBinding(0,vul::ShaderStageFlagBits::FragmentBit)},
            {vul::DescriptorBindingFlagBits::UpdateAfterBindBit});
    m_descriptorTypeCounts = m_dsetLayoutBuilder.createDescriptorTypeCounts();
    m_dsetLayout = m_dsetLayoutBuilder.create().assertValue();

    m_pipelineLayout = device.createPipelineLayout(m_dsetLayout).assertValue();
    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);
    pipelineBuilder.setPrimitiveStateInfo();
    pipelineBuilder.setViewportStateFromExtent(swapchain.getExtent());
    pipelineBuilder.setDynamicState({vul::DynamicState::Viewport, vul::DynamicState::Scissor});
    pipelineBuilder.setRasterizationState();
    pipelineBuilder.setMultisampleState();
    pipelineBuilder.setDepthStencilState();
    pipelineBuilder.setBlendState({vul::PipelineColorBlendAttachmentState::disableBlendRGBA()});
    pipelineBuilder.setRenderPass(renderPass, 0);
    pipelineBuilder.setPipelineLayout(m_pipelineLayout);
    auto vertexShader = device.createShaderModule("spirv/fullscreenr32f.vert.spv").assertValue();
    auto fragmentShader = device.createShaderModule("spirv/fullscreenr32f.frag.spv").assertValue();
    pipelineBuilder.setShaderCreateInfo(
            vertexShader.createVertexStageInfo(),
            fragmentShader.createFragmentStageInfo());
    m_pipeline = pipelineBuilder.create().assertValue();
}

const vul::PipelineLayout &vul::RaytraceBitfieldDisplayRenderer::getPipelineLayout() const {
    return m_pipelineLayout;
}

const vul::GraphicsPipeline &vul::RaytraceBitfieldDisplayRenderer::getPipeline() const {
    return m_pipeline;
}

const vul::DescriptorSetLayout &vul::RaytraceBitfieldDisplayRenderer::getDescriptorSetLayout() const {
    return m_dsetLayout;
}

const vul::DescriptorSetLayoutBuilder &vul::RaytraceBitfieldDisplayRenderer::getDescriptorSetLayoutBuilder() const {
    return m_dsetLayoutBuilder;
}

const std::vector<vul::DescriptorTypeCount>& vul::RaytraceBitfieldDisplayRenderer::getDescriptorTypeCounts() const {
    return m_descriptorTypeCounts;
}

