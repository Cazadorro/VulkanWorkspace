//
// Created by User on 1/13/2024.
//

#ifndef VULKANWORKSPACE_GAME_STATE_H
#define VULKANWORKSPACE_GAME_STATE_H

#include <readerwriterqueue.h>
#include <czdr/unitutil/frequency.h>
#include <czdr/stdutil/control_token.h>
#include <glm/vec2.hpp>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>

namespace game{
    [[nodiscard]]
    double get_current_time_double(){
        auto current_time = std::chrono::system_clock::now();
        return std::chrono::duration<double>(current_time.time_since_epoch()).count();
    }

    struct BreakoutInputState{
        bool left_move = false;
        bool right_move = false;
    };
    struct BreakoutPhysicsState{
        glm::vec2 paddle_position;
    };
    struct BreakoutSettings{
        float player_velocity = 500.0f;
        BreakoutPhysicsState update_breakout_state(const BreakoutInputState& input_state,
                                                   const BreakoutPhysicsState& previous_state,
                                                   double t, double delta_time){

            BreakoutPhysicsState physics_state = previous_state;

            auto dt_player_velocity = static_cast<float>(player_velocity * delta_time);
            if(input_state.left_move){
                physics_state.paddle_position.x += dt_player_velocity;
            }
            if(input_state.right_move){
                physics_state.paddle_position.x -= dt_player_velocity;
            }

        }
    };



    void game_state_thread_function(czdr::control_token& thread_control_token,
                                    BreakoutPhysicsState initial_state,
                                    BreakoutSettings settings,
                                    czdr::hertz<double> update_rate,
                                    moodycamel::ReaderWriterQueue<BreakoutInputState> &input_state_queue,
                                    std::atomic<double>& output_alpha){
        //idea from https://gafferongames.com/post/fix_your_timestep/
        double t = 0.0;
//        std::chrono::duration<double, std::ratio<1>> test{1.0 / update_rate.count()};
        double delta_time = (1.0 / update_rate).count();
        auto current_time = get_current_time_double();
        double accumulator = 0.0;
        BreakoutPhysicsState previous_state = initial_state;
        BreakoutPhysicsState current_state = previous_state;
        BreakoutInputState current_input_state;
        while(thread_control_token.wait(std::memory_order_relaxed)){
            double new_time = get_current_time_double();
            //I guess this is clamped to make sure that we don't try to iterate too much?
            double frame_time = std::clamp(new_time - current_time, 0.0, 0.25);
            current_time = new_time;
            accumulator += frame_time;
            bool queue_result = input_state_queue.try_dequeue(current_input_state);
            while(accumulator >= delta_time){
                previous_state = current_state;
                current_state = settings.update_breakout_state(current_input_state, current_state, t, delta_time);
                t += delta_time;
                accumulator -= delta_time;
            }
            double alpha = accumulator / delta_time;
            output_alpha.store(alpha, std::memory_order_relaxed);
            //   State state = currentState * alpha +
            //            previousState * ( 1.0 - alpha );
            //
            //        render( state );
        }
    }
    class BreakoutGameState{
    public:
        ~BreakoutGameState(){
            m_thread_control_source.request_stop(std::memory_order_relaxed);
            m_game_state_thread.join();
        }
    private:
        czdr::control_source m_thread_control_source;
        std::atomic<double> m_current_alpha_state;
        std::atomic_flag m_thread_stop_flag;
        std::thread m_game_state_thread;
    };
}
#endif //VULKANWORKSPACE_GAME_STATE_H
