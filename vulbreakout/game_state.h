//
// Created by User on 1/13/2024.
//

#ifndef VULKANWORKSPACE_GAME_STATE_H
#define VULKANWORKSPACE_GAME_STATE_H


#include "sprite_2d_rendering.h"

#include <vul/temparrayproxy.h>
#include <vul/submitinfobuilder.h>
#include <czdr/unitutil/frequency.h>
#include <czdr/stdutil/control_token.h>

#include <vul/queue.h>
#include <vul/commandpool.h>
#include <vul/commandbuffer.h>
#include <vul/device.h>
#include <vul/vmaallocator.h>
#include <vul/vmaallocation.h>
#include <vul/buffer.h>
#include <vul/buffermemorybarrier.h>
#include <vul/renderpass.h>
#include <vul/descriptorsetlayout.h>

#include <vul/deviceaddress.h>
#include <readerwriterqueue.h>
#include <glm/vec2.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>

namespace game {
    struct CounterTaggedInterpolatedAlphaState {
        double alpha;
        std::uint64_t count;
    };

    [[nodiscard]]
    double get_current_time_double() {
        auto current_time = std::chrono::system_clock::now();
        return std::chrono::duration<double>(current_time.time_since_epoch()).count();
    }

    struct BreakoutInputState {
        bool left_move = false;
        bool right_move = false;
    };
    struct BreakoutPhysicsState {
        glm::vec2 paddle_position;
    };

    struct BreakoutSettings {
        float player_velocity = 0.0;

        BreakoutPhysicsState update_breakout_state(const BreakoutInputState &input_state,
                                                   const BreakoutPhysicsState &previous_state,
                                                   double t, double delta_time) {

            BreakoutPhysicsState physics_state = previous_state;

            auto dt_player_velocity = static_cast<float>(player_velocity * delta_time);
            if (input_state.left_move) {
                physics_state.paddle_position.x += dt_player_velocity;
            }
            if (input_state.right_move) {
                physics_state.paddle_position.x -= dt_player_velocity;
            }

        }
    };


    void upload_game_state(vul::Queue transfer_queue) {

    }


    void game_state_thread_function(czdr::control_token thread_control_token,
                                    vul::Queue *transfer_queue_ptr,
                                    std::uint32_t transfer_to_queue_index,
                                    vul::VmaAllocator *device_allocator_ptr,
                                    vul::CommandPool transfer_command_pool,
                                    DeviceBufferedSprite2dInterpolatedDrawState *device_physics_interp_draw_state_ptr,
                                    DeviceBufferedSprite2dGridDrawState *device_brick_grid_draw_state_ptr,
                                    BufferedSprite2dSynchronizationState *synchronization_state_ptr,
                                    BreakoutPhysicsState init_state,
                                    BreakoutSettings settings,
                                    czdr::hertz<double> update_rate,
                                    moodycamel::ReaderWriterQueue<BreakoutInputState> *input_state_queue,
                                    std::atomic<CounterTaggedInterpolatedAlphaState> *output_alpha) {
        //if this fails, we want the host thread to fail too!
        auto transfer_command_buffer = transfer_command_pool.createPrimaryCommandBuffer().assertValue();
        transfer_command_buffer.setObjectName("Transfer Command Buffer");

        HostBufferedSprite2dInterpolatedDrawState host_physics_interp_draw_state(*device_allocator_ptr, 1);

        //idea from https://gafferongames.com/post/fix_your_timestep/
        double t = 0.0;
//        std::chrono::duration<double, std::ratio<1>> test{1.0 / update_rate.count()};
        double delta_time = (1.0 / update_rate).count();
        auto current_time = get_current_time_double();
        double accumulator = 0.0;
        BreakoutPhysicsState prev_state = init_state;
        BreakoutPhysicsState curr_state = prev_state;
        BreakoutInputState current_input_state;
        std::uint64_t last_buffer_sent_counter = 0;
        std::uint64_t last_buffer_sent_idx = 0;
        std::uint64_t next_buffer_index = 0;
        bool last_buffer_need_upload = true;
        while (thread_control_token.wait(std::memory_order_relaxed)) {
            double new_time = get_current_time_double();
            //I guess this is clamped to make sure that we don't try to iterate too much?
            double frame_time = std::clamp(new_time - current_time, 0.0, 0.25);
            current_time = new_time;
            accumulator += frame_time;
            bool queue_result = input_state_queue->try_dequeue(current_input_state);
            while (accumulator >= delta_time) {
                prev_state = curr_state;
                curr_state = settings.update_breakout_state(current_input_state, curr_state, t, delta_time);
                t += delta_time;
                accumulator -= delta_time;
                host_physics_interp_draw_state.set_prev_transform(next_buffer_index, 0,
                                                                  Transform2D{.translation=prev_state.paddle_position});
                host_physics_interp_draw_state.set_curr_transform(next_buffer_index, 0,
                                                                  Transform2D{.translation=curr_state.paddle_position});

                last_buffer_need_upload = true;
            }
            if (last_buffer_need_upload) {
                auto current_frame_counter_value = synchronization_state_ptr->frames_using_semaphore.getCounterValue().assertValue();
                auto current_upload_counter_value = synchronization_state_ptr->upload_finished_semaphore.getCounterValue().assertValue();
                transfer_command_buffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);

                auto swapchain_size = synchronization_state_ptr->swapchain_size;
                auto last_buffer_uploaded_idx =
                        current_upload_counter_value % host_physics_interp_draw_state.buffer_count;
                bool last_upload_finished = last_buffer_uploaded_idx == last_buffer_sent_idx;
                auto last_upload_frames_used = (current_frame_counter_value % (2 * swapchain_size)) / swapchain_size;
                bool last_upload_frames_used_was_last_upload = last_upload_frames_used == last_buffer_sent_idx;
                bool all_frames_using_last_upload = (current_frame_counter_value % swapchain_size) == 0;
                bool all_frames_in_flight_using_last_upload =
                        last_upload_frames_used_was_last_upload && all_frames_using_last_upload;
                if (all_frames_in_flight_using_last_upload) {
                    last_buffer_need_upload = false;
                    next_buffer_index = (last_buffer_sent_idx + 1) % host_physics_interp_draw_state.buffer_count;
                    last_buffer_sent_counter += 1;
                    record_transfer_command(transfer_command_buffer,
                                            transfer_queue_ptr->getQueueFamilyIndex(),
                                            transfer_to_queue_index, next_buffer_index,
                                            host_physics_interp_draw_state,
                                            *device_physics_interp_draw_state_ptr);
                    transfer_command_buffer.end();

                    std::array<VkSemaphoreSubmitInfoKHR, 1> signal_infos;
                    signal_infos[0] = synchronization_state_ptr->upload_finished_semaphore.createSubmitInfo(
                            current_upload_counter_value + 1,
                            vul::PipelineStageFlagBits2::AllCommandsBit);

                    transfer_queue_ptr->submit(vul::SubmitInfoBuilder()
                                                       //.waitSemaphoreInfos(presentationWaitInfo)
                                                       .commandBufferInfos(transfer_command_buffer.createSubmitInfo())
                                                       .signalSemaphoreInfos(signal_infos)
                                                       .create());

                }
            }


            //alpha value how to use? accumulator may want to use

            double alpha = accumulator / delta_time;

            //we need to make sure everything before this point actually becomes accounted for?
            //we also need to make sure we include what the counter value used was, because if we don't we'll get weird
            //behvavior of states lagging backwards occasionally if alpha is sent before?
            output_alpha->store({alpha, last_buffer_sent_counter}, std::memory_order_release);


            //   State state = currentState * alpha +
            //            previousState * ( 1.0 - alpha );
            //
            //        render( state );
        }
    }


    class BreakoutGameState {
    public:
        BreakoutGameState(vul::VmaAllocator &device_allocator) :
                m_device_allocator_ptr(&device_allocator),
                m_device_physics_interp_draw_state(device_allocator, 1),
                m_device_brick_grid_draw_state(device_allocator, 15 * 8) {

        }

        // Note transfer queue should *not* be used by any other sources while in use here, no synchronization is happening
        void start(vul::Device &device, vul::Queue &transfer_queue,
                   vul::DescriptorSetLayout shared_dset_layout,
                   const vul::RenderPass &render_pass,
                   std::uint32_t transfer_to_queue_index,
                   czdr::hertz<double> update_rate,
                   std::uint32_t swapchain_size) {
            auto transfer_command_pool = device.createCommandPool(
                    transfer_queue.getQueueFamilyIndex(),
                    vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();
            m_paddle_renderer = Sprite2dInterpolatedRender(device, shared_dset_layout, render_pass);
            m_block_grid_renderer = Sprite2dGridRender(device, shared_dset_layout, render_pass);
            m_synchronization_state = BufferedSprite2dSynchronizationState(device, swapchain_size);
            m_game_state_thread = std::thread(game_state_thread_function,
                                              m_thread_control_source.get_token(),
                                              &transfer_queue,
                                              transfer_to_queue_index,
                                              m_device_allocator_ptr,
                                              std::move(transfer_command_pool),
                                              &m_device_physics_interp_draw_state,
                                              &m_device_brick_grid_draw_state,
                                              &m_synchronization_state,
                                              BreakoutPhysicsState{},
                                              BreakoutSettings{500.f},
                                              update_rate,
                                              &m_input_state_queue,
                                              &m_interpolation_alpha_state);
        }

        void enqueue_input_state(const BreakoutInputState &input_state) {
            m_input_state_queue.try_enqueue(input_state);
        }

        void record_draw_commands(vul::PrimaryCommandBuffer &graphics_command_buffer, const vul::RenderPass &render_pass,
                                  const vul::Framebuffer &frame_buffer, VkDescriptorSet descriptor_set,
                                  vul::Extent2D viewport_extent,
                                  std::span<vul::ImageMemoryBarrier> attachment_memory_barriers) {
            graphics_command_buffer.setViewport(vul::Viewport(viewport_extent).get());
            graphics_command_buffer.setScissor(vul::Rect2D(viewport_extent).get());

            std::uint32_t sprite_count_x = 15;
            std::uint32_t sprite_count_y = 8;
            auto affine_mat = glm::identity<glm::mat3x3>();
            affine_mat = glm::translate(affine_mat, glm::vec2(-static_cast<float>(window_width)/2.0,
                                                              -static_cast<float>(window_height)/2.0));
            affine_mat = glm::scale(affine_mat, element_size);

            m_block_grid_renderer.record_draw_commands(
                    graphics_command_buffer,
                    render_pass,
                    frame_buffer,
                    descriptor_set,
                    viewport_extent,
                    m_device_brick_grid_draw_state,
                    0,
                    sprite_count_x,
                    sprite_count_y
                    );
            graphics_command_buffer.bindPipeline(m_pipline);
            //TODO don't actually need to rebind descriptor sets.
            graphics_command_buffer.bindDescriptorSets(
                    vul::PipelineBindPoint::Graphics, renderGridSprites2dPipelineLayout,
                    descriptorSets[swapchainImageIndex]);
            auto current_swapchain_resource_index = last_upload_buffer_sent_idx;
            auto affine_mat = glm::identity<glm::mat3x3>();

            glm::vec2 element_size(
                    window_width / static_cast<float>(break_out_grid_width) * 1.0f,
                    (window_height / 2) / static_cast<float>(break_out_grid_height) * 1.0f
            );

            glm::vec2 breakout_grid_dim(static_cast<float>(break_out_grid_width),
                                        static_cast<float>(break_out_grid_height));

            affine_mat = glm::translate(affine_mat, glm::vec2(-static_cast<float>(window_width) / 2.0,
                                                              -static_cast<float>(window_height) / 2.0));
            affine_mat = glm::scale(affine_mat, element_size);
//                    affine_mat[1][1] *= -1;
            RenderGridSpritesPushConstantBlock pushConstant = {
                    device_grid_complex_id_arrays[current_swapchain_resource_index].getDeviceAddress(),
                    static_cast<std::uint32_t>(break_out_grid_width),
                    static_cast<std::uint32_t>(break_out_grid_height),
                    glm::mat3x2(affine_mat)
            };


            graphics_command_buffer.pushConstants(renderGridSprites2dPipelineLayout,
                                                  vul::ShaderStageFlagBits::VertexBit |
                                                  vul::ShaderStageFlagBits::FragmentBit,
                                                  pushConstant);
            auto vertex_count = break_out_grid_size * 6;
            renderPassBlock.draw(vertex_count);


        }

        void pause() {
            m_thread_control_source.request_pause(std::memory_order_relaxed);
        }

        void resume() {
            m_thread_control_source.request_resume(std::memory_order_relaxed);
        }

        CounterTaggedInterpolatedAlphaState get_interpolation_alpha() {
            //we need to make sure everything before this point actually becomes accounted for?
            return m_interpolation_alpha_state.load(std::memory_order_acquire);
        }

        ~BreakoutGameState() {
            m_thread_control_source.request_stop(std::memory_order_relaxed);
            if (m_game_state_thread.joinable()) {
                m_game_state_thread.join();
            }
        }

    private:
        vul::VmaAllocator *m_device_allocator_ptr;
        DeviceBufferedSprite2dInterpolatedDrawState m_device_physics_interp_draw_state;
        DeviceBufferedSprite2dGridDrawState m_device_brick_grid_draw_state;
        BufferedSprite2dSynchronizationState m_synchronization_state;
        Sprite2dInterpolatedRender m_paddle_renderer;
        Sprite2dGridRender m_block_grid_renderer;
        czdr::control_source m_thread_control_source;
        std::thread m_game_state_thread;
        std::atomic<CounterTaggedInterpolatedAlphaState> m_interpolation_alpha_state;
        moodycamel::ReaderWriterQueue<BreakoutInputState> m_input_state_queue;


    };
}
#endif //VULKANWORKSPACE_GAME_STATE_H
