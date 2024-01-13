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

layout (location = 0) in vec2 in_texcoord;
layout (location = 1) flat in uint in_complex_material_id;
layout(location = 0) out vec4 out_color;
//TODO demonstrate using spec constants host side.
layout (constant_id = 0) const uint BLOCK_ID = 2;
layout (constant_id = 1) const uint BLOCK_SOLID_ID = 3;

const uint MATERIAL_IDS[2] =  uint[2](
    BLOCK_ID,
    BLOCK_SOLID_ID
);

const uint INDIRECT_MATERIAL_ID_MASK =  0xF0000000u;
const uint COLOR_ID_MASK = ~INDIRECT_MATERIAL_ID_MASK;

uint extract_indirect_material_id(uint complex_material_id){
    //gets first 4 bits
    return (complex_material_id & INDIRECT_MATERIAL_ID_MASK) >> 28;
}

uint extract_color_id(uint complex_material_id){
    //gets last 28 bits.
    return (complex_material_id & COLOR_ID_MASK);
}

//TODO specialization constant these values too?
const vec4[6] COLOR_VALUES = vec4[6](
    vec4(0.0f),
    vec4(1.0f),
    vec4(0.2f, 0.6f, 1.0f, 1.0f),
    vec4(0.0f, 0.7f, 0.0f, 1.0f),
    vec4(0.8f, 0.8f, 0.4f, 1.0f),
    vec4(1.0f, 0.5f, 0.0f, 1.0f)
);
//TODO potential issue with coloring with 0 instead of just not rendering it at the initial vertex generation, could be perf issue?
void main() {
    uint indirect_material_id = extract_material_id(in_complex_material_id);
    uint material_id = MATERIAL_IDS[indirect_material_id];
    uint color_id = extract_color_id(in_complex_material_id);
    vec4 color_multiplier = COLOR_VALUES[color_id];

    out_color = texture(sampler2D(u_materials[material_id], u_sampler), in_texcoord) * color_multiplier;
}
