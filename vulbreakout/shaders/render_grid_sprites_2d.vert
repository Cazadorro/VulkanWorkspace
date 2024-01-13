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
    uint32_ref u_complex_material_ids;
    float u_element_width;
    float u_element_height;
    uint u_width;
    uint u_height;
    mat3x2 u_affine_matrix;
}; //8 * 3 bytes + 12 = 36 total bytes, though I think it's aligned to 8 bytes, so this gets turned into 40 bytes?

layout (location = 0) out vec2 texcoord;
layout (location = 1) out uint complex_material_id;

void main(){
    uint object_index = gl_VertexIndex / 6;
    vec2 quad_vertex = calc_quad_vertex(gl_VertexIndex);
    texcoord = quad_vertex;
    vec2 centered_position = quad_vertex * 2.0f + -1.0f; //now -1 -> 1 space, should we just keep it in zero space?
    uint object_x = object_index % u_width;
    uint object_y = object_index / u_width;

    vec2 object_offset = vec2(float(object_x) * u_element_width,  float(object_y) * u_element_height);
    //Centered, so we offset? probably offset by half a block though, if we start in -1.0 -> 1.0 space.
    object_offset -= vec2(u_width / 2, u_height / 2);
    vec2 offset_position = centered_position + object_offset;
    vec3 position3d = vec3(offset_position, 1.0f);
    mat3x3 affine_matrix = mat3x3(u_affine_matrix);
    affine_matrix[2][2] = 1.0f;
    position3d = affine_matrix *  position3d;
    material_id = u_complex_material_ids[object_index].data;
    vec4 position = vec4(position3d.x, position3d.y, 0.0f, 1.0f);
    //we are relying on our view projection matrix scaling us here, though we should probably be scaling this our selves first with an affine model matrix?
    mat4x4 vp_mat = u_proj * u_view;
    gl_Position = vp_mat * position;
}