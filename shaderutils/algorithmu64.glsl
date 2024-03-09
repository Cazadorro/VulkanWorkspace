#ifndef ALGORITHMU64_GLSL
#define ALGORITHMU64_GLSL

// uint64_t

uint64_t min_v(u64vec2 value){
    return min(value.x, value.y);
}
uint64_t min_v(u64vec3 value){
    return min(min_v(value.xy),value.z);
}
uint64_t min_v(u64vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

uint64_t max_v(u64vec2 value){
    return max(value.x, value.y);
}
uint64_t max_v(u64vec3 value){
    return max(max_v(value.xy),value.z);
}
uint64_t max_v(u64vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(u64vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(u64vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(u64vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(u64vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(u64vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(u64vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint64_t sum(u64vec2 value){
    return value.x + value.y;
}

uint64_t sum(u64vec3 value){
    return sum(value.xy) + value.z;
}

uint64_t sum(u64vec4 value){
    return sum(value.xy) + sum(value.zw);
}


uint64_t prod(u64vec2 value){
    return value.x * value.y;
}

uint64_t prod(u64vec3 value){
    return sum(value.xy) * value.z;
}

uint64_t prod(u64vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout uint64_t a, inout uint64_t b){
    uint64_t temp = b;
    b = a;
    a = temp;
}

void swap(inout u64vec2 a, inout u64vec2 b){
    u64vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout u64vec3 a, inout u64vec3 b){
    u64vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout u64vec4 a, inout u64vec4 b){
    u64vec4 temp = b;
    b = a;
    a = temp;
}

u64vec2 sort_asc(u64vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

u64vec3 sort_asc(u64vec3 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    if (value[0] > value[2]){
        swap(value[0], value[2]);
    }
    if (value[1] > value[2]){
        swap(value[1], value[2]);
    }
    return value;
}

u64vec4 sort_asc(u64vec4 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    if (value[2] > value[3]){
        swap(value[2], value[3]);
    }
    if (value[0] > value[2]){
        swap(value[0], value[2]);
    }
    if (value[1] > value[3]){
        swap(value[1], value[3]);
    }
    if (value[1] > value[2]){
        swap(value[1], value[2]);
    }
    return value;
}

u64vec2 sort_dsc(u64vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


u64vec3 sort_dsc(u64vec3 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    if (value[0] < value[2]){
        swap(value[0], value[2]);
    }
    if (value[1] < value[2]){
        swap(value[1], value[2]);
    }
    return value;
}

u64vec4 sort_dsc(u64vec4 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    if (value[2] < value[3]){
        swap(value[2], value[3]);
    }
    if (value[0] < value[2]){
        swap(value[0], value[2]);
    }
    if (value[1] < value[3]){
        swap(value[1], value[3]);
    }
    if (value[1] < value[2]){
        swap(value[1], value[2]);
    }
    return value;
}

uint64_t median(uint64_t a, uint64_t b, uint64_t c) {
    return max(min(a, b), min(max(a, b), c));
}

uint64_t wrap(uint64_t value, uint64_t max){
    return ((value % max) + max) % max;
}
u64vec2 wrap(u64vec2 value, uint64_t max){
    return u64vec2(wrap(value.x, max), wrap(value.y, max));
}
u64vec3 wrap(u64vec3 value, uint32_t max){
    return u64vec3(wrap(value.x, max), wrap(value.y, max), wrap(value.z, max));
}
u64vec4 wrap(u64vec4 value, uint32_t max){
    return u64vec4(wrap(value.x, max), wrap(value.y, max), wrap(value.z, max), wrap(value.w, max));
}

uint64_t ternary(bool c , uint64_t a, uint64_t b ){
    return c ? a : b;
}

u64vec2 ternary(bvec2 c, u64vec2 a, u64vec2 b ){
    return mix(b, a, c);
}

u64vec3 ternary(bvec3 c, u64vec3 a, u64vec3 b ){
    return mix(b, a, c);
}

u64vec4 ternary(bvec4 c, u64vec4 a, u64vec4 b ){
    return mix(b, a, c);
}
    //////

    #endif //ALGORITHMU64_GLSL