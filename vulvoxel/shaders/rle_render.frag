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
#include "algorithm.glsl"
#include "mathconstants.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camera_pos;
    float time;
} ubo;
layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) flat in uint v_block_material_id;
layout(location = 1) flat in uint v_block_index;
layout(location = 2) in vec2 v_block_tex_coord;
//TODO this should probably be a bvec or something? bvec4, 3 for position 1 for negative, or some small int value?
layout(location = 3) flat in vec3 v_block_world_normal;
layout(location = 4) in vec3 v_block_world_position;



layout(location = 0) out vec4 out_color;
//https://www.khronos.org/opengl/wiki/Early_Fragment_Test
//layout(early_fragment_tests) in //not sure what exactly this implies?
//https://www.khronos.org/opengl/wiki/Fragment_Shader#Conservative_Depth
//layout (depth_<condition>) out float gl_FragDepth;

layout(push_constant) uniform PushConstantBlock{
    uint64_t u_material_data_block_ptr;
    uint32_array u_cumulaive_block_offsets;
    uint32_array u_bitmasks_ref;
    uint8_array u_voxel_sdf_out;
    uint u_cumulative_block_offsets_size;
//    uint u_rle_size;
//    uint u_rle_padding;
//    uint64_t u_material_data_block_ptr;
//    uint32_array u_bitmask;
};
layout (constant_id = 0) const uint64_t RLE_OFFSET_BEGIN = 1024ul*1024ul*8ul;

bool get_bitmask(uint32_array bitmask, ivec3 cell_xyz){
    return get(bitmask, to_voxel_idx(cell_xyz));
}

bool get_bitmask(uint32_array bitmask, uint cell_index){
    return get(bitmask, cell_index);
}

uint8_t get_sdf(uint cell_index){
    return u_voxel_sdf_out.data[cell_index];
}

uint8_t get_sdf(ivec3 cell_xyz){
    return get_sdf(to_voxel_idx(cell_xyz));
}

struct DebugColor{
    bool is_set;
    vec4 color;
};

struct LightInfo{
    bool hit;
    vec4 color;
};

//https://www.shadertoy.com/view/4dX3zl
bool bitmask_intersect_dda(uint32_array bitmask, vec3 orig, vec3 dir, vec3 block_offset, out uint voxel_index, out vec3 final_crossing_T, out vec3 hit_normal, out vec2 texcoord, out float out_t, out LightInfo light_info, out DebugColor debug_color){
    debug_color.is_set = false;
    light_info.hit = false;
    orig = orig.xzy;
    orig.z *= -1.0;
    dir = dir.xzy;
    dir.z *= -1.0;
    ivec3 cell = ivec3(floor(orig + 0.));
    vec3 inv_dir = 1 / dir;
    if(dir.x == 0){
        inv_dir.x = 1e30;
    }
    if(dir.y == 0){
        inv_dir.y = 1e30;
    }
    if(dir.z == 0){
        inv_dir.z = 1e30;
    }
    vec3 delta_dist = abs(inv_dir);

    ivec3 ray_step = ivec3(sign(dir));
    vec3 side_dist = (sign(dir) * (vec3(cell) - orig) + (sign(dir) * 0.5) + 0.5) * delta_dist;
    uvec3 resolution = uvec3(32u,32u,32u);
    bvec3 mask = bvec3(false,false,false);
    int i = 0;
    ivec3 very_old_cell = cell;
    ivec3 old_cell = cell;
    if(max_v(abs(dir)) == 0){
        debug_color.is_set = true;
        debug_color.color = vec4(1.0,1.0,1.0,1.0);
        return false;
    }
    bool at_start = true;
    while (i < 1000 || true) {
        very_old_cell = old_cell;
        old_cell = cell;
        if(!inside_chunk(cell)){
            return false;
        }
        uint sdf_value = uint(get_sdf(cell));

//        if (get_bitmask(bitmask, cell)){
//            if(sdf_value != 0){
//                debug_color.is_set = true;
//                debug_color.color = vec4(0.0,0.0,1.0,1.0);
//                if(sdf_value == 1){
//                    debug_color.is_set = true;
//                    debug_color.color = vec4(0.0,1.0,1.0,1.0);
//                }
//                return false;
//            }
//            float t = length(vec3(mask) * (side_dist - delta_dist)) / length(dir);
//            voxel_index = to_voxel_idx(cell);
//            orig.z *= -1.0;
//            dir.z *= -1.0;
//            cell.z *= -1;
//            dir = dir.xzy;
//            orig = orig.xzy;
//            mask = mask.xzy;
//            cell = cell.xzy;
//
//            vec3 endpoint;
//            if(mask.x){
//                hit_normal = dot(vec3(1.0,0.0,0.0), dir) < 0.0 ? vec3(1.0,0.0,0.0) : vec3(-1.0,0.0,0.0);
//
//                endpoint = orig + dir * t;
//                texcoord = endpoint.zy;
//            }
//            else if(mask.z){
//                hit_normal =  dot(vec3(0.0,0.0,1.0), dir) < 0.0 ? vec3(0.0,0.0,1.0) : vec3(0.0,0.0,-1.0);
//                endpoint = orig + dir * t;
//                texcoord = endpoint.xy;
//            }
//            else if(mask.y){
//                hit_normal = dot(vec3(0.0,1.0,0.0), dir) < 0.0 ? vec3(0.0,1.0,0.0) : vec3(0.0,-1.0,0.0);
//                endpoint = orig + dir * t;
//                texcoord = endpoint.xz;
//            }
//            final_crossing_T = endpoint;
//
//            return true;
//        }

        ivec3 light_pos = ivec3(15,15,15);
        bool hit_light_pos = cell == light_pos;
        if(hit_light_pos){
            light_info = LightInfo(true, vec4(1.0,0.8,0.8,1.0) * 1.0);
            float t = length(vec3(mask) * (side_dist - delta_dist)) / length(dir);
            voxel_index = to_voxel_idx(cell);
            orig.z *= -1.0;
            dir.z *= -1.0;
            cell.z *= -1;
            dir = dir.xzy;
            orig = orig.xzy;
            mask = mask.xzy;
            cell = cell.xzy;
            out_t = t;
            vec3 endpoint;
            endpoint = orig + dir * t;
            texcoord = endpoint.zy;
            final_crossing_T = endpoint;
            return true;
        }
        if(sdf_value == 0){
            if(at_start){
                return false;
            }
            if(!get_bitmask(bitmask, cell)){
                debug_color.is_set = true;
                debug_color.color = vec4(0.0,1.0,0.0,1.0);
                return false;
            }
            float t = length(vec3(mask) * (side_dist - delta_dist)) / length(dir);
            voxel_index = to_voxel_idx(cell);
            orig.z *= -1.0;
            dir.z *= -1.0;
            cell.z *= -1;
            dir = dir.xzy;
            orig = orig.xzy;
            mask = mask.xzy;
            cell = cell.xzy;

            vec3 endpoint;
            if(mask.x){
                hit_normal = dot(vec3(1.0,0.0,0.0), dir) < 0.0 ? vec3(1.0,0.0,0.0) : vec3(-1.0,0.0,0.0);

                endpoint = orig + dir * t;
                texcoord = endpoint.zy;
            }
            else if(mask.z){
                hit_normal =  dot(vec3(0.0,0.0,1.0), dir) < 0.0 ? vec3(0.0,0.0,1.0) : vec3(0.0,0.0,-1.0);
                endpoint = orig + dir * t;
                texcoord = endpoint.xy;
            }
            else if(mask.y){
                hit_normal = dot(vec3(0.0,1.0,0.0), dir) < 0.0 ? vec3(0.0,1.0,0.0) : vec3(0.0,-1.0,0.0);
                endpoint = orig + dir * t;
                texcoord = endpoint.xz;
            }
            final_crossing_T = endpoint;
            out_t = t;
            return true;
        }else if(sdf_value > 3 && false){
//            debug_color.is_set =true;
//            debug_color.color = vec4(1.0,1.0,1.0,1.0);

            //chebsheyv distance
            vec3 cheb_dir = dir / max_v(abs(dir));
//            float sdf_dist = float(sdf_value - 1) * sqrt(cheb_dir.x*cheb_dir.x + cheb_dir.y*cheb_dir.y + cheb_dir.z*cheb_dir.z);
            cell = ivec3(floor(vec3(cell) + 0.0 + cheb_dir * (float(sdf_value - 2))));
//            cell = ivec3(floor(vec3(cell) + 0.0 + dir * float(sdf_value - 2)));
            side_dist = (sign(dir) * (vec3(cell) - orig) + (sign(dir) * 0.5) + 0.5) * delta_dist;
            side_dist += cheb_dir * (sdf_value - 2);
//            if(!inside_chunk(cell)){
//                debug_color.is_set = true;
//                debug_color.color = vec4(0.0,0.0,1.0,1.0);
//                return false;
//            }
            if(inside_chunk(cell) && get_bitmask(bitmask, cell)){
                debug_color.is_set = true;
                debug_color.color = vec4(0.0,1.0,0.0,1.0);
                return false;
            }
            if(old_cell == cell){
                debug_color.is_set = true;
                debug_color.color = vec4(1.0,1.0,1.0,1.0);
                return false;
            }
        }else if(false){
            for(int j = 0; j < sdf_value; ++j){
                mask = lessThanEqual(side_dist.xyz, min(side_dist.yzx, side_dist.zxy));
                side_dist += vec3(mask) * delta_dist;
                cell += ivec3(vec3(mask)) * ray_step;

                if(!any(mask) || old_cell == cell){
                    debug_color.is_set = true;
                    debug_color.color = vec4(1.0,1.0,0.0,1.0);
                    return false;
                }
            }
        }
        //should always happen otherwise?
         else {
            mask = lessThanEqual(side_dist.xyz, min(side_dist.yzx, side_dist.zxy));
            side_dist += vec3(mask) * delta_dist;
            cell += ivec3(vec3(mask)) * ray_step;

            if(!any(mask) || old_cell == cell){
                debug_color.is_set = true;
                debug_color.color = vec4(1.0,1.0,0.0,1.0);
                return false;
            }
        }
        if(very_old_cell == cell){
            debug_color.is_set = true;
            debug_color.color = vec4(1.0,1.0,1.0,1.0);
            return false;
        }
        at_start = false;
        i += 1;
    }
    debug_color.is_set = true;
    debug_color.color = vec4(1.0,0.0,1.0,1.0);
    return false;

}

bool bitmask_intersect(uint32_array bitmask, vec3 orig, vec3 dir, vec3 block_offset, out uint voxel_index, out vec3 final_crossing_T, out vec3 hit_normal, out vec2 texcoord, out DebugColor debug_color){
    debug_color.is_set = false;
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
        cell[i] = int(clamp(int(floor(rayOrigCell / cellDimension[i])), 0, int((resolution[i] - 1))));
        cell[i] = int(floor(rayOrigCell / cellDimension[i]));
        if (dir[i] < 0) {
            deltaT[i] = -cellDimension[i] * invDir[i];
            nextCrossingT[i] = tHitBox + (float(cell[i]) * cellDimension[i] - rayOrigCell) * invDir[i];
            exit[i] = -1;
            step[i] = -1;
        }
        else {
            deltaT[i] = cellDimension[i] * invDir[i];
            nextCrossingT[i] = tHitBox + ((float(cell[i]) + 1)  * cellDimension[i] - rayOrigCell) * invDir[i];
            exit[i] = int(resolution[i]);
            step[i] = 1;
        }
    }

    // Walk through each cell of the grid and test for an intersection if
    // current cell contains geometry
    uint previous_axis = 0;
    float last_t = 0;
    while (true) {
        if(cell.x >= 32 || cell.y >= 32 || cell.z >= 32 || cell.x < 0 || cell.y < 0 || cell.z < 0){
            return false;
        }
        uint32_t o = cell.z * resolution.x * resolution.y + cell.y * resolution.x + cell.x;
        if (get_bitmask(bitmask, o)) {
            voxel_index = o;
            float t = last_t;
            orig.z *= -1.0;
            dir.z *= -1.0;
            dir = dir.xzy;
            orig = orig.xzy;
            vec3 endpoint = orig + dir * t;
            vec3 fixed_endpoint = endpoint.xzy;
            fixed_endpoint =endpoint;
            final_crossing_T = fixed_endpoint;
            if(previous_axis == 0){
                texcoord = fixed_endpoint.zy;
                //if facing oposite direction, set normal to the opposite direction.
                hit_normal = dot(vec3(1.0,0.0,0.0), dir) < 0.0 ? vec3(1.0,0.0,0.0) : vec3(-1.0,0.0,0.0);
            }
            else if(previous_axis == 1){
//                fixed_endpoint.y *= -1.0;
                texcoord = fixed_endpoint.xy; //xy?
                hit_normal = dot(vec3(0.0,0.0,1.0), dir) < 0.0 ? vec3(0.0,0.0,1.0) : vec3(0.0,0.0,-1.0);
            }
            else if(previous_axis == 2){
                texcoord = fixed_endpoint.xz; //xz?
                hit_normal = dot(vec3(0.0,1.0,0.0), dir) < 0.0 ? vec3(0.0,1.0,0.0) : vec3(0.0,-1.0,0.0);
            }
            return true;
        }

        uint sdf_value = uint(get_sdf(o));

        if(sdf_value > 2 && false){
            debug_color.is_set =true;
            debug_color.color = vec4(1.0,1.0,1.0,1.0);
            vec3 rayOrigCell = vec3(cell) + dir * (sdf_value - 1);
            for (int i = 0; i < 3; ++i) {
                cell[i] = int(floor(rayOrigCell[i]));
                if (dir[i] < 0) {
                    nextCrossingT[i] = (float(cell[i]) * cellDimension[i] - rayOrigCell[i]) * invDir[i];
                }
                else {
                    nextCrossingT[i] = ((float(cell[i]) + 1)  * cellDimension[i] - rayOrigCell[i]) * invDir[i];
                }
            }
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

        uint8_t axis = map[uint(k)];

        //not needed because if we "get" a value at the position, we gaurantee a hit.
//        if (tHit < nextCrossingT[axis]) break;
        cell[uint(axis)] += step[uint(axis)];
        if (cell[uint(axis)] == exit[uint(axis)]){
            break;
        }
        last_t = nextCrossingT[uint(axis)];
        nextCrossingT[uint(axis)] += deltaT[uint(axis)];
        previous_axis = uint(axis);
    }
    return false;
}

vec3 get_material_color(uint material_id, vec2 tex_coord){
    vec4 color = texture(texSampler, vec3(tex_coord, float(material_id - 1)));
    return color.rgb;
}

uint16_t binary_search_known(RLEData rle_data,uint cell_index){

    uint low = 0u;
    uint high = rle_data.size - 1u;
    uint mid = 0u;


    while(low <= high){
        uint mid = (high + low) / 2;
        uint before_arr = mid > 0 ? uint(rle_data.materials.data[mid - 1]) : 0;
        //If cell index is greater than last index in LE, ignore left half
        if( uint(rle_data.materials.data[mid]) <= cell_index){
            low = mid + 1u;
        }
        //If x is smaller than RLE range, ignore right half
        else if(before_arr > cell_index){
            high = mid - 1u;
        }
        //means x is present at mid
        else{
            return uint16_t(mid);
        }
    }
    // We would never reach here, If we reach here, then the element was not present
}
uint get_material_id(RLEData rle_data, uint cell_index){
    uint16_t material_id_index = binary_search_known(rle_data, cell_index);
    uint material_id = rle_data.materials.data[uint(material_id_index)];
    return material_id;
}

vec2 round_func(vec2 xy){
//    return xy;
    return floor(xy) + 0.5;
    return round(xy);
}

float light_attenuation(float dist, float rad){
    float constant = 1.0;
    float linear = 4.5 / rad;
    float quadratic = 75.0 / (rad * rad);
    float attenuation = constant + linear * dist + quadratic * dist * dist;
    return 1.0 / attenuation;
    return clamp(1.0 - ((dist * dist) / (rad * rad)), 0.0, 1.0);
}

void main() {

    RLEData rle_data = extract_rle_data(v_block_index, u_cumulaive_block_offsets, u_material_data_block_ptr, u_bitmasks_ref, RLE_OFFSET_BEGIN);

    vec3 color = get_material_color(v_block_material_id, v_block_tex_coord);

    if(v_block_material_id == 5){
        out_color = vec4(color, 1.0);
        return;
    }


    vec3 temp_color = color;
    color += vec3(0.001,0.001,0.001);

    vec3 light_dir_normal = vec3(0.0,1.0,0.0);
    vec3 light_color = vec3(1.0,1.0,1.0);
    float diff = max(dot(v_block_world_normal, light_dir_normal), 0.0);
    vec3 ambient = color.rgb * 0.2;
    vec3 diffuse = light_color * diff * color.rgb * 0.5;

    vec3 view_dir = normalize(ubo.camera_pos - v_block_world_position);
    vec3 reflect_dir = reflect(-light_dir_normal, v_block_world_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0f);
    vec3 specular = light_color * spec * color.rgb;

    vec3 result = ambient + diffuse + specular;
    result = ambient;
    result = color;

    vec3 ray_world_position = v_block_world_position;

    if(true){
        if (abs(v_block_world_normal) == vec3(1.0, 0.0, 0.0)){
            ray_world_position.yz = vec2(vec2(round_func((ray_world_position.yz * 16) + 0.0))) / 16.0;
            ray_world_position.x = round(ray_world_position.x);
        }
        if (abs(v_block_world_normal) == vec3(0.0, 1.0, 0.0)){
            ray_world_position.xz = vec2(vec2(round_func((ray_world_position.xz * 16) + 0.0))) / 16.0;
            ray_world_position.y = round(ray_world_position.y);
        }
        if (abs(v_block_world_normal) == vec3(0.0, 0.0, 1.0)){
            ray_world_position.xy = vec2(vec2(round_func((ray_world_position.xy * 16) + 0.0))) / 16.0;
            ray_world_position.z = round(ray_world_position.z);
        }
    }

    vec3 ray_normal = normalize(reflect(normalize(ray_world_position - ubo.camera_pos), v_block_world_normal));
    uint cell_position;
    vec3 hit_position;
    vec3 hit_normal;
    vec2 hit_texcoord;

    uint temp_cell_position;
    vec3 temp_hit_position;
    vec3 temp_hit_normal;
    vec2 temp_hit_texcoord;


    vec3 offset = vec3(0.0f);

//    result = vec3(0.2);
    //todo multiple bounce
    //texture array for materials.
    //sample from sky if make it to sky.
    //fix the actual ray tracing?
//    if(v_block_world_normal.x != 0){
//        out_color = vec4(vec3(0.0, 1.0, 0.0), 1.0);
//    }
//    else if((v_block_world_normal) == vec3(0.0,1.0,0.0)){
//        out_color = vec4(vec3(1.0, 1.0, 0.0), 1.0);
//    }
//    else if((v_block_world_normal) == vec3(0.0,0.0,1.0)){
//        out_color = vec4(vec3(0.0, 1.0, 1.0), 1.0);
//    }else{
//        out_color = vec4(vec3(0.0),1.0);
//    }
//
//    return;
    vec3 ray_world_normal = v_block_world_normal;

    uint iteration = 0;

    uint max_iteration = 2048 / 8;
    max_iteration = 128;
    DebugColor debug_color;
    LightInfo light_info;
    debug_color.is_set = false;


    vec3 init_ray_world_normal = ray_world_normal;
    vec3 initial_position = ray_world_position;
    vec3 init_result = result;
    vec3 init_ray_normal = ray_normal;

    vec3 light_pos = vec3(15.5,-15.5,15.5);
    float light_radius = 128;
    vec3 light_energy = vec3(0);
    float total_distance = distance(ray_world_position, ubo.camera_pos);
    float intersect_t = 0;
    if(bitmask_intersect_dda(rle_data.bitmask, ray_world_position + ray_world_normal * 0.00001, normalize(light_pos - ray_world_position), offset,temp_cell_position,hit_position, hit_normal, hit_texcoord,intersect_t, light_info, debug_color)){
        if(light_info.hit){
            float pos_light_dist = distance(light_pos,ray_world_position);

            light_energy += result * light_info.color.rgb * light_attenuation(pos_light_dist + total_distance, light_radius);
        }
    }

    while(iteration < max_iteration && bitmask_intersect_dda(rle_data.bitmask, ray_world_position + ray_world_normal * 0.001, ray_normal, offset,cell_position,hit_position, hit_normal, hit_texcoord, intersect_t,light_info, debug_color)){
        if(light_info.hit){
            ray_world_position = hit_position;
            ray_world_normal = vec3(0.0,0.0,0.0);
            total_distance += intersect_t;
            result *= light_info.color.rgb;
        }else{
            uint material_id = get_material_id(rle_data, cell_position);
            result *= get_material_color(material_id, hit_texcoord) * 0.99;
            total_distance += intersect_t;
            if(bitmask_intersect_dda(rle_data.bitmask, ray_world_position + ray_world_normal * 0.001, normalize(light_pos - ray_world_position), offset,temp_cell_position,temp_hit_position, temp_hit_normal, temp_hit_texcoord, intersect_t,light_info, debug_color)){
                if(light_info.hit){
                    float pos_light_dist = distance(light_pos,ray_world_position);
                    light_energy += result * light_info.color.rgb * light_attenuation(pos_light_dist + total_distance, light_radius);
                }
            }
            ray_normal = reflect(ray_normal, hit_normal);
            ray_world_position = hit_position;
            ray_world_normal = hit_normal;

        }

        iteration += 1;
    }
    vec3 other_result = result;
    vec3 new_result = color * 0.2 + light_energy;
//    new_result = light_energy;
    float alt_angle_spred = 0.01;
    vec3 alt_normal_array[4] = vec3[4](
        normalize(vec3( alt_angle_spred, 1.0, alt_angle_spred)),
        normalize(vec3( alt_angle_spred, 1.0,-alt_angle_spred)),
        normalize(vec3(-alt_angle_spred, 1.0, alt_angle_spred)),
        normalize(vec3(-alt_angle_spred, 1.0,-alt_angle_spred))
    );
    vec2 alt_normal_offset[4] = vec2[4](
        vec2(0.5, 0.5) / 16,
        vec2(0.5, -0.5) / 16,
        vec2(-0.5, 0.5) / 16,
        vec2(-0.5, -0.5) / 16
    );
    int max_normal_i = 4;

    bool enable_multi_angle = false;
    if(enable_multi_angle){
        for(int i = 0; i < max_normal_i; ++i){
            ray_world_position = initial_position;
            vec3 alt_normal = alt_normal_array[i];
            if(v_block_world_normal.y != 0){
                //TODO need to use this to spread the points out on the texel.
                if(v_block_world_normal.y < 0){
                    alt_normal.y *= -1.0;
                }
                ray_world_position.xz += alt_normal_offset[i];
            }
            else if(v_block_world_normal.x != 0){
                if(v_block_world_normal.x < 0){
                    alt_normal.y *= -1.0;
                }
                alt_normal.xyz = alt_normal.yxz;
                ray_world_position.yz += alt_normal_offset[i];
            }
            else if(v_block_world_normal.z != 0){
                if(v_block_world_normal.z < 0){
                    alt_normal.y *= -1.0;
                }
                alt_normal.xyz = alt_normal.xzy;
                ray_world_position.xy += alt_normal_offset[i];
            }
            ray_world_normal = init_ray_world_normal;
            result = init_result;
            intersect_t = 0;
            ray_normal = normalize(reflect(normalize(ray_world_position - ubo.camera_pos), alt_normal));
            iteration = 0;
            while(iteration < max_iteration && bitmask_intersect_dda(rle_data.bitmask, ray_world_position + ray_world_normal * 0.001, ray_normal, offset,cell_position,hit_position, hit_normal, hit_texcoord, intersect_t, light_info, debug_color)){
                if(light_info.hit){
                    ray_world_position = hit_position;
                    ray_world_normal = vec3(0.0,0.0,0.0);
                    result *= light_info.color.rgb * 3;
                }else{
                    uint material_id = get_material_id(rle_data, cell_position);
                    result *= get_material_color(material_id, hit_texcoord) * 0.99;
                    ray_normal = reflect(ray_normal, hit_normal);
                    ray_world_position = hit_position;
                    ray_world_normal = hit_normal;
                }
                iteration += 1;
            }
            new_result += result;
            //        new_result = result;
        }
        new_result /= (max_normal_i + 1);
    }
    if(false){
        for(int i = 0; i < max_normal_i; ++i){
            ray_world_position = initial_position;
            vec3 alt_normal = alt_normal_array[i];
            ray_world_normal = init_ray_world_normal;
            result = init_result;

            ray_normal = normalize(reflect(normalize(ray_world_position - ubo.camera_pos), alt_normal));
            iteration = 0;
            while(iteration < max_iteration && bitmask_intersect_dda(rle_data.bitmask, ray_world_position + ray_world_normal * 0.001, ray_normal, offset,cell_position, hit_position, hit_normal, hit_texcoord, intersect_t, light_info, debug_color)){
                if(light_info.hit){
                    ray_world_position = hit_position;
                    ray_world_normal = vec3(0.0,0.0,0.0);
                    result *= light_info.color.rgb * 3;
                }else{
                    uint material_id = get_material_id(rle_data, cell_position);
                    result *= get_material_color(material_id, hit_texcoord) * 0.99;
                    ray_normal = reflect(ray_normal, hit_normal);
                    ray_world_position = hit_position;
                    ray_world_normal = hit_normal;
                }
                iteration += 1;
            }
            new_result += result;
            //        new_result = result;
        }
        new_result /= (max_normal_i + 1);
    }

    if(debug_color.is_set){
        out_color = debug_color.color;
    }else{
        out_color = vec4(new_result, 1.0);
    }


}