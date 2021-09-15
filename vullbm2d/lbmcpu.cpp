//
// Created by Shae Bolt on 9/12/2021.
//

#include <glm/geometric.hpp>
#include "lbmcpu.h"
#include <vul/vkassert.h>
#include <fmt/core.h>
#include <iostream>


enum LBM_IDX : std::uint32_t{
    LBM_IDX_NW = 0,
    LBM_IDX_NM = 1,
    LBM_IDX_NE = 2,
    LBM_IDX_WW = 3,
    LBM_IDX_MM = 4,
    LBM_IDX_EE = 5,
    LBM_IDX_SW = 6,
    LBM_IDX_SM = 7,
    LBM_IDX_SE = 8,
    LBM_DIR_COUNT = 9,
};
constexpr float cardinal_weight = 1.0/9.0;
constexpr float ordinal_weight = 1.0/36.0;
constexpr float middle_weight = 4.0/9.0;
std::array weights = {
        ordinal_weight, cardinal_weight, ordinal_weight,
        cardinal_weight, middle_weight, cardinal_weight,
        ordinal_weight, cardinal_weight, ordinal_weight
};

std::array velocities = {
        glm::vec2(-1.0f, -1.0f),  glm::vec2(0.0f, -1.0f),  glm::vec2(1.0, -1.0),
        glm::vec2(-1.0f, 0.0f),  glm::vec2(0.0f, 0.0f),  glm::vec2(1.0, 0.0),
        glm::vec2(-1.0f, 1.0f),  glm::vec2(0.0f, 1.0f),  glm::vec2(1.0, 1.0)
};

std::array opposite_idxs = {
        LBM_IDX_SE, LBM_IDX_SM, LBM_IDX_SW,
        LBM_IDX_EE, LBM_IDX_MM, LBM_IDX_WW,
        LBM_IDX_NE, LBM_IDX_NM, LBM_IDX_NW
};


std::array stream_offsets = {
        glm::ivec2(-1, -1), glm::ivec2(0, -1), glm::ivec2(1, -1),
        glm::ivec2(-1, 0), glm::ivec2(0, 0), glm::ivec2(1, 0),
        glm::ivec2(-1, 1), glm::ivec2(0, 1), glm::ivec2(1, 1)
};

std::uint32_t vul::toLinear(glm::ivec2 pos, std::uint32_t width) {
    return pos.y * width + pos.x;
}

bool vul::inBounds(glm::uvec2 pos, glm::uvec2 dims) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < dims.x && pos.y < dims.y;
}


bool vul::inBounds(glm::ivec2 pos, glm::ivec2 dims) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < dims.x && pos.y < dims.y;
}

vul::lbmcpu::lbmcpu(std::uint32_t width, std::uint32_t height, float tau,
                    const std::vector<float> &lbm_array) : u_width(width),
                                                           u_height(height),
                                                           u_tau(tau),
                                                           u_lbm_arrays{
                                                                   lbm_array,
                                                                   lbm_array} {
    VUL_ASSERT(width*height*LBM_DIR_COUNT == lbm_array.size(), "EXPECTED SIZES TO MATCH!");

}

template<typename T>
T pow2(const T& value){
    return value * value;
}
void vul::lbmcpu::compute_next_iteration() {
    for(std::uint32_t x_idx = 0; x_idx < (u_width * u_height); ++x_idx){
        std::uint32_t x = x_idx % u_width;
        std::uint32_t y = x_idx / u_width;
        glm::ivec2 pos =  glm::ivec2(x, y);
        glm::ivec2 dims =  glm::ivec2(u_width, u_height);
        if (!inBounds(pos, dims)){
            return;
        }
        float streamed[LBM_DIR_COUNT] = {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f
        };

        for (std::uint32_t i = 0; i < LBM_DIR_COUNT; ++i){
            streamed[i] = getCheckedInputFor(i, pos, dims);
        }

        //assumes speed of sound is 1/sqrt(3)
        float density = 0;
        glm::vec2 momentum = glm::vec2(0.0, 0.0);
        for (std::uint32_t i = 0; i < LBM_DIR_COUNT; ++i){
            glm::vec2 c_i = velocities[i];
            float value = streamed[i];
            momentum += c_i * value;
            density += value;
        }

        glm::vec2 velocity;
        if(!(density < 0.000001)){
            velocity = momentum / density;
        }else{
            velocity = glm::vec2(0.0);
        }
        float mass_conservation_sum = 0.0;
#define USE_DOUBLE
#define USE_UNROLLED
#ifdef USE_UNROLLED
        float uu = glm::dot(velocity, velocity);

        streamed[4] += -(1.0f/u_tau) *(streamed[4] - ((2 * density / 9.0f) * (2 - 3 * uu)));
        streamed[5] += -(1.0f/u_tau) *(streamed[5] -((density/18) * (2 + 6 * velocity.x + 9 * velocity.x - 3 * uu)));
        streamed[7] += -(1.0f/u_tau) *(streamed[7] -((density/18) * (2 + 6 * velocity.y + 9 * velocity.y - 3 * uu)));
        streamed[3] += -(1.0f/u_tau) *(streamed[3] -((density/18) * (2 - 6 * velocity.x + 9 * velocity.x - 3 * uu)));
        streamed[1] += -(1.0f/u_tau) *(streamed[1] -((density/18) * (2 - 6 * velocity.y + 9 * velocity.y - 3 * uu)));

        streamed[8] += -(1.0f/u_tau) *(streamed[8] -((density/36) * (1 + 3 * (velocity.x + velocity.y) + 9 * (velocity.x *velocity.y) + 3 * uu)));
        streamed[6] += -(1.0f/u_tau) *(streamed[6] -((density/36) * (1 - 3 * (velocity.x - velocity.y) - 9 * (velocity.x *velocity.y) + 3 * uu)));
        streamed[0] += -(1.0f/u_tau) *(streamed[0] -((density/36) * (1 - 3 * (velocity.x + velocity.y) + 9 * (velocity.x *velocity.y) + 3 * uu)));
        streamed[2] += -(1.0f/u_tau) *(streamed[2] -((density/36) * (1 + 3 * (velocity.x - velocity.y) - 9 * (velocity.x *velocity.y) + 3 * uu)));

        for (glm::uint i = 0; i < LBM_DIR_COUNT; ++i) {
            mass_conservation_sum += streamed[i];
        }
        if(mass_conservation_sum < 0) {
            fmt::print("x {}, y {}, expected density {}, density {}\n", x, y, density, mass_conservation_sum);
        }


#elif defined(USE_DOUBLE)
        for (glm::uint i = 0; i < LBM_DIR_COUNT; ++i){
            double w_i = weights[i];
            glm::dvec2 c_i = velocities[i];
            double cu_i = glm::dot(c_i, glm::dvec2(velocity));
            double uu = glm::dot(velocity, velocity);
            //actually a -3/2 instead of +
            double f_eq = w_i * density * (1 + 3 *cu_i + (9.0/2.0) * pow2(cu_i) - (3.0/2.0) * pow2(uu));
//        streamed[i] = max(streamed[i] * (1.0 - (1/u_tau)) + f_eq*(1/u_tau), 0.0);
//            streamed[i] = (streamed[i] * (1.0f - (1/u_tau)) + f_eq*(1/u_tau));
            streamed[i] += static_cast<float>(-(1.0/u_tau) * (streamed[i] - f_eq));
        mass_conservation_sum += streamed[i];
        }
#else
          for (glm::uint i = 0; i < LBM_DIR_COUNT; ++i){
            float w_i = weights[i];
            glm::vec2 c_i = velocities[i];
            float cu_i = glm::dot(c_i, velocity);
            float uu = glm::dot(velocity, velocity);
            //actually a -3/2 instead of +
            float f_eq = w_i * density * (1 + 3 *cu_i + (9.0f/2.0f) * pow2(cu_i) - (3.0f/2.0f) * pow2(uu));
//        streamed[i] = max(streamed[i] * (1.0 - (1/u_tau)) + f_eq*(1/u_tau), 0.0);
//            streamed[i] = (streamed[i] * (1.0f - (1/u_tau)) + f_eq*(1/u_tau));
            streamed[i] += -(1.0f/u_tau) * (streamed[i] - f_eq);
        mass_conservation_sum += streamed[i];
        }
#endif
        if(std::abs(mass_conservation_sum - density) > 0.0000001 && y == 0){
            fmt::print("x {}, y {}, expected density {}, density {}\n", x, y, density, mass_conservation_sum);
        }
        for (std::uint32_t i = 0; i < LBM_DIR_COUNT; ++i){
            setOutput(i, pos, streamed[i]);
        }
    }
    u_iteration_idx += 1;
}

const std::vector<float> &vul::lbmcpu::getInput() const {
    return u_lbm_arrays[u_iteration_idx % 2];
}

std::vector<float> &vul::lbmcpu::getOutput() {
    return u_lbm_arrays[(u_iteration_idx + 1) % 2];
}

float vul::lbmcpu::getInputFor(std::uint32_t frame, glm::ivec2 pos) const {
    std::uint32_t frame_offset = (u_width * u_height) * frame;
    return getInput().at(frame_offset + toLinear(pos, u_width));
}

float
vul::lbmcpu::getBounceInputFor(std::uint32_t frame, glm::ivec2 pos) const {
    std::uint32_t opposite_frame = opposite_idxs[frame];
    std::uint32_t frame_offset = (u_width * u_height) * opposite_frame;
    return getInput().at(frame_offset + toLinear(pos, u_width));
}

float vul::lbmcpu::getCheckedInputFor(std::uint32_t frame, glm::ivec2 pos,
                                      glm::ivec2 dims) const {
    glm::ivec2 offset_pos = pos - stream_offsets[frame];
    if (inBounds(offset_pos, dims) /*&& (!get(u_bitmask, offset_pos, dims))*/) {
        return getInputFor(frame, offset_pos);
    } else {
        return getBounceInputFor(frame, pos);
    }
}

void
vul::lbmcpu::setOutput(std::uint32_t frame, glm::ivec2 pos,
                       float value) {
    std::uint32_t frame_offset = (u_width * u_height) * frame;
    getOutput().at(frame_offset + toLinear(pos, u_width)) = value;
}

const std::vector<float> &vul::lbmcpu::get_current_vector() const {
    return getInput();
}

void vul::lbmcpu::print_current_vector() const {
    auto& curr_vector = get_current_vector();
    for(std::size_t y = 0; y < u_height; ++y){
        for(std::size_t x = 0;x < u_width; ++x){
            std::cout <<"[";
            for(std::size_t frame = 0; frame < LBM_DIR_COUNT; ++frame){
                glm::ivec2 pos(x,y);
                std::cout << curr_vector[frame * u_width * u_height + toLinear(pos, u_width)] << ",";
            }
            std::cout <<"],\n";

        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}
