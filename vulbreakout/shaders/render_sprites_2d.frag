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

layout(set =0, binding = 0) uniform UniformBufferBlock{
    mat4 u_model;
    mat4 u_view;
    mat4 u_proj;
    vec3 u_camera_pos;
    float u_time;
};
layout(set = 0, binding = 1) uniform sampler u_sampler;
layout(set = 0, binding = 2) uniform texture2D u_materials[];

layout (location = 0) in vec2 texcoord;
layout (location = 1) flat in uint material_id;
layout(location = 0) out vec4 out_color;

void main() {
    vec4 color = texture(sampler2D(u_materials[material_id], u_sampler), texcoord);
    if(color.a < 0.5){
        discard;
    }
    out_color = color;
}
