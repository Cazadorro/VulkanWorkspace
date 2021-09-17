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

#include "mathutils.glsl"
#include "basicreftypes.glsl"
const uvec3 chunk_dim = uvec3(32u,32u,32u);
const uint chunk_size = 32u*32u*32u;


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camera_pos;
    float time;
} ubo;


layout(location = 0) flat out uint block_material_id;
layout(location = 1) out vec2 block_tex_coord;
layout(location = 2) out vec3 block_world_normal;
layout(location = 3) out vec3 block_world_position;

layout(push_constant) uniform PushConstantBlock{
    uint u_rle_size;
    uint u_rle_padding;
    uint32_array u_rle_materials;
    uint16_array u_rle_offsets;
    uint32_array u_bitmask;
};

const uint CUBE_FACE_BACK = 0;
const uint CUBE_FACE_LEFT = 1;
const uint CUBE_FACE_FRONT = 2;
const uint CUBE_FACE_RIGHT = 3;
const uint CUBE_FACE_UP = 4;
const uint CUBE_FACE_DOWN = 5;

struct VertCoord{
    vec3 vertex;
    vec3 normal;
    vec2 tex_coord;
};

VertCoord create_cube_vertex(uint cube_vertex_index){
    uint quad_vert_idx = cube_vertex_index % 6;
    uint face_index = cube_vertex_index / 6;

    uint corner_idx = quad_vert_idx > 2 ? (quad_vert_idx - 1) % 4 : quad_vert_idx; //each corner 0 -> 3, assuming counter clockwise.
    // using bool to float conversions to get corner offsets, using normal vulkan clip space convention, 1 is down in y.
    vec3 vertex = vec3(float(corner_idx == 2 || corner_idx == 3), float(corner_idx == 1 || corner_idx == 2), 0.0);
    vec2 tex_coord = vertex.xy;
    vec3 normal = vec3(0.0,0.0,-1.0);
    vertex *= 2;
    vertex -= 1; //renormalizing between -1 and 1.
    switch(face_index){
        case CUBE_FACE_BACK : {
            break;
        }
        case CUBE_FACE_LEFT : {
            //90 degree rotation CW, in xz plane
            vertex = vec3(vertex.z, vertex.y, -vertex.x);
            normal = vec3(normal.z, normal.y, -normal.x);
            break;
        }
        case CUBE_FACE_FRONT : {
            //180 degree rotation CW, in xz plane
            vertex = vec3(-vertex.x, vertex.y, -vertex.z);
            normal = vec3(-normal.x, normal.y, -normal.z);
            break;
        }
        case CUBE_FACE_RIGHT : {
            //-90 degree rotation CCW, in xz plane
            vertex = vec3(-vertex.z, vertex.y, vertex.x);
            normal = vec3(-normal.z, normal.y, normal.x);
            break;
        }
        case CUBE_FACE_UP : {
            //90 degree rotation CW, in xy plane
            vertex = vec3(vertex.x,-vertex.z, vertex.y);
            normal = vec3(normal.x,-normal.z, normal.y);
            break;
        }
        case CUBE_FACE_DOWN : {
            //-90 degree rotation CCW, in xy plane
            vertex = vec3(vertex.x, vertex.z, -vertex.y);
            normal = vec3(normal.x, normal.z, -normal.y);
            break;
        }
    }
    return VertCoord(vertex, normal, tex_coord);
}


void main() {
    uint rle_by_rounds_idx = gl_VertexIndex / 36;
    //a "round" is a full processing of the whole rle for the next "box"
    uint rle_round_idx = rle_by_rounds_idx / u_rle_size;
    uint rle_idx = rle_by_rounds_idx % u_rle_size;
    uint material_id = u_rle_materials.data[rle_idx];
    if(material_id == 0u){
        block_material_id = 33;
        //create degenerate geometry, no need to render empty sky.
        gl_Position = vec4(0.0,0.0,0.0,1.0);
        return;
    }
    block_material_id = material_id;

    uint16_t current_offset = u_rle_offsets.data[rle_idx];
    uint16_t previous_offset = rle_idx > 0 ? u_rle_offsets.data[rle_idx - 1] : 0us;
    uvec3 offset_vec3;
    uvec3 offset_dim3;
    for(uint i = 0; i <= rle_round_idx; ++i){
        uint16_t offset_diff = current_offset - previous_offset;
        if(offset_diff == 0us){ // no box to create.
            gl_Position = vec4(0.0,0.0,0.0,1.0);
            return;
        }
        bool offset_begin_on_3d = previous_offset % (32*32) == 0;
        bool offset_begin_on_2d = previous_offset % (32) == 0;
        bool offset_begin_on_1d = !(offset_begin_on_3d || offset_begin_on_2d);
        //next block ends
        bool offset_end_on_3d = offset_begin_on_3d && offset_diff % (32*32) == 0;
        bool offset_end_on_2d = !(offset_begin_on_1d) && offset_diff % (32) == 0;
        bool offset_end_on_1d = !(offset_end_on_3d || offset_end_on_2d);

        offset_vec3 = uvec3(previous_offset % 32, previous_offset / (32*32), (previous_offset / 32) % 32);


        if(offset_begin_on_3d && offset_diff > 32*32){
            uint16_t plane_count = offset_diff / (32us*32us);
            offset_dim3 = uvec3(32, uint(plane_count), 32);
            previous_offset += plane_count * 32us;
        }else if(offset_begin_on_2d && offset_diff > 32){ //begins on 2d should be true if begins on 3d
            uint16_t line_count = min(offset_diff/32us, 32us - uint16_t(offset_vec3.z));
            offset_dim3 = uvec3(32, 1, uint(line_count));
            previous_offset += 32us * (line_count);
        }else{ //begins on 1d.
            uint16_t cell_count = min(offset_diff, 32us - uint16_t(offset_vec3.x));
            offset_dim3 = uvec3(uint(cell_count), 1, 1);
            previous_offset += cell_count;
        }
    }

    uint cube_vertex_index = gl_VertexIndex % 36;
    VertCoord vert_coord = create_cube_vertex(cube_vertex_index);
    vec3 vertex = vert_coord.vertex;
    block_tex_coord = vert_coord.tex_coord;
// halve, because vertex is between -1 and 1
    vertex += 1;
    vertex *= 0.5;
    vertex *= offset_dim3;

//    vertex *= 1.0/32.0;
//    vertex += offset_vec3 * 1.0/32.0;

    vertex += offset_vec3;
    //vertex *= 1.0/32.0;
    vertex.y *= -1.0;
    vec3 temp_normal = vert_coord.normal;
    temp_normal.y *= -1.0;
    block_world_normal = temp_normal;
    block_world_position = vertex;




    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertex, 1.0);

}
