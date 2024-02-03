//
// Created by User on 1/16/2024.
//

#ifndef VULKANWORKSPACE_SPRITE_2D_RENDERING_H
#define VULKANWORKSPACE_SPRITE_2D_RENDERING_H


#include <vul/buffer.h>
#include <vul/device.h>
#include <vul/semaphore.h>

#include <vul/pipelinelayout.h>
#include <vul/graphicspipeline.h>

#include <glm/vec2.hpp>
#include <glm/mat3x2.hpp>
#include <vector>
#include <cstdint>
#include <array>


namespace vul {
    class CommandBuffer;

    class BufferMemoryBarrier;

    class PrimaryCommandBuffer;

}
namespace game {

    struct Transform2D {
        glm::vec2 translation;
        glm::vec2 scale;
        glm::vec2 shear;
        float rotation;
    };

    struct TransformSoaReference{
        vul::DeviceAddress translation_array;
        vul::DeviceAddress scale_array;
        vul::DeviceAddress shear_array;
        vul::DeviceAddress rotation_array;
    };

    struct BufferedSprite2dSynchronizationState {
        std::uint32_t swapchain_size;
        //value used to determine which host buffer to use to copy on transfer.
        //next buffer = 0 + 1, current semaphore = 0; t % 2 if !=, upload index, swap buffers to use. otherwise keep updating current buffer.
        vul::TimelineSemaphore upload_finished_semaphore;
        //value used to determine which device buffer can be used to copy to.
        //copy to next buffer when all frames in flight sit on buffer , swapchain size used here
        // swapchain size of 3 means x % 6 = 0->5, if 0,1,2,3 0,3 good, if 1,2,4,5 not done.  if 0 means done at first.
        // signal when specifying using given semiphore.
        vul::TimelineSemaphore frames_using_semaphore;
        BufferedSprite2dSynchronizationState() = default;
        explicit BufferedSprite2dSynchronizationState(vul::Device &device, std::uint32_t swapchain_size, std::uint64_t initial_state = 0u);
    };
    //TODO make buffer amount templated?

    //TODO
    // add new attributes, like:
    // Hue shift,
    // mask id,
    // prev and curr material.
    //
    // (probably need a different type?, or make zero sized?)
    struct HostBufferedSprite2dInterpolatedDrawState {

        static constexpr auto buffer_count = 2;

        [[nodiscard]]
        std::size_t size() const;

        std::array<vul::Buffer, buffer_count> host_curr_translation_arrays;
        std::array<vul::Buffer, buffer_count> host_curr_scale_arrays;
        std::array<vul::Buffer, buffer_count> host_curr_shear_arrays;
        std::array<vul::Buffer, buffer_count> host_curr_rotation_arrays;

        std::array<vul::Buffer, buffer_count> host_prev_translation_arrays;
        std::array<vul::Buffer, buffer_count> host_prev_scale_arrays;
        std::array<vul::Buffer, buffer_count> host_prev_shear_arrays;
        std::array<vul::Buffer, buffer_count> host_prev_rotation_arrays;

        std::array<vul::Buffer, buffer_count> host_material_id_arrays;

        HostBufferedSprite2dInterpolatedDrawState(vul::VmaAllocator &device_allocator,
                                                  std::size_t element_count);

        //TODO use vkCopyMemoryRange or what ever to only copy over updated chunks
        // keep track of updated N element chunks with czdr::bit_vector?
        // normally 1/bitsize to keep track even smaller if done chunkwise (ie 1/(32 * 8) for 256 byes on a float array)
        // maybe do something per "arrays", amount of data would still be small, but not sure if size of data would be good
        // would also need to keep track of each buffers updates, then upload the combined updates for the next buffer as well if it didn't already have the update
        // possible coalesce updates?
        // Would need to profile as well to check.
        void set_curr_transform(std::uint64_t buffer_index, std::size_t idx, const Transform2D &transform);

        void set_prev_transform(std::uint64_t buffer_index, std::size_t idx, const Transform2D &transform);

        void set_material_id(std::uint64_t buffer_index, std::size_t idx, std::uint32_t material_id);
    };

    //TODO sinngle device double buffered, but multiple host double buffered with different copy offsets?  Don't need it yet.

    struct DeviceBufferedSprite2dInterpolatedDrawState {
        static constexpr auto buffer_count = 2;

        [[nodiscard]]
        std::size_t size() const;

        std::array<vul::Buffer, buffer_count> device_curr_translation_arrays;
        std::array<vul::Buffer, buffer_count> device_curr_scale_arrays;
        std::array<vul::Buffer, buffer_count> device_curr_shear_arrays;
        std::array<vul::Buffer, buffer_count> device_curr_rotation_arrays;

        std::array<vul::Buffer, buffer_count> device_prev_translation_arrays;
        std::array<vul::Buffer, buffer_count> device_prev_scale_arrays;
        std::array<vul::Buffer, buffer_count> device_prev_shear_arrays;
        std::array<vul::Buffer, buffer_count> device_prev_rotation_arrays;

        std::array<vul::Buffer, buffer_count> device_material_id_arrays;

        std::array<vul::Buffer, buffer_count> device_prev_arrays_reference;
        std::array<vul::Buffer, buffer_count> device_curr_arrays_reference;

        //Note using default queue, if exists.
        DeviceBufferedSprite2dInterpolatedDrawState(vul::VmaAllocator &device_allocator,
                                                    std::size_t element_count);
    };

    [[nodiscard]]
    std::array<vul::BufferMemoryBarrier, 9> create_memory_barriers_from_transfer(std::uint32_t src_queue_index,
                                                                                 std::uint32_t dst_queue_index,
                                                                                 std::uint32_t double_buffer_index,
                                                                                 const DeviceBufferedSprite2dInterpolatedDrawState &dst_state);

    [[nodiscard]]
    std::array<vul::BufferMemoryBarrier, 9> create_memory_barriers_to_vertex(std::uint32_t src_queue_index,
                                                                             std::uint32_t dst_queue_index,
                                                                             std::uint32_t double_buffer_index,
                                                                             const DeviceBufferedSprite2dInterpolatedDrawState &dst_state);

    void record_transfer_command(vul::CommandBuffer &transfer_command_buffer, std::uint32_t src_queue_index,
                                 std::uint32_t dst_queue_index, std::uint32_t double_buffer_index,
                                 const HostBufferedSprite2dInterpolatedDrawState &src_state,
                                 DeviceBufferedSprite2dInterpolatedDrawState &dst_state);


    struct HostBufferedSprite2dGridDrawState {

        static constexpr auto buffer_count = 2;

        [[nodiscard]]
        std::size_t size() const;

        std::array<vul::Buffer, buffer_count> host_material_id_arrays;

        HostBufferedSprite2dGridDrawState(vul::VmaAllocator &device_allocator,
                                          std::size_t element_count);

        void set_material_id(std::uint64_t buffer_index, std::size_t idx, std::uint32_t material_id);
    };

    //TODO sinngle device double buffered, but multiple host double buffered with different copy offsets?  Don't need it yet.
    struct DeviceBufferedSprite2dGridDrawState {
        static constexpr auto buffer_count = 2;

        [[nodiscard]]
        std::size_t size() const;

        std::array<vul::Buffer, buffer_count> device_material_id_arrays;

        DeviceBufferedSprite2dGridDrawState(vul::VmaAllocator &device_allocator,
                                            std::size_t element_count);
    };


    [[nodiscard]]
    std::array<vul::BufferMemoryBarrier, 1> create_memory_barriers_from_transfer(std::uint32_t src_queue_index,
                                                                                 std::uint32_t dst_queue_index,
                                                                                 std::uint32_t double_buffer_index,
                                                                                 const DeviceBufferedSprite2dGridDrawState &dst_state);

    [[nodiscard]]
    std::array<vul::BufferMemoryBarrier, 1> create_memory_barriers_to_vertex(std::uint32_t src_queue_index,
                                                                             std::uint32_t dst_queue_index,
                                                                             std::uint32_t double_buffer_index,
                                                                             const DeviceBufferedSprite2dGridDrawState &dst_state);

    void record_transfer_command(vul::CommandBuffer &transfer_command_buffer, std::uint32_t src_queue_index,
                                 std::uint32_t dst_queue_index, std::uint32_t double_buffer_index,
                                 const HostBufferedSprite2dGridDrawState &src_state,
                                 DeviceBufferedSprite2dGridDrawState &dst_state);


    struct alignas(8) Sprite2dInterpolatedPushConstant {
        vul::DeviceAddress u_previous_transforms; // structure of arrays of all the previous transforms.
        vul::DeviceAddress u_current_transforms; // structure of arrays of all the current transforms.
        vul::DeviceAddress u_material_ids; // 24;
        float u_position_interpolate_alpha;
    };
    static_assert(sizeof(Sprite2dInterpolatedPushConstant) == 32);

    class Sprite2dInterpolatedRender {
    public:
        Sprite2dInterpolatedRender() = default;
        Sprite2dInterpolatedRender(vul::Device &device, const vul::DescriptorSetLayout &shared_dset_layout,
                                   const vul::RenderPass &render_pass);

        void record_draw_commands(vul::PrimaryCommandBuffer &command_buffer,
                                  const vul::RenderPass &render_pass,
                                  const vul::Framebuffer &frame_buffer,
                                  VkDescriptorSet descriptor_set,
                                  vul::Extent2D extent,
                                  const DeviceBufferedSprite2dInterpolatedDrawState &draw_state,
                                  std::uint32_t buffered_resource_index,
                                  std::uint32_t sprite_count,
                                  float interpolated_alpha);

    private:
        vul::PipelineLayout m_pipeline_layout;
        vul::GraphicsPipeline m_graphics_pipeline;
    };

    struct alignas(8) Sprite2dGridPushConstant {
        vul::DeviceAddress u_complex_material_ids;
        std::uint32_t u_width;
        std::uint32_t u_height;
        glm::mat3x2 u_affine_matrix;
    };
    static_assert(sizeof(Sprite2dGridPushConstant) == 40);

    class Sprite2dGridRender {
    public:
        Sprite2dGridRender() = default;
        Sprite2dGridRender(vul::Device &device, const vul::DescriptorSetLayout &shared_dset_layout,
                                   const vul::RenderPass &render_pass);

        void record_draw_commands(vul::PrimaryCommandBuffer &command_buffer,
                                  const vul::RenderPass &render_pass,
                                  const vul::Framebuffer &frame_buffer,
                                  VkDescriptorSet descriptor_set,
                                  vul::Extent2D extent,
                                  const DeviceBufferedSprite2dGridDrawState &draw_state,
                                  std::uint32_t buffered_resource_index,
                                  std::uint32_t sprite_count_x,
                                  std::uint32_t sprite_count_y,
                                  const glm::mat3x2& affine_matrix);

    private:
        vul::PipelineLayout m_pipeline_layout;
        vul::GraphicsPipeline m_graphics_pipeline;
    };


}
#endif //VULKANWORKSPACE_SPRITE_2D_RENDERING_H
