#ifndef UTILS_SPRITE_2D
#define UTILS_SPRITE_2D
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable

#include "mathconstants.glsl"

layout(buffer_reference, buffer_reference_align=8, scalar) buffer f32mat3x2_ref {
    f32mat3x2 data;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer uint32_ref {
    uint32_t data;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer f32vec2_ref {
    f32vec2 data;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer float32_ref {
    float32_t data;
};

layout(buffer_reference, buffer_reference_align=8, scalar) buffer transform_soa_ref{
    f32vec2_ref translation_array;
    f32vec2_ref scale_array;
    f32vec2_ref shear_array;
    float32_ref rotation_array;
};

struct Transform2D{
//todo, use f32vec2/f32 or default types?
    vec2 translation;
    vec2 scale;
    vec2 shear;
    float rotation;
};

Transform2D loadTransform2D(const in transform_soa_ref transform_arrays, uint index){
    Transform2D transform;
    transform.translation = transform_arrays.translation_array.data[index];
    transform.scale = transform_arrays.scale_array.data[index];
    transform.shear = transform_arrays.shear_array.data[index];
    transform.rotation = transform_arrays.rotation_array.data[index];
    return transform;
}

float angle_difference(float angle_lhs, float angle_rhs){
    float diff = ( angle_rhs - angle_lhs + eta ) % pi - eta;
    return diff < -eta ? diff + pi : diff;
}

Transform2D interpolate(const in Transform2D lhs, const in Transform2D rhs, float alpha){
    Transform2D interpolated_transform;
    interpolated_transform.translation = lhs.translation * alpha + rhs.translation * (1.0 - alpha);
    interpolated_transform.scale = lhs.scale * alpha + rhs.scale * (1.0 - alpha);
    interpolated_transform.shear = lhs.shear * alpha + rhs.shear * (1.0 - alpha);
    //angles wrap around, to make sure we can still interpolate, we find out the shortest angle difference, and then interpolate to that angle.
    float temp_rotation = rhs.rotation + angle_difference(rhs.rotation, lhs.rotation);
    interpolated_transform.rotation = temp_rotation * alpha + rhs.rotation * (1.0 - alpha);
    return interpolated_transform;
}


vec2 apply(const in Transform2D transform, in vec2 value){
    //order, scale, shear, rotate, translate
    //scale
    value *= transform.scale;
    //shear
    value += value.yx * transform.shear;
    //rotate
    float sintheta = sin(transform.rotation);
    float costheta = cos(transform.rotation);
    value *= costheta;
    value += value.yx * vec2(-sintheta, sintheta);
    //translate
    value += transform.translation;
    return value;
}


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
