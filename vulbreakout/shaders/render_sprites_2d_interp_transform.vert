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
    transform_soa_ref u_previous_transforms; // structure of arrays of all the previous transforms.
    transform_soa_ref u_current_transforms; // structure of arrays of all the current transforms.
    uint32_ref u_material_ids; // 24;
    float u_position_interpolate_alpha;
};

layout (location = 0) out vec2 texcoord;
layout (location = 1) out uint material_id;


void main(){
    uint object_index = gl_VertexIndex / 6;
    vec2 quad_vertex = calc_quad_vertex_ccw(gl_VertexIndex);
    texcoord = quad_to_texcoord(quad_vertex);
    vec2 position2d = vec2(quad_vertex * 2.0f + -1.0f);
    Transform2D previous_transform = loadTransform2D(u_previous_transforms, object_index);
    Transform2D current_trasnform = loadTransform2D(u_current_transforms, object_index);
    Transform2D interpolated_transform = interpolate(current_trasnform, previous_transform, u_position_interpolate_alpha);
    position2d = apply(interpolated_transform, position2d);
    material_id = u_material_ids[object_index].data;
    vec4 position = vec4(position3d.x, position3d.y, 0.0f, 1.0f);
    mat4x4 vp_mat = u_proj * u_view;
    position = vp_mat * position;
    position.y *= -1.0;
    gl_Position = position;
//    gl_Position = vp_mat * position;
}