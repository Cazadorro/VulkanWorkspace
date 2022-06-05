#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_shader_subgroup_vote: enable
#extension GL_KHR_shader_subgroup_ballot: enable
#extension GL_KHR_shader_subgroup_arithmetic: enable
#extension GL_KHR_shader_subgroup_shuffle: enable
#extension GL_KHR_shader_subgroup_shuffle_relative: enable
#extension GL_KHR_shader_subgroup_clustered: enable
#extension GL_KHR_shader_subgroup_quad: enable
#extension GL_KHR_memory_scope_semantics : enable
#extension GL_EXT_scalar_block_layout: enable
//#extension GL_EXT_buffer_reference : require
//#extension GL_EXT_buffer_reference_uvec2 : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_buffer_reference2 : enable
//for uint64_t etc...
#extension GL_EXT_shader_explicit_arithmetic_types         : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8    : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int16   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float16 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float64 : enable

#include "reftypes.glsl"


layout(push_constant) uniform PushConstantBlock {
    f32vec2_ref u_offsets; //8 bytes
    f32vec2_ref u_scale;  //16 bytes
    uint32_ref u_material_ids; // 24;
    uint64_t u_color_state;
    mat4x4 u_mvp; // 64 + 24 bytes;
};

layout (location = 0) out vec2 texcoord;
layout (location = 1) out uint material_id;

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
void main(){
    uint quad_index = gl_VertexIndex % 6;
    vec2 quad_vertex = calc_quad_vertex(gl_VertexIndex);
    texcoord = quad_vertex;
    vec4 position =  vec4(quad_vertex * 2.0f + -1.0f, 0.0f, 1.0f);
    position.xy *= u_scale[quad_index];
    position.xy += u_offset[quad_index];
    material_id = u_material_ids[quad_index];
    gl_Position = u_mvp * position;
}