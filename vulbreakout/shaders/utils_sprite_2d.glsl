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

vec2 calc_quad_vertex_ccw(uint idx){
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

vec2 calc_quad_vertex_cw(uint idx){
    // idx : x   idx : y    x, y
    //   0 : 0     0 : 0    0, 0
    //   1 : 0     1 : 1    0, 1
    //   2 : 1     2 : 1    1, 1
    //   3 : 1     3 : 1    1, 1
    //   4 : 1     4 : 0    1, 0
    //   5 : 0     5 : 0    0, 0
    idx %= 6;
    float y = float(((uint(idx) + 2u) / 3u)%2u);
    float x = float(((uint(idx) + 1u) / 3u)%2u);
    return vec2(x,y);
}


vec2 calc_quad_vertex_clip_ccw(uint idx){
    //we use cw underneath because vulkan's clip space (-1.0 up, 1.0 down y) will make the order the opposite.
    return calc_quad_vertex_cw(idx);
}

vec2 calc_quad_vertex_clip_cw(uint idx){
    //we use ccw underneath because vulkan's clip space (-1.0 up, 1.0 down y) will make the order the opposite.
    return calc_quad_vertex_ccw(idx);
}

vec2 quad_to_texcoord(vec2 quad_coord){
    //quad is 0 bottom, texcoords are 0 top (because that's how it's layed out in memory).
    //so we reverse the y coordinate.
    return vec2(quad_coord.x, 1.0 - quad_coord.y);
}
#endif //UTILS_SPRITE_2D
