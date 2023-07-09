//
// Created by Bolt on 7/6/2023.
//

#include "raytrace_bitfield_renderer.h"
#include <vul/pushconstantrange.h>
#include <vul/device.h>
#include <vul/enums.h>
#include <vul/temparrayproxy.h>
#include <range/v3/range/conversion.hpp>


vul::RaytraceBitfieldRenderer::RaytraceBitfieldRenderer(const vul::Device &device, std::size_t swapchainSize) {
    m_dset_layout_builder = vul::DescriptorSetLayoutBuilder(device)
            .setFlags(vul::DescriptorSetLayoutCreateFlagBits::UpdateAfterBindPoolBit)
            .setBindings({vul::UniformBufferBinding(0, vul::ShaderStageFlagBits::ComputeBit),
                          vul::StorageImageBinding(1, vul::ShaderStageFlagBits::ComputeBit, swapchainSize)},
                         {vul::DescriptorBindingFlagBits::UpdateAfterBindBit}
            );
    m_dset_updater = m_dset_layout_builder.createUpdateBuilder();
    m_dset_layout = m_dset_layout_builder.create().assertValue();
    m_pipeline_layout = device.createPipelineLayout(
            m_dset_layout,
            vul::PushConstantRange::create<PushConstant>(
                    vul::ShaderStageFlagBits::ComputeBit)).assertValue();
    auto pipeline_builder = vul::ComputePipelineBuilder(device);
    auto compute_shader = device.createShaderModule("spirv/raytrace_bitfield.comp.spv").assertValue();
    pipeline_builder.setShaderCreateInfo(compute_shader.createComputeStageInfo());
    pipeline_builder.setPipelineLayout(m_pipeline_layout);
    m_pipeline = pipeline_builder.create().assertValue();
}

const vul::PipelineLayout &vul::RaytraceBitfieldRenderer::get_pipeline_layout() const {
    return m_pipeline_layout;
}

const vul::ComputePipeline &vul::RaytraceBitfieldRenderer::get_pipeline() const {
    return m_pipeline;
}

const vul::DescriptorSetLayout &vul::RaytraceBitfieldRenderer::get_descriptor_set_layout() const {
    return m_dset_layout;
}

const vul::DescriptorSetLayoutBuilder &vul::RaytraceBitfieldRenderer::get_descriptor_set_layout_builder() const {
    return m_dset_layout_builder;
}

vul::DescriptorSetUpdateBuilder vul::RaytraceBitfieldRenderer::create_descriptor_set_update_builder() const {
    return m_dset_updater;
}
