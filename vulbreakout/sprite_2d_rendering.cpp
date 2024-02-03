//
// Created by User on 1/16/2024.
//

#include "sprite_2d_rendering.h"
#include <vul/commandbuffer.h>
#include <vul/buffermemorybarrier.h>
#include <vul/vmaallocator.h>
#include <vul/vmaallocation.h>
#include <vul/renderpass.h>
#include <vul/pushconstantrange.h>
#include <vul/shadermodule.h>
#include <vul/descriptorset.h>

#include <czdr/utility/assert.h>

namespace game {

    BufferedSprite2dSynchronizationState::BufferedSprite2dSynchronizationState(vul::Device &device,
                                                                               std::uint32_t swapchain_size,
                                                                               std::uint64_t initial_state)
            :
            swapchain_size(swapchain_size),
            upload_finished_semaphore(device.createTimelineSemaphore(initial_state).assertValue()),
            frames_using_semaphore(device.createTimelineSemaphore(initial_state).assertValue()) {}

    std::size_t HostBufferedSprite2dInterpolatedDrawState::size() const {
        return host_material_id_arrays.size();
    }

    HostBufferedSprite2dInterpolatedDrawState::HostBufferedSprite2dInterpolatedDrawState(
            vul::VmaAllocator &device_allocator, std::size_t element_count) {
        for (std::size_t i = 0; i < buffer_count; ++i) {
            host_curr_translation_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(glm::vec2) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
            host_curr_scale_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(glm::vec2) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
            host_curr_shear_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(glm::vec2) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
            host_curr_rotation_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(float) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();

            host_prev_translation_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(glm::vec2) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
            host_prev_scale_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(glm::vec2) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
            host_prev_shear_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(glm::vec2) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
            host_prev_rotation_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(float) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();

            host_material_id_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(std::uint32_t) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
        }
    }

    void
    HostBufferedSprite2dInterpolatedDrawState::set_curr_transform(std::uint64_t buffer_index, std::size_t idx, const Transform2D &transform) {
        CZDR_ASSERT(idx < size());
        CZDR_ASSERT(buffer_index < buffer_count);
        reinterpret_cast<glm::vec2 *>(host_curr_translation_arrays[idx].mapMemory())[buffer_index] = transform.translation;
        reinterpret_cast<glm::vec2 *>(host_curr_scale_arrays[idx].mapMemory())[buffer_index] = transform.scale;
        reinterpret_cast<glm::vec2 *>(host_curr_shear_arrays[idx].mapMemory())[buffer_index] = transform.shear;
        reinterpret_cast<float *>(host_curr_rotation_arrays[idx].mapMemory())[buffer_index] = transform.rotation;
    }

    void
    HostBufferedSprite2dInterpolatedDrawState::set_prev_transform(std::uint64_t buffer_index, std::size_t idx, const Transform2D &transform) {
        CZDR_ASSERT(idx < size());
        CZDR_ASSERT(buffer_index < buffer_count);
        reinterpret_cast<glm::vec2 *>(host_prev_translation_arrays[buffer_index].mapMemory())[idx] = transform.translation;
        reinterpret_cast<glm::vec2 *>(host_prev_scale_arrays[buffer_index].mapMemory())[idx] = transform.scale;
        reinterpret_cast<glm::vec2 *>(host_prev_shear_arrays[buffer_index].mapMemory())[idx] = transform.shear;
        reinterpret_cast<float *>(host_prev_rotation_arrays[buffer_index].mapMemory())[idx] = transform.rotation;
    }

    void HostBufferedSprite2dInterpolatedDrawState::set_material_id(std::uint64_t buffer_index, std::size_t idx, std::uint32_t material_id) {
        reinterpret_cast<std::uint32_t *>(host_material_id_arrays[buffer_index].mapMemory())[idx] = material_id;
    }

    std::size_t DeviceBufferedSprite2dInterpolatedDrawState::size() const {
        return device_material_id_arrays.size();
    }

    DeviceBufferedSprite2dInterpolatedDrawState::DeviceBufferedSprite2dInterpolatedDrawState(
            vul::VmaAllocator &device_allocator, std::size_t element_count) {
        static constexpr auto usage_flags =
                vul::BufferUsageFlagBits::TransferSrcBit | vul::BufferUsageFlagBits::ShaderDeviceAddressBit;
        for (std::size_t i = 0; i < buffer_count; ++i) {

            device_curr_translation_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(glm::vec2) * element_count,
                                                        usage_flags).assertValue();
            device_curr_scale_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(glm::vec2) * element_count,
                                                        usage_flags).assertValue();
            device_curr_shear_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(glm::vec2) * element_count,
                                                        usage_flags).assertValue();
            device_curr_rotation_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(float) * element_count,
                                                        usage_flags).assertValue();

            device_prev_translation_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(glm::vec2) * element_count,
                                                        usage_flags).assertValue();
            device_prev_scale_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(glm::vec2) * element_count,
                                                        usage_flags).assertValue();
            device_prev_shear_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(glm::vec2) * element_count,
                                                        usage_flags).assertValue();
            device_prev_rotation_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(float) * element_count,
                                                        usage_flags).assertValue();

            device_material_id_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(std::uint32_t) * element_count,
                                                        usage_flags).assertValue();


            device_curr_arrays_reference[i] = device_allocator.createDeviceBuffer(
                    {TransformSoaReference{
                            device_curr_translation_arrays[i].getDeviceAddress(),
                            device_curr_scale_arrays[i].getDeviceAddress(),
                            device_curr_shear_arrays[i].getDeviceAddress(),
                            device_curr_rotation_arrays[i].getDeviceAddress(),
                    }},
                    vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

            //TODO add "default staging command pool and buffer"
            //TODO would need to externally synchronize the command poole (easy enough), but the queue could prove to be problematic.
            //TODO would need to add sychronization around queue. Re-creating stream concept from cuda? (queue + command pool? )

            device_prev_arrays_reference[i] = device_allocator.createDeviceBuffer(
                    {TransformSoaReference{
                            device_prev_translation_arrays[i].getDeviceAddress(),
                            device_prev_scale_arrays[i].getDeviceAddress(),
                            device_prev_shear_arrays[i].getDeviceAddress(),
                            device_prev_rotation_arrays[i].getDeviceAddress(),
                    }},
                    vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
        }
    }

    std::array<vul::BufferMemoryBarrier, 9>
    create_memory_barriers_from_transfer(std::uint32_t src_queue_index, std::uint32_t dst_queue_index,
                                         std::uint32_t double_buffer_index,
                                         const DeviceBufferedSprite2dInterpolatedDrawState &dst_state) {
        //TODO don't know if I need to specify dst access mask here if doing queue transfer?
        //https://github.com/KhronosGroup/Vulkan-ValidationLayers/pull/3337 doesn't matter for dst
        return {
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_translation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_scale_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_shear_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_rotation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),

                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_translation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_scale_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_shear_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_rotation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),

                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_material_id_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
        };
    }

    std::array<vul::BufferMemoryBarrier, 9>
    create_memory_barriers_to_vertex(std::uint32_t src_queue_index, std::uint32_t dst_queue_index,
                                     std::uint32_t double_buffer_index,
                                     const DeviceBufferedSprite2dInterpolatedDrawState &dst_state) {
        return {
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_translation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_scale_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_shear_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_curr_rotation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),

                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_translation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_scale_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_shear_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_prev_rotation_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),

                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_material_id_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
        };
    }

    void record_transfer_command(vul::CommandBuffer &transfer_command_buffer, std::uint32_t src_queue_index,
                                 std::uint32_t dst_queue_index, std::uint32_t double_buffer_index,
                                 const HostBufferedSprite2dInterpolatedDrawState &src_state,
                                 DeviceBufferedSprite2dInterpolatedDrawState &dst_state) {

        transfer_command_buffer.copyBuffer(src_state.host_curr_translation_arrays[double_buffer_index],
                                           dst_state.device_curr_translation_arrays[double_buffer_index]);
        transfer_command_buffer.copyBuffer(src_state.host_curr_scale_arrays[double_buffer_index],
                                           dst_state.device_curr_scale_arrays[double_buffer_index]);
        transfer_command_buffer.copyBuffer(src_state.host_curr_shear_arrays[double_buffer_index],
                                           dst_state.device_curr_shear_arrays[double_buffer_index]);
        transfer_command_buffer.copyBuffer(src_state.host_curr_rotation_arrays[double_buffer_index],
                                           dst_state.device_curr_rotation_arrays[double_buffer_index]);

        transfer_command_buffer.copyBuffer(src_state.host_prev_translation_arrays[double_buffer_index],
                                           dst_state.device_prev_translation_arrays[double_buffer_index]);
        transfer_command_buffer.copyBuffer(src_state.host_prev_scale_arrays[double_buffer_index],
                                           dst_state.device_prev_scale_arrays[double_buffer_index]);
        transfer_command_buffer.copyBuffer(src_state.host_prev_shear_arrays[double_buffer_index],
                                           dst_state.device_prev_shear_arrays[double_buffer_index]);
        transfer_command_buffer.copyBuffer(src_state.host_prev_rotation_arrays[double_buffer_index],
                                           dst_state.device_prev_rotation_arrays[double_buffer_index]);

        transfer_command_buffer.copyBuffer(src_state.host_material_id_arrays[double_buffer_index],
                                           dst_state.device_material_id_arrays[double_buffer_index]);


        auto transfer_to_graphics_barriers = create_memory_barriers_from_transfer(src_queue_index, dst_queue_index,
                                                                                  double_buffer_index,
                                                                                  dst_state);
        auto dependency_info = vul::DependencyInfo({}, transfer_to_graphics_barriers, {});
        transfer_command_buffer.pipelineBarrier(dependency_info);

    }


    std::size_t HostBufferedSprite2dGridDrawState::size() const {
        return host_material_id_arrays.size();
    }

    HostBufferedSprite2dGridDrawState::HostBufferedSprite2dGridDrawState(
            vul::VmaAllocator &device_allocator, std::size_t element_count) {
        for (std::size_t i = 0; i < buffer_count; ++i) {
            host_material_id_arrays[i] =
                    device_allocator.createMappedCoherentBuffer(sizeof(std::uint32_t) * element_count,
                                                                vul::BufferUsageFlagBits::TransferSrcBit).assertValue();
        }
    }

    void HostBufferedSprite2dGridDrawState::set_material_id(std::uint64_t buffer_index, std::size_t idx, std::uint32_t material_id) {
        reinterpret_cast<std::uint32_t *>(host_material_id_arrays[buffer_index].mapMemory())[idx] = material_id;
    }

    std::size_t DeviceBufferedSprite2dGridDrawState::size() const {
        return device_material_id_arrays.size();
    }

    DeviceBufferedSprite2dGridDrawState::DeviceBufferedSprite2dGridDrawState(
            vul::VmaAllocator &device_allocator, std::size_t element_count) {
        static constexpr auto usage_flags =
                vul::BufferUsageFlagBits::TransferSrcBit | vul::BufferUsageFlagBits::ShaderDeviceAddressBit;
        for (std::size_t i = 0; i < buffer_count; ++i) {
            device_material_id_arrays[i] =
                    device_allocator.createDeviceBuffer(sizeof(std::uint32_t) * element_count,
                                                        usage_flags).assertValue();
        }
    }

    std::array<vul::BufferMemoryBarrier, 1>
    create_memory_barriers_from_transfer(std::uint32_t src_queue_index, std::uint32_t dst_queue_index,
                                         std::uint32_t double_buffer_index,
                                         const DeviceBufferedSprite2dGridDrawState &dst_state) {
        return {
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_material_id_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::TopOfPipeBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
        };
    }

    std::array<vul::BufferMemoryBarrier, 1>
    create_memory_barriers_to_vertex(std::uint32_t src_queue_index, std::uint32_t dst_queue_index,
                                     std::uint32_t double_buffer_index,
                                     const DeviceBufferedSprite2dGridDrawState &dst_state) {
        return {
                vul::BufferMemoryBarrier::fromBuffer(
                        dst_state.device_material_id_arrays[double_buffer_index],
                        vul::PipelineStageFlagBits2::BottomOfPipeBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                        dst_queue_index,
                        src_queue_index
                ),
        };
    }

    void record_transfer_command(vul::CommandBuffer &transfer_command_buffer, std::uint32_t src_queue_index,
                                 std::uint32_t dst_queue_index, std::uint32_t double_buffer_index,
                                 const HostBufferedSprite2dGridDrawState &src_state,
                                 DeviceBufferedSprite2dGridDrawState &dst_state) {
        transfer_command_buffer.copyBuffer(src_state.host_material_id_arrays[double_buffer_index],
                                           dst_state.device_material_id_arrays[double_buffer_index]);

        auto transfer_to_graphics_barriers = create_memory_barriers_from_transfer(src_queue_index, dst_queue_index,
                                                                                  double_buffer_index,
                                                                                  dst_state);
        auto dependency_info = vul::DependencyInfo({}, transfer_to_graphics_barriers, {});
        transfer_command_buffer.pipelineBarrier(dependency_info);
    }


    Sprite2dInterpolatedRender::Sprite2dInterpolatedRender(vul::Device &device,
                                                           const vul::DescriptorSetLayout &shared_dset_layout,
                                                           const vul::RenderPass &render_pass) {

        m_pipeline_layout = device.createPipelineLayout(
                shared_dset_layout,
                vul::PushConstantRange::create<Sprite2dInterpolatedPushConstant>((vul::ShaderStageFlagBits::VertexBit |
                                                                                  vul::ShaderStageFlagBits::FragmentBit))).assertValue();
        auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);
        pipelineBuilder.setPrimitiveStateInfo();
        pipelineBuilder.setDynamicViewportStateCount(1, 1);
        pipelineBuilder.setDynamicState({vul::DynamicState::Viewport, vul::DynamicState::Scissor});
        pipelineBuilder.setRasterizationState(
                vul::PipelineRasterizationStateCreateInfo::fromCullMode(vul::CullModeFlagBits::BackBit));
        pipelineBuilder.setMultisampleState();
        pipelineBuilder.setDepthStencilState(
                vul::PipelineDepthStencilStateCreateInfo::fromCompareOp(vul::CompareOp::LessOrEqual));
        pipelineBuilder.setBlendState({vul::PipelineColorBlendAttachmentState::disableBlendRGBA()});
        pipelineBuilder.setRenderPass(render_pass, 0);
        pipelineBuilder.setPipelineLayout(m_pipeline_layout);
        auto vertexShader = device.createShaderModule(
                "spirv/render_sprites_2d_interp_transform.vert.spv").assertValue();
        auto fragmentShader = device.createShaderModule(
                "spirv/render_sprites_2d_interp_transform.frag.spv").assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());

        m_graphics_pipeline = pipelineBuilder.create().assertValue();
    }

    void Sprite2dInterpolatedRender::record_draw_commands(vul::PrimaryCommandBuffer &command_buffer,
                                                          const vul::RenderPass &render_pass,
                                                          const vul::Framebuffer &frame_buffer,
                                                          VkDescriptorSet descriptor_set,
                                                          vul::Extent2D extent,
                                                          const DeviceBufferedSprite2dInterpolatedDrawState &draw_state,
                                                          std::uint32_t buffered_resource_index,
                                                          std::uint32_t sprite_count,
                                                          float interpolated_alpha) {

        auto render_pass_block = command_buffer.beginRenderPass(
                render_pass,
                frame_buffer,
                VkRect2D{{0, 0}, extent},
                {});

        command_buffer.bindPipeline(m_graphics_pipeline);
        command_buffer.bindDescriptorSets(
                vul::PipelineBindPoint::Graphics, m_pipeline_layout,
                descriptor_set);

        Sprite2dInterpolatedPushConstant pushConstant = {
                draw_state.device_prev_arrays_reference[buffered_resource_index].getDeviceAddress(),
                draw_state.device_curr_arrays_reference[buffered_resource_index].getDeviceAddress(),
                draw_state.device_material_id_arrays[buffered_resource_index].getDeviceAddress(),
                interpolated_alpha
        };


        command_buffer.pushConstants(m_pipeline_layout,
                                    vul::ShaderStageFlagBits::VertexBit |
                                    vul::ShaderStageFlagBits::FragmentBit,
                                    pushConstant);
        auto vertex_count = 6 * sprite_count;
        render_pass_block.draw(vertex_count);
    }

    Sprite2dGridRender::Sprite2dGridRender(vul::Device &device, const vul::DescriptorSetLayout &shared_dset_layout,
                                           const vul::RenderPass &render_pass) {
        m_pipeline_layout = device.createPipelineLayout(
                shared_dset_layout,
                vul::PushConstantRange::create<Sprite2dGridPushConstant>((vul::ShaderStageFlagBits::VertexBit |
                                                                                  vul::ShaderStageFlagBits::FragmentBit))).assertValue();
        auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);
        pipelineBuilder.setPrimitiveStateInfo();
        pipelineBuilder.setDynamicViewportStateCount(1, 1);
        pipelineBuilder.setDynamicState({vul::DynamicState::Viewport, vul::DynamicState::Scissor});
        pipelineBuilder.setRasterizationState(
                vul::PipelineRasterizationStateCreateInfo::fromCullMode(vul::CullModeFlagBits::BackBit));
        pipelineBuilder.setMultisampleState();
        pipelineBuilder.setDepthStencilState(
                vul::PipelineDepthStencilStateCreateInfo::fromCompareOp(vul::CompareOp::LessOrEqual));
        pipelineBuilder.setBlendState({vul::PipelineColorBlendAttachmentState::disableBlendRGBA()});
        pipelineBuilder.setRenderPass(render_pass, 0);
        pipelineBuilder.setPipelineLayout(m_pipeline_layout);
        auto vertexShader = device.createShaderModule(
                "spirv/render_grid_sprites.vert.spv").assertValue();
        auto fragmentShader = device.createShaderModule(
                "spirv/render_grid_sprites.frag.spv").assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());

        m_graphics_pipeline = pipelineBuilder.create().assertValue();
    }

    void Sprite2dGridRender::record_draw_commands(vul::PrimaryCommandBuffer &command_buffer,
                                                  const vul::RenderPass &render_pass,
                                                  const vul::Framebuffer &frame_buffer, VkDescriptorSet descriptor_set,
                                                  vul::Extent2D extent,
                                                  const DeviceBufferedSprite2dGridDrawState &draw_state,
                                                  std::uint32_t buffered_resource_index, std::uint32_t sprite_count_x,
                                                  std::uint32_t sprite_count_y, const glm::mat3x2 &affine_matrix) {
        std::uint32_t sprite_count = sprite_count_x * sprite_count_y;
        auto render_pass_block = command_buffer.beginRenderPass(
                render_pass,
                frame_buffer,
                VkRect2D{{0, 0}, extent},
                {});

        command_buffer.bindPipeline(m_graphics_pipeline);
        command_buffer.bindDescriptorSets(
                vul::PipelineBindPoint::Graphics, m_pipeline_layout,
                descriptor_set);

        Sprite2dGridPushConstant pushConstant = {
                draw_state.device_material_id_arrays[buffered_resource_index].getDeviceAddress(),
                sprite_count_x,
                sprite_count_y,
                affine_matrix
        };


        command_buffer.pushConstants(m_pipeline_layout,
                                     vul::ShaderStageFlagBits::VertexBit |
                                     vul::ShaderStageFlagBits::FragmentBit,
                                     pushConstant);
        auto vertex_count = 6 * sprite_count;
        render_pass_block.draw(vertex_count);
    }
}