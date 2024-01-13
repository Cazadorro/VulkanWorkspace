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
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require
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

#include "utils_sprite_2d.glsl"
//layout(buffer_reference, buffer_reference_align=8, scalar) buffer f32mat3x2_ref {
//        f32mat3x2 data;
//};
//layout(buffer_reference, buffer_reference_align=4, scalar) buffer uint32_ref {
//    uint32_t data;
//};

layout(set =0, binding = 0) uniform UniformBufferBlock{
   mat4 u_model;
   mat4 u_view;
   mat4 u_proj;
   vec3 u_camera_pos;
   float u_time;
};
layout(set = 0, binding = 1) uniform sampler u_sampler;
layout(set = 0, binding = 2) uniform texture2D u_materials[];

layout(push_constant) uniform PushConstantBlock {
    f32mat3x2_ref u_affine_matrices; // 4 * 2 * 3 = 24 bytes. starts at 0.
    uint32_ref u_material_ids; // 24;
};

layout (location = 0) out vec2 texcoord;
layout (location = 1) out uint material_id;


void main(){
    uint object_index = gl_VertexIndex / 6;
    vec2 quad_vertex = calc_quad_vertex(gl_VertexIndex);
    texcoord = quad_vertex;
    vec3 position3d = vec3(quad_vertex * 2.0f + -1.0f, 1.0f);
    mat3x3 affine_matrix = mat3x3(u_affine_matrices[object_index].data);
    affine_matrix[2][2] = 1.0f;
    position3d = affine_matrix *  position3d;
    material_id = u_material_ids[object_index].data;
    vec4 position = vec4(position3d.x, position3d.y, 0.0f, 1.0f);
    mat4x4 vp_mat = u_proj * u_view;
//    if(quad_vertex == 0){
//        gl_Position =  vec4(1.0, 0.0, 0.0, 1.0);
//    }
//    if(quad_vertex == 1){
//        gl_Position =  vec4(1.0, 0.0, 0.0, 1.0);
//    }
//    if(quad_vertex == 0){
//        gl_Position =  vec4(1.0, 0.0, 0.0, 1.0);
//    }
//    if(quad_vertex == 0){
//        gl_Position =  vec4(1.0, 0.0, 0.0, 1.0);
//    }
    gl_Position = vp_mat * position;
}