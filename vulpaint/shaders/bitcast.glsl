#ifndef BITCAST
#define BITCAST

#extension GL_EXT_shader_explicit_arithmetic_types         : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8    : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int16   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float16 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float64 : enable

//https://github.com/gfx-rs/naga/issues/161 bitcast
//https://github.com/KhronosGroup/GLSL/blob/main/extensions/ext/GL_EXT_shader_explicit_arithmetic_types.txt

void bitcast(const in uint8_t lhs, out int8_t rhs){
    rhs = int8_t(lhs);
}
void bitcast(const in uint8_t lhs, out uint8_t rhs){
    rhs = uint8_t(lhs);
}
void bitcast(const in int8_t lhs, out uint8_t rhs){
    rhs = uint8_t(lhs);
}
void bitcast(const in int8_t lhs, out int8_t rhs){
    rhs = int8_t(lhs);
}

void bitcast(const in uint16_t lhs, out int16_t rhs){
    rhs = int16_t(lhs);
}
void bitcast(const in uint16_t lhs, out uint16_t rhs){
    rhs = uint16_t(lhs);
}
void bitcast(const in int16_t lhs, out uint16_t rhs){
    rhs = uint16_t(lhs);
}
void bitcast(const in int16_t lhs, out int16_t rhs){
    rhs = int16_t(lhs);
}

void bitcast(const in int16_t lhs, out float16_t rhs){
    rhs = int16BitsToFloat16(lhs);
}
void bitcast(const in uint16_t lhs, out float16_t rhs){
    rhs = uint16BitsToFloat16(lhs);
}

void bitcast(const in float16_t lhs, out int16_t rhs){
    rhs = float16BitsToInt16(lhs);
}
void bitcast(const in float16_t lhs, out uint16_t rhs){
    rhs = float16BitsToUint16(lhs);
}
void bitcast(const in float16_t lhs, out float16_t rhs){
    rhs = float16_t(lhs);
}


void bitcast(const in uint32_t lhs, out int32_t rhs){
    rhs = int32_t(lhs);
}
void bitcast(const in uint32_t lhs, out uint32_t rhs){
    rhs = uint32_t(lhs);
}
void bitcast(const in int32_t lhs, out uint32_t rhs){
    rhs = uint32_t(lhs);
}
void bitcast(const in int32_t lhs, out int32_t rhs){
    rhs = int32_t(lhs);
}

void bitcast(const in int32_t lhs, out float32_t rhs){
    rhs = intBitsToFloat(lhs);
}
void bitcast(const in uint32_t lhs, out float32_t rhs){
    rhs = uintBitsToFloat(lhs);
}

void bitcast(const in float32_t lhs, out int32_t rhs){
    rhs = floatBitsToInt(lhs);
}
void bitcast(const in float32_t lhs, out uint32_t rhs){
    rhs = floatBitsToUint(lhs);
}
void bitcast(const in float32_t lhs, out float32_t rhs){
    rhs = float32_t(lhs);
}


void bitcast(const in uint64_t lhs, out int64_t rhs){
    rhs = int64_t(lhs);
}
void bitcast(const in uint64_t lhs, out uint64_t rhs){
    rhs = uint64_t(lhs);
}
void bitcast(const in int64_t lhs, out uint64_t rhs){
    rhs = uint64_t(lhs);
}
void bitcast(const in int64_t lhs, out int64_t rhs){
    rhs = int64_t(lhs);
}

void bitcast(const in int64_t lhs, out float64_t rhs){
    rhs = int64BitsToDouble(lhs);
}
void bitcast(const in uint64_t lhs, out float64_t rhs){
    rhs = uint64BitsToDouble(lhs);
}

void bitcast(const in float64_t lhs, out int64_t rhs){
    rhs = doubleBitsToInt64(lhs);
}
void bitcast(const in float64_t lhs, out uint64_t rhs){
    rhs = doubleBitsToUint64(lhs);
}
void bitcast(const in float64_t lhs, out float64_t rhs){
    rhs = float64_t(lhs);
}


void bitcast(const in u8vec2 lhs, out i8vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in i8vec2 lhs, out u8vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in u16vec2 lhs, out i16vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in i16vec2 lhs, out u16vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in f16vec2 lhs, out u16vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in f16vec2 lhs, out i16vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}


void bitcast(const in u32vec2 lhs, out i32vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in i32vec2 lhs, out u32vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in f32vec2 lhs, out u32vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}
void bitcast(const in f32vec2 lhs, out i32vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in u64vec2 lhs, out i64vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in i64vec2 lhs, out u64vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in f64vec2 lhs, out u64vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}
void bitcast(const in f64vec2 lhs, out i64vec2 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
}

void bitcast(const in u8vec3 lhs, out i8vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in i8vec3 lhs, out u8vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in u16vec3 lhs, out i16vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in i16vec3 lhs, out u16vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in f16vec3 lhs, out u16vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in f16vec3 lhs, out i16vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}


void bitcast(const in u32vec3 lhs, out i32vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in i32vec3 lhs, out u32vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in f32vec3 lhs, out u32vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}
void bitcast(const in f32vec3 lhs, out i32vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in u64vec3 lhs, out i64vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in i64vec3 lhs, out u64vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}

void bitcast(const in f64vec3 lhs, out u64vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}
void bitcast(const in f64vec3 lhs, out i64vec3 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
}


void bitcast(const in u8vec4 lhs, out i8vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in i8vec4 lhs, out u8vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in u16vec4 lhs, out i16vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in i16vec4 lhs, out u16vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in f16vec4 lhs, out u16vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in f16vec4 lhs, out i16vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}


void bitcast(const in u32vec4 lhs, out i32vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in i32vec4 lhs, out u32vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in f32vec4 lhs, out u32vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}
void bitcast(const in f32vec4 lhs, out i32vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in u64vec4 lhs, out i64vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in i64vec4 lhs, out u64vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}

void bitcast(const in f64vec4 lhs, out u64vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}
void bitcast(const in f64vec4 lhs, out i64vec4 rhs){
    bitcast(lhs.x, rhs.x);
    bitcast(lhs.y, rhs.y);
    bitcast(lhs.z, rhs.z);
    bitcast(lhs.w, rhs.w);
}



void bitcast(const in i8vec2 lhs, out int16_t  rhs){
    rhs = pack16(lhs);
}
void bitcast(const in u8vec2 lhs, out uint16_t  rhs){
    rhs = pack16(lhs);
}
void bitcast(const in i8vec2 lhs, out uint16_t  rhs){
    bitcast(pack16(lhs), rhs);
}
void bitcast(const in u8vec2 lhs, out int16_t  rhs){
    bitcast(pack16(lhs), rhs);
}

void bitcast(const in i8vec2 lhs, out float16_t  rhs){
    bitcast(pack16(lhs), rhs);
}
void bitcast(const in u8vec2 lhs, out float16_t  rhs){
    bitcast(pack16(lhs), rhs);
}

void bitcast(const in int16_t lhs, out i8vec2  rhs){
    rhs = unpack8(lhs);
}
void bitcast(const in uint16_t lhs, out u8vec2  rhs){
    rhs = unpack8(lhs);
}
void bitcast(const in uint16_t lhs, out i8vec2  rhs){
    bitcast(unpack8(lhs), rhs);
}
void bitcast(const in int16_t lhs, out u8vec2  rhs){
    bitcast(unpack8(lhs), rhs);
}
void bitcast(const in float16_t lhs, out i8vec2  rhs){
    uint16_t temp;
    bitcast(lhs, temp);
    bitcast(unpack8(temp), rhs);
}
void bitcast(const in float16_t lhs, out u8vec2  rhs){
    uint16_t temp;
    bitcast(lhs, temp);
    bitcast(unpack8(temp), rhs);
}


void bitcast(const in i8vec4 lhs, out int32_t  rhs){
    rhs = pack32(lhs);
}
void bitcast(const in u8vec4 lhs, out uint32_t  rhs){
    rhs = pack32(lhs);
}
void bitcast(const in i8vec4 lhs, out uint32_t  rhs){
    bitcast(pack32(lhs), rhs);
}
void bitcast(const in u8vec4 lhs, out int32_t  rhs){
    bitcast(pack32(lhs), rhs);
}
void bitcast(const in i8vec4 lhs, out float32_t  rhs){
    bitcast(pack32(lhs), rhs);
}
void bitcast(const in u8vec4 lhs, out float32_t  rhs){
    bitcast(pack32(lhs), rhs);
}


void bitcast(const in int32_t lhs, out i8vec4  rhs){
    rhs = unpack8(lhs);
}
void bitcast(const in uint32_t lhs, out u8vec4  rhs){
    rhs = unpack8(lhs);
}
void bitcast(const in uint32_t lhs, out i8vec4  rhs){
    bitcast(unpack8(lhs), rhs);
}
void bitcast(const in int32_t lhs, out u8vec4  rhs){
    bitcast(unpack8(lhs), rhs);
}
void bitcast(const in float32_t lhs, out i8vec4  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    bitcast(unpack8(temp), rhs);
}
void bitcast(const in float32_t lhs, out u8vec4  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    bitcast(unpack8(temp), rhs);
}


void bitcast(const in i16vec2 lhs, out int32_t  rhs){
    rhs = pack32(lhs);
}
void bitcast(const in u16vec2 lhs, out uint32_t  rhs){
    rhs = pack32(lhs);
}
void bitcast(const in i16vec2 lhs, out uint32_t  rhs){
    bitcast(pack32(lhs), rhs);
}
void bitcast(const in u16vec2 lhs, out int32_t  rhs){
    bitcast(pack32(lhs), rhs);
}
void bitcast(const in i16vec2 lhs, out float32_t  rhs){
    bitcast(pack32(lhs), rhs);
}
void bitcast(const in u16vec2 lhs, out float32_t  rhs){
    bitcast(pack32(lhs), rhs);
}


void bitcast(const in int32_t lhs, out i16vec2  rhs){
    rhs = unpack16(lhs);
}
void bitcast(const in uint32_t lhs, out u16vec2  rhs){
    rhs = unpack16(lhs);
}
void bitcast(const in uint32_t lhs, out i16vec2  rhs){
    bitcast(unpack16(lhs), rhs);
}
void bitcast(const in int32_t lhs, out u16vec2  rhs){
    bitcast(unpack16(lhs), rhs);
}

void bitcast(const in float32_t lhs, out i16vec2  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    bitcast(unpack16(temp), rhs);
}
void bitcast(const in float32_t lhs, out u16vec2  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    bitcast(unpack16(temp), rhs);
}


void bitcast(const in int32_t lhs, out f16vec2  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    rhs = unpackFloat2x16(temp);
}
void bitcast(const in uint32_t lhs, out f16vec2  rhs){
    rhs = unpackFloat2x16(lhs);
}

void bitcast(const in float32_t lhs, out f16vec2  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    rhs = unpackFloat2x16(temp);
}
void bitcast(const in float32_t lhs, out f16vec2  rhs){
    uint32_t temp;
    bitcast(lhs, temp);
    rhs = unpackFloat2x16(temp);
}

void bitcast(const in f16vec2 lhs, out int32_t  rhs){
    bitcast(packFloat2x16(temp), rhs);
}
void bitcast(const in f16vec2 lhs, out uint32_t  rhs){
    rhs = packFloat2x16(temp);
}
void bitcast(const in f16vec2 lhs, out float32_t  rhs){
    bitcast(packFloat2x16(temp), rhs);
}
void bitcast(const in f16vec2 lhs, out float32_t  rhs){
    bitcast(packFloat2x16(temp), rhs);
}


void bitcast(const in u16vec4 lhs, out uint64_t  rhs){
    rhs = pack64(lhs);
}
void bitcast(const in i16vec4 lhs, out int64_t  rhs){
    rhs = pack64(lhs);
}
void bitcast(const in u16vec4 lhs, out int64_t  rhs){
    bitcast(pack64(lhs), rhs);
}
void bitcast(const in i16vec4 lhs, out uint64_t  rhs){
    bitcast(pack64(lhs), rhs);
}

void bitcast(const in u32vec2 lhs, out uint64_t  rhs){
    rhs = pack64(lhs);
}
void bitcast(const in i32vec2 lhs, out int64_t  rhs){
    rhs = pack64(lhs);
}
void bitcast(const in u32vec2 lhs, out int64_t  rhs){
    bitcast(pack64(lhs), rhs);
}
void bitcast(const in i32vec2 lhs, out uint64_t  rhs){
    bitcast(pack64(lhs), rhs);
}

void bitcast(const in int64_t lhs, out i16vec4  rhs){
    rhs = unpack16(lhs);
}
void bitcast(const in uint64_t lhs, out u16vec4  rhs){
    rhs = unpack16(lhs);
}
void bitcast(const in uint64_t lhs, out i16vec4  rhs){
    bitcast(unpack16(lhs), rhs);
}
void bitcast(const in int64_t lhs, out u16vec4  rhs){
    bitcast(unpack16(lhs), rhs);
}

void bitcast(const in int64_t lhs, out i32vec2  rhs){
    rhs = unpack32(lhs);
}
void bitcast(const in uint64_t lhs, out u32vec2  rhs){
    rhs = unpack32(lhs);
}
void bitcast(const in uint64_t lhs, out i32vec2  rhs){
    bitcast(unpack32(lhs), rhs);
}
void bitcast(const in int64_t lhs, out u32vec2  rhs){
    bitcast(unpack32(lhs), rhs);
}





//

void bitcast(const in f16vec4 lhs, out uint64_t  rhs){
    u16vec4 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}
void bitcast(const in f16vec4 lhs, out int64_t  rhs){
    u16vec4 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}

void bitcast(const in f32vec2 lhs, out uint64_t  rhs){
    u32vec2 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}
void bitcast(const in f32vec2 lhs, out int64_t  rhs){
    u32vec2 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}

void bitcast(const in int64_t lhs, out f16vec4  rhs){
    u16vec4 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}
void bitcast(const in uint64_t lhs, out f16vec4  rhs){
    u16vec4 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}


void bitcast(const in int64_t lhs, out f32vec2  rhs){
    u32vec2 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}
void bitcast(const in uint64_t lhs, out f32vec2  rhs){
    u16vec4 temp;
    bitcast(lhs, temp);
    bitcast(temp, rhs);
}



void bitcast(const in float64_t lhs, out u32vec2 rhs){
    rhs = unpackDouble2x32(lhs);
}

void bitcast(const in u32vec2 lhs, out float64_t rhs){
    rhs = packDouble2x32(lhs);
}


int8_t bitcast_i(const in uint8_t lhs){
    int8_t result; 
    bitcast(lhs, result);
    return result; 
}

int16_t bitcast_i(const in uint16_t lhs){
    int16_t result;
    bitcast(lhs, result);
    return result;
}

int32_t bitcast_i(const in uint32_t lhs){
    int32_t result;
    bitcast(lhs, result);
    return result;
}

int64_t bitcast_i(const in uint64_t lhs){
    int64_t result;
    bitcast(lhs, result);
    return result;
}


int8_t bitcast_i(const in int8_t lhs){
    int8_t result;
    bitcast(lhs, result);
    return result;
}

int16_t bitcast_i(const in int16_t lhs){
    int16_t result;
    bitcast(lhs, result);
    return result;
}

int32_t bitcast_i(const in int32_t lhs){
    int32_t result;
    bitcast(lhs, result);
    return result;
}

int64_t bitcast_i(const in int64_t lhs){
    int64_t result;
    bitcast(lhs, result);
    return result;
}

int16_t bitcast_i(const in float16_t lhs){
    int16_t result;
    bitcast(lhs, result);
    return result;
}


int32_t bitcast_i(const in float32_t lhs){
    int32_t result;
    bitcast(lhs, result);
    return result;
}

int64_t bitcast_i(const in float64_t lhs){
    int64_t result;
    bitcast(lhs, result);
    return result;
}

i8vec2 bitcast_i(const in u8vec2 lhs){
    i8vec2 result;
    bitcast(lhs, result);
    return result;
}

i16vec2 bitcast_i(const in u16vec2 lhs){
    i16vec2 result;
    bitcast(lhs, result);
    return result;
}

i32vec2 bitcast_i(const in u32vec2 lhs){
    i32vec2 result;
    bitcast(lhs, result);
    return result;
}

i64vec2 bitcast_i(const in u64vec2 lhs){
    i64vec2 result;
    bitcast(lhs, result);
    return result;
}


i8vec2 bitcast_i(const in i8vec2 lhs){
    i8vec2 result;
    bitcast(lhs, result);
    return result;
}

i16vec2 bitcast_i(const in i16vec2 lhs){
    i16vec2 result;
    bitcast(lhs, result);
    return result;
}

i32vec2 bitcast_i(const in i32vec2 lhs){
    i32vec2 result;
    bitcast(lhs, result);
    return result;
}

i64vec2 bitcast_i(const in i64vec2 lhs){
    i64vec2 result;
    bitcast(lhs, result);
    return result;
}

i16vec2 bitcast_i(const in f16vec2 lhs){
    i16vec2 result;
    bitcast(lhs, result);
    return result;
}


i32vec2 bitcast_i(const in f32vec2 lhs){
    i32vec2 result;
    bitcast(lhs, result);
    return result;
}

i64vec2 bitcast_i(const in f64vec2 lhs){
    i64vec2 result;
    bitcast(lhs, result);
    return result;
}

i8vec3 bitcast_i(const in u8vec3 lhs){
    i8vec3 result;
    bitcast(lhs, result);
    return result;
}

i16vec3 bitcast_i(const in u16vec3 lhs){
    i16vec3 result;
    bitcast(lhs, result);
    return result;
}

i32vec3 bitcast_i(const in u32vec3 lhs){
    i32vec3 result;
    bitcast(lhs, result);
    return result;
}

i64vec3 bitcast_i(const in u64vec3 lhs){
    i64vec3 result;
    bitcast(lhs, result);
    return result;
}

i8vec3 bitcast_i(const in i8vec3 lhs){
    i8vec3 result;
    bitcast(lhs, result);
    return result;
}

i16vec3 bitcast_i(const in i16vec3 lhs){
    i16vec3 result;
    bitcast(lhs, result);
    return result;
}

i32vec3 bitcast_i(const in i32vec3 lhs){
    i32vec3 result;
    bitcast(lhs, result);
    return result;
}

i64vec3 bitcast_i(const in i64vec3 lhs){
    i64vec3 result;
    bitcast(lhs, result);
    return result;
}

i16vec3 bitcast_i(const in f16vec3 lhs){
    i16vec3 result;
    bitcast(lhs, result);
    return result;
}


i32vec3 bitcast_i(const in f32vec3 lhs){
    i32vec3 result;
    bitcast(lhs, result);
    return result;
}

i64vec3 bitcast_i(const in f64vec3 lhs){
    i64vec3 result;
    bitcast(lhs, result);
    return result;
}

i8vec4 bitcast_i(const in u8vec4 lhs){
    i8vec4 result;
    bitcast(lhs, result);
    return result;
}

i16vec4 bitcast_i(const in u16vec4 lhs){
    i16vec4 result;
    bitcast(lhs, result);
    return result;
}

i32vec4 bitcast_i(const in u32vec4 lhs){
    i32vec4 result;
    bitcast(lhs, result);
    return result;
}

i64vec4 bitcast_i(const in u64vec4 lhs){
    i64vec4 result;
    bitcast(lhs, result);
    return result;
}

i8vec4 bitcast_i(const in i8vec4 lhs){
    i8vec4 result;
    bitcast(lhs, result);
    return result;
}

i16vec4 bitcast_i(const in i16vec4 lhs){
    i16vec4 result;
    bitcast(lhs, result);
    return result;
}

i32vec4 bitcast_i(const in i32vec4 lhs){
    i32vec4 result;
    bitcast(lhs, result);
    return result;
}

i64vec4 bitcast_i(const in i64vec4 lhs){
    i64vec4 result;
    bitcast(lhs, result);
    return result;
}

i16vec4 bitcast_i(const in f16vec4 lhs){
    i16vec4 result;
    bitcast(lhs, result);
    return result;
}


i32vec4 bitcast_i(const in f32vec4 lhs){
    i32vec4 result;
    bitcast(lhs, result);
    return result;
}

i64vec4 bitcast_i(const in f64vec4 lhs){
    i64vec4 result;
    bitcast(lhs, result);
    return result;
}


uint8_t bitcast_u(const in uint8_t lhs){
    uint8_t result;
    bitcast(lhs, result);
    return result;
}

uint16_t bitcast_u(const in uint16_t lhs){
    uint16_t result;
    bitcast(lhs, result);
    return result;
}

uint32_t bitcast_u(const in uint32_t lhs){
    uint32_t result;
    bitcast(lhs, result);
    return result;
}

uint64_t bitcast_u(const in uint64_t lhs){
    uint64_t result;
    bitcast(lhs, result);
    return result;
}


uint8_t bitcast_u(const in int8_t lhs){
    uint8_t result;
    bitcast(lhs, result);
    return result;
}

uint16_t bitcast_u(const in int16_t lhs){
    uint16_t result;
    bitcast(lhs, result);
    return result;
}

uint32_t bitcast_u(const in int32_t lhs){
    uint32_t result;
    bitcast(lhs, result);
    return result;
}

uint64_t bitcast_u(const in int64_t lhs){
    uint64_t result;
    bitcast(lhs, result);
    return result;
}

uint16_t bitcast_u(const in float16_t lhs){
    uint16_t result;
    bitcast(lhs, result);
    return result;
}


uint32_t bitcast_u(const in float32_t lhs){
    uint32_t result;
    bitcast(lhs, result);
    return result;
}

uint64_t bitcast_u(const in float64_t lhs){
    uint64_t result;
    bitcast(lhs, result);
    return result;
}

u8vec2 bitcast_u(const in u8vec2 lhs){
    u8vec2 result;
    bitcast(lhs, result);
    return result;
}

u16vec2 bitcast_u(const in u16vec2 lhs){
    u16vec2 result;
    bitcast(lhs, result);
    return result;
}

u32vec2 bitcast_u(const in u32vec2 lhs){
    u32vec2 result;
    bitcast(lhs, result);
    return result;
}

u64vec2 bitcast_u(const in u64vec2 lhs){
    u64vec2 result;
    bitcast(lhs, result);
    return result;
}


u8vec2 bitcast_u(const in i8vec2 lhs){
    u8vec2 result;
    bitcast(lhs, result);
    return result;
}

u16vec2 bitcast_u(const in i16vec2 lhs){
    u16vec2 result;
    bitcast(lhs, result);
    return result;
}

u32vec2 bitcast_u(const in i32vec2 lhs){
    u32vec2 result;
    bitcast(lhs, result);
    return result;
}

u64vec2 bitcast_u(const in i64vec2 lhs){
    u64vec2 result;
    bitcast(lhs, result);
    return result;
}

u16vec2 bitcast_u(const in f16vec2 lhs){
    u16vec2 result;
    bitcast(lhs, result);
    return result;
}


u32vec2 bitcast_u(const in f32vec2 lhs){
    u32vec2 result;
    bitcast(lhs, result);
    return result;
}

u64vec2 bitcast_u(const in f64vec2 lhs){
    u64vec2 result;
    bitcast(lhs, result);
    return result;
}

u8vec3 bitcast_u(const in u8vec3 lhs){
    u8vec3 result;
    bitcast(lhs, result);
    return result;
}

u16vec3 bitcast_u(const in u16vec3 lhs){
    u16vec3 result;
    bitcast(lhs, result);
    return result;
}

u32vec3 bitcast_u(const in u32vec3 lhs){
    u32vec3 result;
    bitcast(lhs, result);
    return result;
}

u64vec3 bitcast_u(const in u64vec3 lhs){
    u64vec3 result;
    bitcast(lhs, result);
    return result;
}

u8vec3 bitcast_u(const in i8vec3 lhs){
    u8vec3 result;
    bitcast(lhs, result);
    return result;
}

u16vec3 bitcast_u(const in i16vec3 lhs){
    u16vec3 result;
    bitcast(lhs, result);
    return result;
}

u32vec3 bitcast_u(const in i32vec3 lhs){
    u32vec3 result;
    bitcast(lhs, result);
    return result;
}

u64vec3 bitcast_u(const in i64vec3 lhs){
    u64vec3 result;
    bitcast(lhs, result);
    return result;
}

u16vec3 bitcast_u(const in f16vec3 lhs){
    u16vec3 result;
    bitcast(lhs, result);
    return result;
}


u32vec3 bitcast_u(const in f32vec3 lhs){
    u32vec3 result;
    bitcast(lhs, result);
    return result;
}

u64vec3 bitcast_u(const in f64vec3 lhs){
    u64vec3 result;
    bitcast(lhs, result);
    return result;
}

u8vec4 bitcast_u(const in u8vec4 lhs){
    u8vec4 result;
    bitcast(lhs, result);
    return result;
}

u16vec4 bitcast_u(const in u16vec4 lhs){
    u16vec4 result;
    bitcast(lhs, result);
    return result;
}

u32vec4 bitcast_u(const in u32vec4 lhs){
    u32vec4 result;
    bitcast(lhs, result);
    return result;
}

u64vec4 bitcast_u(const in u64vec4 lhs){
    u64vec4 result;
    bitcast(lhs, result);
    return result;
}

u8vec4 bitcast_u(const in i8vec4 lhs){
    u8vec4 result;
    bitcast(lhs, result);
    return result;
}

u16vec4 bitcast_u(const in i16vec4 lhs){
    u16vec4 result;
    bitcast(lhs, result);
    return result;
}

u32vec4 bitcast_u(const in i32vec4 lhs){
    u32vec4 result;
    bitcast(lhs, result);
    return result;
}

u64vec4 bitcast_u(const in i64vec4 lhs){
    u64vec4 result;
    bitcast(lhs, result);
    return result;
}

u16vec4 bitcast_u(const in f16vec4 lhs){
    u16vec4 result;
    bitcast(lhs, result);
    return result;
}


u32vec4 bitcast_u(const in f32vec4 lhs){
    u32vec4 result;
    bitcast(lhs, result);
    return result;
}

u64vec4 bitcast_u(const in f64vec4 lhs){
    u64vec4 result;
    bitcast(lhs, result);
    return result;
}


///



float8_t bitcast_f(const in uint8_t lhs){
    float8_t result;
    bitcast(lhs, result);
    return result;
}

float16_t bitcast_f(const in uint16_t lhs){
    float16_t result;
    bitcast(lhs, result);
    return result;
}

float32_t bitcast_f(const in uint32_t lhs){
    float32_t result;
    bitcast(lhs, result);
    return result;
}

float64_t bitcast_f(const in uint64_t lhs){
    float64_t result;
    bitcast(lhs, result);
    return result;
}


float8_t bitcast_f(const in int8_t lhs){
    float8_t result;
    bitcast(lhs, result);
    return result;
}

float16_t bitcast_f(const in int16_t lhs){
    float16_t result;
    bitcast(lhs, result);
    return result;
}

float32_t bitcast_f(const in int32_t lhs){
    float32_t result;
    bitcast(lhs, result);
    return result;
}

float64_t bitcast_f(const in int64_t lhs){
    float64_t result;
    bitcast(lhs, result);
    return result;
}

float16_t bitcast_f(const in float16_t lhs){
    float16_t result;
    bitcast(lhs, result);
    return result;
}


float32_t bitcast_f(const in float32_t lhs){
    float32_t result;
    bitcast(lhs, result);
    return result;
}

float64_t bitcast_f(const in float64_t lhs){
    float64_t result;
    bitcast(lhs, result);
    return result;
}

f8vec2 bitcast_f(const in u8vec2 lhs){
    f8vec2 result;
    bitcast(lhs, result);
    return result;
}

f16vec2 bitcast_f(const in u16vec2 lhs){
    f16vec2 result;
    bitcast(lhs, result);
    return result;
}

f32vec2 bitcast_f(const in u32vec2 lhs){
    f32vec2 result;
    bitcast(lhs, result);
    return result;
}

f64vec2 bitcast_f(const in u64vec2 lhs){
    f64vec2 result;
    bitcast(lhs, result);
    return result;
}


f8vec2 bitcast_f(const in i8vec2 lhs){
    f8vec2 result;
    bitcast(lhs, result);
    return result;
}

f16vec2 bitcast_f(const in i16vec2 lhs){
    f16vec2 result;
    bitcast(lhs, result);
    return result;
}

f32vec2 bitcast_f(const in i32vec2 lhs){
    f32vec2 result;
    bitcast(lhs, result);
    return result;
}

f64vec2 bitcast_f(const in i64vec2 lhs){
    f64vec2 result;
    bitcast(lhs, result);
    return result;
}

f16vec2 bitcast_f(const in f16vec2 lhs){
    f16vec2 result;
    bitcast(lhs, result);
    return result;
}


f32vec2 bitcast_f(const in f32vec2 lhs){
    f32vec2 result;
    bitcast(lhs, result);
    return result;
}

f64vec2 bitcast_f(const in f64vec2 lhs){
    f64vec2 result;
    bitcast(lhs, result);
    return result;
}

f8vec3 bitcast_f(const in u8vec3 lhs){
    f8vec3 result;
    bitcast(lhs, result);
    return result;
}

f16vec3 bitcast_f(const in u16vec3 lhs){
    f16vec3 result;
    bitcast(lhs, result);
    return result;
}

f32vec3 bitcast_f(const in u32vec3 lhs){
    f32vec3 result;
    bitcast(lhs, result);
    return result;
}

f64vec3 bitcast_f(const in u64vec3 lhs){
    f64vec3 result;
    bitcast(lhs, result);
    return result;
}

f8vec3 bitcast_f(const in i8vec3 lhs){
    f8vec3 result;
    bitcast(lhs, result);
    return result;
}

f16vec3 bitcast_f(const in i16vec3 lhs){
    f16vec3 result;
    bitcast(lhs, result);
    return result;
}

f32vec3 bitcast_f(const in i32vec3 lhs){
    f32vec3 result;
    bitcast(lhs, result);
    return result;
}

f64vec3 bitcast_f(const in i64vec3 lhs){
    f64vec3 result;
    bitcast(lhs, result);
    return result;
}

f16vec3 bitcast_f(const in f16vec3 lhs){
    f16vec3 result;
    bitcast(lhs, result);
    return result;
}


f32vec3 bitcast_f(const in f32vec3 lhs){
    f32vec3 result;
    bitcast(lhs, result);
    return result;
}

f64vec3 bitcast_f(const in f64vec3 lhs){
    f64vec3 result;
    bitcast(lhs, result);
    return result;
}

f8vec4 bitcast_f(const in u8vec4 lhs){
    f8vec4 result;
    bitcast(lhs, result);
    return result;
}

f16vec4 bitcast_f(const in u16vec4 lhs){
    f16vec4 result;
    bitcast(lhs, result);
    return result;
}

f32vec4 bitcast_f(const in u32vec4 lhs){
    f32vec4 result;
    bitcast(lhs, result);
    return result;
}

f64vec4 bitcast_f(const in u64vec4 lhs){
    f64vec4 result;
    bitcast(lhs, result);
    return result;
}

f8vec4 bitcast_f(const in i8vec4 lhs){
    f8vec4 result;
    bitcast(lhs, result);
    return result;
}

f16vec4 bitcast_f(const in i16vec4 lhs){
    f16vec4 result;
    bitcast(lhs, result);
    return result;
}

f32vec4 bitcast_f(const in i32vec4 lhs){
    f32vec4 result;
    bitcast(lhs, result);
    return result;
}

f64vec4 bitcast_f(const in i64vec4 lhs){
    f64vec4 result;
    bitcast(lhs, result);
    return result;
}

f16vec4 bitcast_f(const in f16vec4 lhs){
    f16vec4 result;
    bitcast(lhs, result);
    return result;
}


f32vec4 bitcast_f(const in f32vec4 lhs){
    f32vec4 result;
    bitcast(lhs, result);
    return result;
}

f64vec4 bitcast_f(const in f64vec4 lhs){
    f64vec4 result;
    bitcast(lhs, result);
    return result;
}




#endif//BITCAST
