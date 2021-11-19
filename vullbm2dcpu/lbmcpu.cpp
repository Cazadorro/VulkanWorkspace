//
// Created by Shae Bolt on 9/12/2021.
//

#include <glm/geometric.hpp>
#include "lbmcpu.h"
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/iota.hpp>
#include <glm/glm.hpp>
#include <vul/vkassert.h>
#include <fmt/core.h>
#include <iostream>
#include <random>
#include <glm/ext/scalar_constants.hpp>


static const std::uint32_t dir_count = 9;
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
static const std::array weights = {
        ordinal_weight, cardinal_weight, ordinal_weight,
        cardinal_weight, middle_weight, cardinal_weight,
        ordinal_weight, cardinal_weight, ordinal_weight
};

static const std::array velocities = {
        glm::vec2(-1.0f, -1.0f),  glm::vec2(0.0f, -1.0f),  glm::vec2(1.0, -1.0),
        glm::vec2(-1.0f, 0.0f),  glm::vec2(0.0f, 0.0f),  glm::vec2(1.0, 0.0),
        glm::vec2(-1.0f, 1.0f),  glm::vec2(0.0f, 1.0f),  glm::vec2(1.0, 1.0)
};

static const std::array opposite_idxs = {
        LBM_IDX_SE, LBM_IDX_SM, LBM_IDX_SW,
        LBM_IDX_EE, LBM_IDX_MM, LBM_IDX_WW,
        LBM_IDX_NE, LBM_IDX_NM, LBM_IDX_NW
};


static const std::array stream_offsets = {
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

template<typename T>
T pow2(T t){
    return t*t;
}

vul::lbmcpu2D::lbmcpu2D(std::uint32_t width, std::uint32_t height,
                        float delta_time, float tau, float rho) : m_width(width), m_height(height), m_delta_time(delta_time), m_tau(tau), m_rho(rho), m_bitmask(width*height){
    m_lbm_array = std::vector<LbmType>(dir_count * width * height, 1.0);
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0,1.0);
    for(auto& value : m_lbm_array){
//        value += 0.01f * distribution(generator);
    }
    {
        auto start = static_cast<std::uint32_t>(LBM_IDX::LBM_IDX_EE) * width *
                     height;
        auto end =
                (static_cast<std::uint32_t>(LBM_IDX::LBM_IDX_EE) + 1) * width *
                height;
        for (const auto &idx: ranges::views::iota(start, end)) {
            std::uint32_t linear_idx = (idx - start);
            auto x = linear_idx % m_width;
            auto y = linear_idx / m_height;
            auto &value = m_lbm_array[idx];
            value += 2.0f * (1 + 0.2f * std::cos(
                    2 * glm::pi<LbmType>() * static_cast<LbmType>(x) /
                    static_cast<LbmType>(m_width) * 4));
        }
    }
    std::vector<LbmType> rho_array(width * height, 0.0);
    for(std::uint32_t i = 0; i < dir_count; ++i){
        auto start = i * width * height;
        auto end = (i + 1) * width * height;
        for (const auto &idx: ranges::views::iota(start, end)) {
            std::uint32_t linear_idx = (idx - start);
            rho_array[linear_idx] += m_lbm_array[idx];
        }
    }

    for(auto idx : ranges::views::iota(0u, (m_width * m_height))){
        std::uint32_t linear_idx = (idx % (m_width * m_height));
        auto rho_mod =  m_rho / rho_array[linear_idx];
        for(std::uint32_t dir = 0; dir < dir_count; ++dir){
            m_lbm_array[dir * (m_width * m_height) + idx] *= rho_mod;
        }
    }
    for(auto idx : ranges::views::iota(0u, m_width*m_height)){
        std::int32_t x = idx % m_width;
        std::int32_t y = idx / m_width;
        if(pow2(x - m_width / 4) + pow2(y - m_height / 2) < pow2(m_height / 4)){
            m_bitmask.set(idx);
        }
    }
}

void vul::lbmcpu2D::next() {
    std::vector<LbmType> advected_buffer(dir_count * m_width * m_height, 1.0);
    for(std::uint32_t i = 0; i < dir_count; ++i){
        auto start = i * m_width * m_height;
        auto end = (i + 1) * m_width * m_height;
        std::int32_t offset_x = stream_offsets[i].x;
        std::int32_t offset_y = stream_offsets[i].y;
        glm::ivec2 offset = {offset_x, offset_y};
        for (const auto &idx: ranges::views::iota(start, end)) {
            std::uint32_t linear_idx = (idx - start);
            auto x = linear_idx % m_width;
            auto y = linear_idx / m_width;
            auto advect_value = m_lbm_array[start + linear_idx];
            auto new_x = (x + offset_x) % m_width;
            auto new_y = (y + offset_y) % m_height;
            advected_buffer[start + (new_x + new_y * m_width)] = advect_value;
        }
    }

    for (const auto &idx: ranges::views::iota(0u, size())) {
        LbmType rho = 0;
        glm::vec<2,LbmType> momentum = {0.0f,0.0f};
        for(std::uint32_t dir = 0; dir < dir_count; ++dir){
            auto value = advected_buffer[dir * size() + idx];
            rho += value;
            momentum += value * glm::vec<2,LbmType>(velocities[dir]);
        }
        auto velocity = momentum / rho;
        for (std::uint32_t dir = 0; dir < dir_count; ++dir) {
            LbmType w = weights[dir];
            LbmType cx = velocities[dir].x;
            LbmType cy = velocities[dir].y;
            LbmType ux = velocity.x;
            LbmType uy = velocity.y;
            LbmType f_eq = rho * w * (
                    1 + 3 * (cx * ux + cy * uy) +
                    9 * pow2(cx * ux + cy * uy) / 2 -
                    3 * (pow2(ux) + pow2(uy)) / 2);
            LbmType f = advected_buffer[dir * size() + idx];
            m_lbm_array[dir * size() + idx] =
                    f + (-(m_delta_time / m_tau) * (f - f_eq));
        }

    }
    for (const auto &idx: ranges::views::iota(0u, size())) {
        if (m_bitmask.get(idx)) {
            for (std::uint32_t dir = 0; dir < dir_count; ++dir) {
                m_lbm_array[dir * size() + idx] = advected_buffer[
                        opposite_idxs[dir] * size() + idx];
            }
        }
    }
}

const std::vector<vul::LbmType> &vul::lbmcpu2D::get_current_lbm() {
    return m_lbm_array;
}

std::vector<float> vul::lbmcpu2D::calc_rho() const {

    return std::vector<float>();
}

std::vector<glm::vec2> vul::lbmcpu2D::calc_uxy() const {
    std::vector<glm::vec2> uxy(m_width * m_height);
    for (const auto &idx: ranges::views::iota(0u, size())) {
        LbmType rho = 0;
        glm::vec<2,LbmType> momentum = {0.0f, 0.0f};
        for (std::uint32_t dir = 0; dir < dir_count; ++dir) {
            auto value = m_lbm_array[dir * size() + idx];
            rho += value;
            momentum += value * glm::vec<2,LbmType>(velocities[dir]);
        }
        auto velocity = momentum / rho;
        uxy[idx] = velocity;
    }
    return uxy;
}


std::uint32_t vul::lbmcpu2D::size() const {
    return m_width * m_height;
}

const vul::bitmask &vul::lbmcpu2D::get_bitmask() const {
    return m_bitmask;
}

std::uint32_t vul::lbmcpu2D::get_width() const {
    return m_width;
}

std::uint32_t vul::lbmcpu2D::get_height() const {
    return m_height;
}
