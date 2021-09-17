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


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camera_pos;
    float time;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) flat in uint block_material_id;
layout(location = 1) in vec2 block_tex_coord;
layout(location = 2) in vec3 block_world_normal;
layout(location = 3) in vec3 block_world_position;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstantBlock{
    uint u_rle_size;
    uint u_rle_padding;
    uint32_array u_rle_materials;
    uint16_array u_rle_offsets;
    uint32_array u_bitmask;
};

bool bitmask_intersect(vec3 orig, vec3 dir, vec3 block_offset, out uint voxel_index){
    orig = orig.xzy;
    orig.z *= -1.0;
    dir = dir.xzy;
    dir.z *= -1.0;
    vec3 invDir = 1 / dir;
//    bvec3 sign = bvec3(dir.x < 0, dir.y < 0, dir.z < 0);
    vec3 cellDimension = vec3(1,1,1);
    uvec3 resolution = uvec3(32u,32u,32u);
    float tHitBox = 0.0f;
    // initialization step
    ivec3 exit, step, cell;
    vec3 deltaT, nextCrossingT;
//    vec3 bboxoffset = vec3(-4,-4,-4);
    for (uint8_t i_8 = uint8_t(0); i_8 < uint8_t(3); ++i_8) {
        uint i = uint(i_8);
        // convert ray starting point to cell coordinates
        //bbox origin should be 0,0,0 now?

        float rayOrigCell = ((orig[i] + dir[i] * tHitBox) - block_offset[i]);
        cell[i] = int(clamp(int(floor(rayOrigCell / cellDimension[i])), 0, resolution[i] - 1));
        cell[i] = int(floor(rayOrigCell / cellDimension[i]));
        if (dir[i] < 0) {
            deltaT[i] = -cellDimension[i] * invDir[i];
            nextCrossingT[i] = tHitBox + (float(cell[i]) * cellDimension[i] - rayOrigCell) * invDir[i];
            if(i == 234){
                exit[i] = int(resolution[i]);
                step[i] = 1;
            }else{
                exit[i] = -1;
                step[i] = -1;
            }

        }
        else {
            deltaT[i] = cellDimension[i] * invDir[i];
            nextCrossingT[i] = tHitBox + ((float(cell[i]) + 1)  * cellDimension[i] - rayOrigCell) * invDir[i];
//            exit[i] = int(resolution[i]);
//            step[i] = 1;
            if(i == 234){
                exit[i] = -1;
                step[i] = -1;
            }else{
                exit[i] = int(resolution[i]);
                step[i] = 1;
            }

        }
    }

    // Walk through each cell of the grid and test for an intersection if
    // current cell contains geometry
    while (true) {
        if(cell.x >= 32 || cell.y >= 32 || cell.z >= 32 || cell.x < 0 || cell.y < 0 || cell.z < 0){
            return false;
        }
        uint32_t o = cell.z * resolution.x * resolution.y + cell.y * resolution.x + cell.x;
        if (get(u_bitmask, o)) {
            voxel_index = o;
            return true;
        }
        uint8_t k =
            (uint8_t(nextCrossingT[0] < nextCrossingT[1]) << uint8_t(2)) + //y > x
            (uint8_t(nextCrossingT[0] < nextCrossingT[2]) << uint8_t(1)) + //z > x
            (uint8_t(nextCrossingT[1] < nextCrossingT[2])); //z > y
        const uint8_t map[8] = {uint8_t(2),
                                uint8_t(1),
                                uint8_t(2),
                                uint8_t(1),
                                uint8_t(2),
                                uint8_t(2),
                                uint8_t(0),
                                uint8_t(0)};
//        const uint8_t map[8] = {uint8_t(0),
//                                uint8_t(0),
//                                uint8_t(2),
//                                uint8_t(2),
//                                uint8_t(1),
//                                uint8_t(2),
//                                uint8_t(1),
//                                uint8_t(2)};
        uint8_t axis = map[uint(k)];

        //not needed because if we "get" a value at the position, we gaurantee a hit.
//        if (tHit < nextCrossingT[axis]) break;
        cell[uint(axis)] += step[uint(axis)];
        if (cell[uint(axis)] == exit[uint(axis)]){
            break;
        }
        nextCrossingT[uint(axis)] += deltaT[uint(axis)];
    }
    return false;
}


void main() {
    vec3 color;
    if (block_material_id == 1){
        color = vec3(0.1, 1.0, 0.1);
    }else if(block_material_id == 2){
        color = vec3(1.0, 0.1, 0.1);
    }else if(block_material_id == 3){
        color = vec3(0.1, 0.1, 1.0);
    }else if(block_material_id == 4){
        color = vec3(1.0, 0.1, 1.0);
    }else if(block_material_id == 5){
        color = vec3(0.5, 0.5, 1.0);
    }else if(block_material_id == 6){
        color = vec3(0.1, 0.1, 0.5);
    }else if(block_material_id == 7){
        color = vec3(0.1, 0.5, 0.5);
    } else if (block_material_id == 354){
        color = vec3(1.0, 0.5, 0.5);
    }else{
        color = vec3(0.0,0.0,0.0);
    }
    color += vec3(0.001,0.001,0.001);

    vec3 light_dir_normal = vec3(0.0,1.0,0.0);
    vec3 light_color = vec3(1.0,1.0,1.0);
    float diff = max(dot(block_world_normal, light_dir_normal), 0.0);
    vec3 ambient = color.rgb * 0.1;
    vec3 diffuse = light_color * diff * color.rgb * 0.5;

    vec3 view_dir = normalize(ubo.camera_pos - block_world_position);
    vec3 reflect_dir = reflect(-light_dir_normal, block_world_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0f);
    vec3 specular = light_color * spec * color.rgb;

    vec3 result = ambient + diffuse + specular;
    vec3 ray_normal = reflect(normalize(block_world_position - ubo.camera_pos), block_world_normal);
    uint cell_position;
    vec3 offset = vec3(0.0f);
    if(bitmask_intersect(block_world_position + block_world_normal * 0.001,offset, ray_normal,cell_position)){
        result *= 0.1;
    }
    outColor = vec4(result, 1.0);
}