#ifndef UTILS_SPRITE_2D
#define UTILS_SPRITE_2D
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable

layout(buffer_reference, buffer_reference_align=8, scalar) buffer f32mat3x2_ref {
    f32mat3x2 data;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer uint32_ref {
    uint32_t data;
};

vec2 calc_quad_vertex(uint idx){
    // idx : x   idx : y    x, y
    //   0 : 0     0 : 0    0, 0
    //   1 : 1     1 : 0    1, 0
    //   2 : 1     2 : 1    1, 1
    //   3 : 1     3 : 1    1, 1
    //   4 : 0     4 : 1    0, 1
    //   5 : 0     5 : 0    0, 0
    idx %= 6;
    float x = float(((uint(idx) + 2u) / 3u)%2u);
    float y = float(((uint(idx) + 1u) / 3u)%2u);
    return vec2(x,y);
}

#endif //UTILS_SPRITE_2D
