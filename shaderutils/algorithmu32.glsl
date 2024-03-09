#ifndef ALGORITHMU32_GLSL
#define ALGORITHMU32_GLSL

// uint32_t

uint32_t min_v(u32vec2 value){
    return min(value.x, value.y);
}
uint32_t min_v(u32vec3 value){
    return min(min_v(value.xy),value.z);
}
uint32_t min_v(u32vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

uint32_t max_v(u32vec2 value){
    return max(value.x, value.y);
}
uint32_t max_v(u32vec3 value){
    return max(max_v(value.xy),value.z);
}
uint32_t max_v(u32vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(u32vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(u32vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(u32vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(u32vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(u32vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(u32vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint32_t sum(u32vec2 value){
    return value.x + value.y;
}

uint32_t sum(u32vec3 value){
    return sum(value.xy) + value.z;
}

uint32_t sum(u32vec4 value){
    return sum(value.xy) + sum(value.zw);
}


uint32_t prod(u32vec2 value){
    return value.x * value.y;
}

uint32_t prod(u32vec3 value){
    return sum(value.xy) * value.z;
}

uint32_t prod(u32vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout uint32_t a, inout uint32_t b){
    uint32_t temp = b;
    b = a;
    a = temp;
}

void swap(inout u32vec2 a, inout u32vec2 b){
    u32vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout u32vec3 a, inout u32vec3 b){
    u32vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout u32vec4 a, inout u32vec4 b){
    u32vec4 temp = b;
    b = a;
    a = temp;
}

u32vec2 sort_asc(u32vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

u32vec3 sort_asc(u32vec3 value){
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

u32vec4 sort_asc(u32vec4 value){
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

u32vec2 sort_dsc(u32vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


u32vec3 sort_dsc(u32vec3 value){
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

u32vec4 sort_dsc(u32vec4 value){
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

uint32_t median(uint32_t a, uint32_t b, uint32_t c) {
    return max(min(a, b), min(max(a, b), c));
}

uint32_t wrap(uint32_t value, uint32_t max){
    return ((value % max) + max) % max;
}
u32vec2 wrap(u32vec2 value, uint32_t max){
    return u32vec2(wrap(value.x, max), wrap(value.y, max));
}
u32vec3 wrap(u32vec3 value, uint32_t max){
    return u32vec3(wrap(value.x, max), wrap(value.y, max), wrap(value.z, max));
}
u32vec4 wrap(u32vec4 value, uint32_t max){
    return u32vec4(wrap(value.x, max), wrap(value.y, max), wrap(value.z, max), wrap(value.w, max));
}

uint32_t ternary(bool c , uint32_t a, uint32_t b ){
    return c ? a : b;
}

u32vec2 ternary(bvec2 c, u32vec2 a, u32vec2 b ){
    return mix(b, a, c);
}

u32vec3 ternary(bvec3 c, u32vec3 a, u32vec3 b ){
    return mix(b, a, c);
}

u32vec4 ternary(bvec4 c, u32vec4 a, u32vec4 b ){
    return mix(b, a, c);
}
    //////

    #endif //ALGORITHMU32_GLSL