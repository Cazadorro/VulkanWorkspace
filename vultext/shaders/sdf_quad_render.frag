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
#include "algorithm.glsl"

layout(set =0, binding = 0) uniform sampler2DArray texSamplerArray;
layout(location = 0) in vec2 texcoord;

layout(location = 1) in uint material_id;

layout(location = 0) out vec4 out_color;


layout(buffer_reference, buffer_reference_align = 1, scalar) buffer SDFColorState{
    vec3 bg_color;
    vec3 fg_color;
    vec2 unit_range;
};

layout(push_constant) uniform PushConstantBlock {
    f32vec2_ref u_offsets; //8 bytes
    f32vec2_ref u_scale;  //16 bytes
    uint32_ref u_material_ids; // 24;
    SDFColorState u_color_state;
    mat4x4 u_mvp; // 64 + 24 bytes;
};


vec3 get_material_color(uint material_id, vec2 tex_coord){
    vec4 color = texture(texSampler, vec3(tex_coord, float(material_id - 1)));
    return color.rgb;
}

float screenPxRange() {
    vec2 screenTexSize = vec2(1.0)/fwidth(texCoord);
    return max(0.5*dot(u_color_state.unit_range, screenTexSize), 1.0);
}
void main(){
    //https://github.com/Chlumsky/msdfgen
    vec3 msd = texture(texSamplerArray, vec3(texcoord, float(material_id))).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    vec3 color = mix(u_color_state.bgColor, u_color_state.fgColor, opacity);
}