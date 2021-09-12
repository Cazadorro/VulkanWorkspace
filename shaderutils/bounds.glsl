#ifndef BOUNDS_GLSL
#define BOUNDS_GLSL

#define IN_BOUNDS_GENERATOR_2(type) \
    bool inBounds(type value, type dim){ \
        return value.x < dim.x && value.y < dim.y && value.x >= 0 && value.y >= 0; \
    }
#define IN_BOUNDS_GENERATOR_3(type) \
    bool inBounds(type value, type dim){ \
        return value.x < dim.x && value.y < dim.y && value.z < dim.z && value.x >= 0 && value.y >= 0 && value.z >= 0; \
    }

IN_BOUNDS_GENERATOR_2(uvec2)
IN_BOUNDS_GENERATOR_2(ivec2)
IN_BOUNDS_GENERATOR_2(vec2)
IN_BOUNDS_GENERATOR_2(dvec2)

IN_BOUNDS_GENERATOR_3(uvec3)
IN_BOUNDS_GENERATOR_3(ivec3)
IN_BOUNDS_GENERATOR_3(vec3)
IN_BOUNDS_GENERATOR_3(dvec3)


#undef IN_BOUNDS_GENERATOR_3
#undef IN_BOUNDS_GENERATOR_2

#endif //BOUNDS_GLSL