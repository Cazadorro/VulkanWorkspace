#ifndef BASIC_PTR_TYPES_GLSL
#define BASIC_PTR_TYPES_GLSL


layout(buffer_reference, buffer_reference_align=4, scalar) buffer float32_ptr{
    float32 value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer f32vec2_ptr{
    f32vec2 value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer f32vec3_ptr{
    f32vec3 value;
};
layout(buffer_reference, buffer_reference_align=16, scalar) buffer f32vec4_ptr{
    f32vec4 value;
};

layout(buffer_reference, buffer_reference_align=8, scalar) buffer float64_ptr{
    float64_t value;
};
layout(buffer_reference, buffer_reference_align=16, scalar) buffer f64vec2_ptr{
    f64vec2 value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer f64vec3_ptr{
    f64vec3 value;
};
layout(buffer_reference, buffer_reference_align=32, scalar) buffer f64vec4_ptr{
    f64vec4 value;
};

layout(buffer_reference, buffer_reference_align=1, scalar) buffer uint8_ptr{
    uint8_t value;
};
layout(buffer_reference, buffer_reference_align=2, scalar) buffer u8vec2_ptr{
    u8vec2 value;
};
layout(buffer_reference, buffer_reference_align=1, scalar) buffer u8vec3_ptr{
    u8vec3 value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer u8vec4_ptr{
    u8vec4 value;
};

layout(buffer_reference, buffer_reference_align=2, scalar) buffer uint16_ptr{
    uint16_t value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer u16vec2_ptr{
    u16vec2 value;
};
layout(buffer_reference, buffer_reference_align=2, scalar) buffer u16vec3_ptr{
    u16vec3 value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer u16vec4_ptr{
    u16vec4 value;
};

layout(buffer_reference, buffer_reference_align=4, scalar) buffer uint32_ptr{
    uint32_t value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer u32vec2_ptr{
    u32vec2 value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer u32vec3_ptr{
    u32vec3 value;
};
layout(buffer_reference, buffer_reference_align=16, scalar) buffer u32vec4_ptr{
    u32vec4 value;
};

layout(buffer_reference, buffer_reference_align=8, scalar) buffer uint64_ptr{
    uint64_t value;
};
layout(buffer_reference, buffer_reference_align=16, scalar) buffer u64vec2_ptr{
    u64vec2 value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer u64vec3_ptr{
    u64vec3 value;
};
layout(buffer_reference, buffer_reference_align=32, scalar) buffer u64vec4_ptr{
    u64vec4 value;
};

layout(buffer_reference, buffer_reference_align=1, scalar) buffer int8_ptr{
    int8_t value;
};
layout(buffer_reference, buffer_reference_align=2, scalar) buffer i8vec2_ptr{
    i8vec2 value;
};
layout(buffer_reference, buffer_reference_align=1, scalar) buffer i8vec3_ptr{
    i8vec3 value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer i8vec4_ptr{
    i8vec4 value;
};


layout(buffer_reference, buffer_reference_align=2, scalar) buffer int16_ptr{
    int16_t value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer i16vec2_ptr{
    i16vec2 value;
};
layout(buffer_reference, buffer_reference_align=2, scalar) buffer i16vec3_ptr{
    i16vec3 value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer i16vec4_ptr{
    i16vec4 value;
};

layout(buffer_reference, buffer_reference_align=4, scalar) buffer int32_ptr{
    int32_t value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer i32vec2_ptr{
    i32vec2 value;
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer i32vec3_ptr{
    i32vec3 value;
};
layout(buffer_reference, buffer_reference_align=16, scalar) buffer i32vec4_ptr{
    i32vec4 value;
};

layout(buffer_reference, buffer_reference_align=8, scalar) buffer int64_ptr{
    int64_t value;
};
layout(buffer_reference, buffer_reference_align=16, scalar) buffer i64vec2_ptr{
    i64vec2 value;
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer i64vec3_ptr{
    i64vec3 value;
};
layout(buffer_reference, buffer_reference_align=32, scalar) buffer i64vec4_ptr{
    i64vec4 value;
};

#endif //BASIC_PTR_TYPES_GLSL
