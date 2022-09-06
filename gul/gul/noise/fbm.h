//
// Created by Shae Bolt on 9/4/2022.
//

#ifndef VULKANWORKSPACE_FBM_H
#define VULKANWORKSPACE_FBM_H

#include "gul/noise/opensimplex.h"
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <thread>
#include <functional>
#include <cstdint>


namespace gul {

    using NoiseFunction2D = std::function<float(std::int64_t, double, double)>;
    using NoiseFunction3D = std::function<float(std::int64_t, double, double,
                                                double)>;

    float fbm2d(
            double x, double y,
            NoiseFunction2D noise_function,
            std::size_t octave_count,
            float persistence,
            double frequency,
            double lacunarity,
            float amplitude);

    float fbm3d(double x, double y, double z,
                gul::NoiseFunction3D noise_function,
                std::size_t octave_count, float persistence,
                double frequency,
                double lacunarity, float amplitude);
}
#endif //VULKANWORKSPACE_FBM_H
