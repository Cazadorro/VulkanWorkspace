#ifndef CHUNK_RENDER_UTILS_GLSL
#define CHUNK_RENDER_UTILS_GLSL
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64   : enable
#include "basicreftypes.glsl"

const uint chunk_width = 32u;
const uint chunk_size = chunk_width*chunk_width*chunk_width;
const uvec3 chunk_dim = uvec3(chunk_width,chunk_width,chunk_width);
const uint full_chunk_bitmask_size_bytes = (chunk_size / 8);
const uint full_chunk_bitmask_size_words = full_chunk_bitmask_size_bytes / 4;

const uint CUBE_FACE_BACK = 0;
const uint CUBE_FACE_LEFT = 1;
const uint CUBE_FACE_FRONT = 2;
const uint CUBE_FACE_RIGHT = 3;
const uint CUBE_FACE_UP = 4;
const uint CUBE_FACE_DOWN = 5;

struct CellIDX{
    uint16_t world_block_idx_x;
    uint16_t world_block_idx_y;
    uint16_t world_block_idx_z;
    uint16_t chunk_idx;
};

uvec2 to_uvec2(const in CellIDX cell_idx){
    uvec2 ret;
    ret.x = uint(cell_idx.chunk_idx) | uint(cell_idx.world_block_idx_x) << 16;
    ret.y = uint(cell_idx.world_block_idx_y) | uint(cell_idx.world_block_idx_z) << 16;
    return ret;
}

uint extract_block_index(uint32_array cumulative_block_offsets, uint cumulative_block_offsets_size, uint global_rle_index){
    uint low = 0u;
    uint high = cumulative_block_offsets_size - 1u;
    uint mid = 0u;

    uint iteration = 0;
    while(low <= high){

        uint mid = (high + low) / 2;
        uint before_arr = mid > 0 ? uint(cumulative_block_offsets.data[mid - 1]) : 0;
        //If cell index is greater than last index in LE, ignore left half
        if( uint(cumulative_block_offsets.data[mid]) <= global_rle_index){
            low = mid + 1u;
        }
        //If x is smaller than RLE range, ignore right half
        else if(before_arr > global_rle_index){
            high = mid - 1u;
        }
        //means x is present at mid
        else{
            return mid;
        }
        if(iteration > 0){
            return 0;
        }
        iteration += 1;
    }
    // We would never reach here, If we reach here, then the element was not present
}

uint get_start_index_at(uint32_array cumulative_block_offsets,  uint block_idx){
    uint start_index = (block_idx > 0) ? cumulative_block_offsets.data[block_idx - 1] : 0;
    return start_index;
}

struct RLEDataBounds{
    uint offset;
    uint size;
};

RLEDataBounds extract_block_index(uint32_array cumulative_block_offsets, uint block_idx){
    uint end_index = cumulative_block_offsets.data[block_idx];
    uint start_index = get_start_index_at(cumulative_block_offsets, block_idx);
    return RLEDataBounds(start_index, end_index - start_index);
}

struct RLEData{
    uint32_array materials;
    uint16_array offsets;
    uint32_array bitmask;
    uint size;
};

RLEData extract_rle_data(
    uint block_idx,
    uint32_array cumulative_block_offsets,
    uint64_t material_data_block_ptr,
    uint32_array bitmasks_ref,
    uint64_t rle_offset_begin){
    RLEDataBounds rle_data_bounds = extract_block_index(cumulative_block_offsets, block_idx);
    uint64_t material_byte_offset = rle_data_bounds.offset * 4;
    uint64_t offset_byte_offset = rle_data_bounds.offset * 2;
    uint32_array materials = uint32_array(uint64_t(material_data_block_ptr + material_byte_offset));
    uint16_array offsets = uint16_array(uint64_t(material_data_block_ptr + offset_byte_offset + rle_offset_begin));
    uint32_array bitmask = uint32_array(uint64_t(bitmasks_ref) + full_chunk_bitmask_size_bytes * block_idx);
    return RLEData(materials, offsets, bitmask, rle_data_bounds.size);
}



    #endif //CHUNK_RENDER_UTILS_GLSL