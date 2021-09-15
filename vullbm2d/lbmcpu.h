//
// Created by Shae Bolt on 9/12/2021.
//

#ifndef VULKANWORKSPACE_LBMCPU_H
#define VULKANWORKSPACE_LBMCPU_H
#include <glm/vec2.hpp>
#include <vector>
#include <array>
#include <cstdint>


namespace vul {


    std::uint32_t toLinear(glm::ivec2 pos, std::uint32_t width);

    bool inBounds(glm::uvec2 pos, glm::uvec2 dims);

    bool inBounds(glm::ivec2 pos, glm::ivec2 dims);


    class lbmcpu {
    public:
        lbmcpu(std::uint32_t width, std::uint32_t height, float tau, const std::vector<float>& lbm_array);
        void compute_next_iteration();
        const std::vector<float>& get_current_vector() const;
        void print_current_vector() const;
    private:
        std::uint32_t u_iteration_idx = 0;
        std::uint32_t u_width;
        std::uint32_t u_height;
        float u_tau;
        std::vector<float> u_lbm_arrays[2];
        const std::vector<float>& getInput() const;
        std::vector<float>& getOutput();
        float getInputFor(std::uint32_t frame, glm::ivec2 pos) const;
        float getBounceInputFor(std::uint32_t frame, glm::ivec2 pos) const;
        float getCheckedInputFor(std::uint32_t frame,  glm::ivec2 pos,  glm::ivec2 dims) const;
        void setOutput(std::uint32_t frame, glm::ivec2 pos, float value);
    };
}


#endif //VULKANWORKSPACE_LBMCPU_H
