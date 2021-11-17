//
// Created by Shae Bolt on 9/12/2021.
//

#ifndef VULKANWORKSPACE_LBMCPU_H
#define VULKANWORKSPACE_LBMCPU_H
#include "bitmask.h"
#include <glm/vec2.hpp>
#include <vector>
#include <array>
#include <cstdint>


namespace vul {


    std::uint32_t toLinear(glm::ivec2 pos, std::uint32_t width);

    bool inBounds(glm::uvec2 pos, glm::uvec2 dims);

    bool inBounds(glm::ivec2 pos, glm::ivec2 dims);


    class lbmcpu2D{
    public:
        lbmcpu2D(std::uint32_t width, std::uint32_t height, float delta_time, float tau, float rho);
        void next();
        [[nodiscard]]
        const std::vector<float>& get_current_lbm();
        [[nodiscard]]
        std::vector<float> calc_rho() const;
        [[nodiscard]]
        std::vector<glm::vec2> calc_uxy() const;
        [[nodiscard]]
        std::uint32_t size() const;
        [[nodiscard]]
        const vul::bitmask& get_bitmask() const;
        [[nodiscard]]
        std::uint32_t get_width() const;
        [[nodiscard]]
        std::uint32_t get_height() const;
    private:
        std::uint32_t m_iteration_idx = 0;
        std::uint32_t m_width;
        std::uint32_t m_height;
        float m_delta_time;
        float m_tau;
        float m_rho;
        vul::bitmask m_bitmask;
        std::vector<float> m_lbm_array;
    };
}


#endif //VULKANWORKSPACE_LBMCPU_H
