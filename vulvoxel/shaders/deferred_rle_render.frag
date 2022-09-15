#version 450
////https://www.khronos.org/blog/vulkan-subgroup-tutorial
//#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_shader_subgroup: enable
//#extension GL_KHR_shader_subgroup_vote: enable
#extension GL_KHR_shader_subgroup_ballot: enable
//#extension GL_KHR_shader_subgroup_arithmetic: enable
//#extension GL_KHR_shader_subgroup_shuffle: enable
//#extension GL_KHR_shader_subgroup_shuffle_relative: enable
//#extension GL_KHR_shader_subgroup_clustered
//#extension GL_KHR_shader_subgroup_quad
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

#include "bitmask.glsl"
#include "chunk_render_utils.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camera_pos;
    float time;
} ubo;

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) flat out uint block_material_id;
layout(location = 1) flat out CellIDX cell_idx;
layout(location = 2) out vec3 block_world_position;
layout(location = 3) out vec3 block_world_normal;
layout(location = 4) out vec2 block_tex_coord;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out uvec2 out_cell_idx;
layout(location = 3) out vec3 world_position;
//https://www.khronos.org/opengl/wiki/Early_Fragment_Test
//layout(early_fragment_tests) in //not sure what exactly this implies?
//https://www.khronos.org/opengl/wiki/Fragment_Shader#Conservative_Depth
//layout (depth_<condition>) out float gl_FragDepth;

layout(push_constant) uniform PushConstantBlock{
//    uint64_t u_material_data_block_ptr;
//    uint32_array u_cumulaive_block_offsets;
//    uint32_array u_bitmask;

    uint u_rle_size;
    uint u_rle_padding;
    uint64_t u_material_data_block_ptr;
    uint32_array u_bitmask;
};
layout (constant_id = 0) const uint64_t RLE_OFFSET_BEGIN = 1024ul*1024ul*8ul;

vec3 get_material_color(uint material_id, vec2 tex_coord){
    vec4 color = texture(texSampler, vec3(tex_coord, float(material_id - 1)));
    return color.rgb;
}

void main() {
    uint32_array u_rle_materials = uint32_array(u_material_data_block_ptr);
    uint16_array u_rle_offsets = uint16_array(u_material_data_block_ptr + RLE_OFFSET_BEGIN);
    vec3 color = get_material_color(block_material_id, block_tex_coord);




    vec3 temp_color = color;
    color += vec3(0.001,0.001,0.001);

    vec3 light_dir_normal = vec3(0.0,1.0,0.0);
    vec3 light_color = vec3(1.0,1.0,1.0);
    float diff = max(dot(block_world_normal, light_dir_normal), 0.0);
    vec3 ambient = color.rgb * 0.2;
    vec3 diffuse = light_color * diff * color.rgb * 0.5;

    vec3 view_dir = normalize(ubo.camera_pos - block_world_position);
    vec3 reflect_dir = reflect(-light_dir_normal, block_world_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0f);
    vec3 specular = light_color * spec * color.rgb;

    vec3 result = ambient + diffuse + specular;
    vec3 ray_normal = normalize(reflect(normalize(block_world_position - ubo.camera_pos), block_world_normal));
    uint cell_position;
    vec3 hit_position;
    vec3 hit_normal;
    vec2 hit_texcoord;
    vec3 offset = vec3(0.0f);

    //    result = vec3(0.2);
    //todo multiple bounce
    //texture array for materials.
    //sample from sky if make it to sky.
    //fix the actual ray tracing?
    vec3 ray_world_position = block_world_position;
    vec3 ray_world_normal = block_world_normal;

    uint iteration = 0;
    uint max_iteration = 4;
    while(iteration < max_iteration && bitmask_intersect(ray_world_position + ray_world_normal * 0.001, ray_normal, offset,cell_position,hit_position, hit_normal, hit_texcoord)){
        uint16_t material_id_index = binary_search_known(u_rle_offsets, u_rle_size, cell_position);
        uint material_id = u_rle_materials.data[uint(material_id_index)];
        result *= get_material_color(material_id, hit_texcoord) * 0.99;

        ray_normal = reflect(ray_normal, hit_normal);
        ray_world_position = hit_position;
        ray_world_normal = hit_normal;
        iteration += 1;
    }

    outColor = vec4(result, 1.0);
}