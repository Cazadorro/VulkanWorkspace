#ifndef BITMASK_GLSL
#define BITMASK_GLSL

layout(buffer_reference, buffer_reference_align=4, scalar) buffer uint32_array{
    uint32_t data[];
};

bool get(const in uint32_array bitmask, i32vec2 index, i32vec2 size){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    uint32_t bit_index = linear_index % bit_count;
    return (bitmask.data[u32_index] & (1u << bit_index)) != 0u;
}

void set(in uint32_array bitmask, i32vec2 index, i32vec2 size){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    uint32_t bit_index = linear_index % bit_count;
    bitmask.data[u32_index] |= (1u << bit_index);
}

void set_chunk(in uint32_array bitmask, i32vec2 index, i32vec2 size, uint32_t value){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    bitmask.data[u32_index] |= value;
}

void set_chunk(in uint32_array bitmask, u32vec2 index, u32vec2 size, uint32_t value){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    bitmask.data[u32_index] |= value;
}

void clear_chunk(in uint32_array bitmask, i32vec2 index, i32vec2 size, uint32_t value){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    bitmask.data[u32_index] &= ~value;
}

void clear_chunk(in uint32_array bitmask, u32vec2 index, u32vec2 size, uint32_t value){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    bitmask.data[u32_index] &= ~value;
}

void clear(in uint32_array bitmask, i32vec2 index, i32vec2 size){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    uint32_t bit_index = linear_index % bit_count;
    bitmask.data[u32_index] &= ~(1u << bit_index);
}

bool get(const in uint32_array bitmask, u32vec2 index, u32vec2 size){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    uint32_t bit_index = linear_index % bit_count;
    return (bitmask.data[u32_index] & (1u << bit_index)) != 0u;
}

void set(in uint32_array bitmask, u32vec2 index, u32vec2 size){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    uint32_t bit_index = linear_index % bit_count;
    bitmask.data[u32_index] |= (1u << bit_index);
}

void clear(in uint32_array bitmask, u32vec2 index, u32vec2 size){
    const uint32_t bit_count = 32u;
    uint32_t linear_index = uint(index.y * size.x + index.x);
    uint32_t u32_index = linear_index / bit_count;
    uint32_t bit_index = linear_index % bit_count;
    bitmask.data[u32_index] &= ~(1u << bit_index);
}


#endif //BITMASK_GLSL