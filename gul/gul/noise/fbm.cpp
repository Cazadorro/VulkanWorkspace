//
// Created by Shae Bolt on 9/4/2022.
//

#include "gul/noise/fbm.h"

float gul::fbm2d(double x, double y, gul::NoiseFunction2D noise_function,
                 std::size_t octave_count, float persistence, double frequency,
                 double lacunarity, float amplitude) {
    std::int64_t seed = 345233;
    float accumulated_noise = 0.0;
    glm::dvec2 pos{x,y};
    pos *= frequency;
    float persistence_value = 1.0;
    for(std::size_t octave = 0; octave < octave_count; ++octave){
        auto noise = noise_function(seed, pos.x, pos.y);
        noise *= persistence_value;
        persistence_value *= persistence;
        accumulated_noise += noise;
        pos *= lacunarity;
    }
    float scale = 2.0f - persistence_value;
    return (accumulated_noise / scale) * amplitude;
}

float gul::fbm3d(double x, double y,double z, gul::NoiseFunction3D noise_function,
                 std::size_t octave_count, float persistence, double frequency,
                 double lacunarity, float amplitude) {
    std::int64_t seed = 345233;
    float accumulated_noise = 0.0;
    glm::dvec3 pos{x,y,z};
    pos *= frequency;
    float persistence_value = 1.0;
    for(std::size_t octave = 0; octave < octave_count; ++octave){
        auto noise = noise_function(seed, pos.x, pos.y, pos.z);
        noise *= persistence_value;
        persistence_value *= persistence;
        accumulated_noise += noise;
        pos *= lacunarity;
    }
    float scale = 2.0f - persistence_value;
    return (accumulated_noise / scale) * amplitude;
}