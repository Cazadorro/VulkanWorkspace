//
// Created by Shae Bolt on 9/3/2022.
//

#ifndef VULKANWORKSPACE_OPENSIMPLEX_H
#define VULKANWORKSPACE_OPENSIMPLEX_H

#include <array>
#include <vector>
#include <cstdint>
#include <tuple>
#include <algorithm>

namespace gul {
    /**
     * K.jpg's OpenSimplex 2, smooth variant ("SuperSimplex")
     *
     */

    /*
     * Noise Evaluators
     */

    /**
     * 2D OpenSimplex2S/SuperSimplex noise, standard lattice orientation.
     */
    float Noise2(std::int64_t seed, double x, double y);

    /**
     * 2D OpenSimplex2S/SuperSimplex noise, with Y pointing down the main diagonal.
     * Might be better for a 2D sandbox style game, where Y is vertical.
     * Probably slightly less optimal for heightmaps or continent maps,
     * unless your map is centered around an equator. It's a slight
     * difference, but the option is here to make it easy.
     */
    float Noise2_ImproveX(std::int64_t seed, double x, double y);

    /**
     * 3D OpenSimplex2S/SuperSimplex noise, with better visual isotropy in (X, Y).
     * Recommended for 3D terrain and time-varied animations.
     * The Z coordinate should always be the "different" coordinate in whatever your use case is.
     * If Y is vertical in world coordinates, call Noise3_ImproveXZ(x, z, Y) or use Noise3_XZBeforeY.
     * If Z is vertical in world coordinates, call Noise3_ImproveXZ(x, y, Z).
     * For a time varied animation, call Noise3_ImproveXY(x, y, T).
     */
    float Noise3_ImproveXY(std::int64_t seed, double x, double y, double z);

    /**
     * 3D OpenSimplex2S/SuperSimplex noise, with better visual isotropy in (X, Z).
     * Recommended for 3D terrain and time-varied animations.
     * The Y coordinate should always be the "different" coordinate in whatever your use case is.
     * If Y is vertical in world coordinates, call Noise3_ImproveXZ(x, Y, z).
     * If Z is vertical in world coordinates, call Noise3_ImproveXZ(x, Z, y) or use Noise3_ImproveXY.
     * For a time varied animation, call Noise3_ImproveXZ(x, T, y) or use Noise3_ImproveXY.
     */
    float Noise3_ImproveXZ(std::int64_t seed, double x, double y, double z);

    /**
     * 3D OpenSimplex2S/SuperSimplex noise, fallback rotation option
     * Use Noise3_ImproveXY or Noise3_ImproveXZ instead, wherever appropriate.
     * They have less diagonal bias. This function's best use is as a fallback.
     */
    float Noise3_Fallback(std::int64_t seed, double x, double y, double z);


    /**
     * 4D SuperSimplex noise, with XYZ oriented like Noise3_ImproveXY
     * and W for an extra degree of freedom. W repeats eventually.
     * Recommended for time-varied animations which texture a 3D object (W=time)
     * in a space where Z is vertical
     */
    float
    Noise4_ImproveXYZ_ImproveXY(std::int64_t seed, double x, double y, double z,
                                double w);

    /**
     * 4D SuperSimplex noise, with XYZ oriented like Noise3_ImproveXZ
     * and W for an extra degree of freedom. W repeats eventually.
     * Recommended for time-varied animations which texture a 3D object (W=time)
     * in a space where Y is vertical
     */
    float
    Noise4_ImproveXYZ_ImproveXZ(std::int64_t seed, double x, double y, double z,
                                double w);

    /**
     * 4D SuperSimplex noise, with XYZ oriented like Noise3_Fallback
     * and W for an extra degree of freedom. W repeats eventually.
     * Recommended for time-varied animations which texture a 3D object (W=time)
     * where there isn't a clear distinction between horizontal and vertical
     */
    float Noise4_ImproveXYZ(std::int64_t seed, double x, double y, double z,
                            double w);

    /**
     * 4D SuperSimplex noise, fallback lattice orientation.
     */
    float
    Noise4_Fallback(std::int64_t seed, double x, double y, double z, double w);

}

#endif //VULKANWORKSPACE_OPENSIMPLEX_H
