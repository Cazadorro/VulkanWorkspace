#ifndef ALGORITHMI32_GLSL
#define ALGORITHMI32_GLSL

// int32_t

int32_t min_v(i32vec2 value){
    return min(value.x, value.y);
}
int32_t min_v(i32vec3 value){
    return min(min_v(value.xy),value.z);
}
int32_t min_v(i32vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

int32_t max_v(i32vec2 value){
    return max(value.x, value.y);
}
int32_t max_v(i32vec3 value){
    return max(max_v(value.xy),value.z);
}
int32_t max_v(i32vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(i32vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(i32vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(i32vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(i32vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(i32vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(i32vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

int32_t sum(i32vec2 value){
    return value.x + value.y;
}

int32_t sum(i32vec3 value){
    return sum(value.xy) + value.z;
}

int32_t sum(i32vec4 value){
    return sum(value.xy) + sum(value.zw);
}


int32_t prod(i32vec2 value){
    return value.x * value.y;
}

int32_t prod(i32vec3 value){
    return prod(value.xy) * value.z;
}

int32_t prod(i32vec4 value){
    return prod(value.xy) * prod(value.zw);
}

void swap(inout int32_t a, inout int32_t b){
    int32_t temp = b;
    b = a;
    a = temp;
}

void swap(inout i32vec2 a, inout i32vec2 b){
    i32vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout i32vec3 a, inout i32vec3 b){
    i32vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout i32vec4 a, inout i32vec4 b){
    i32vec4 temp = b;
    b = a;
    a = temp;
}

i32vec2 sort_asc(i32vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

i32vec3 sort_asc(i32vec3 value){
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

i32vec4 sort_asc(i32vec4 value){
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

i32vec2 sort_dsc(i32vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


i32vec3 sort_dsc(i32vec3 value){
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

i32vec4 sort_dsc(i32vec4 value){
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

int32_t median(int32_t a, int32_t b, int32_t c) {
    return max(min(a, b), min(max(a, b), c));
}

int32_t wrap(int32_t value, int32_t max){
    return ((value % max) + max) % max;
}
i32vec2 wrap(i32vec2 value, int32_t max){
    return i32vec2(wrap(value.x, max), wrap(value.y, max));
}
i32vec3 wrap(i32vec3 value, int32_t max){
    return i32vec3(wrap(value.x, max), wrap(value.y, max), wrap(value.z, max));
}
i32vec4 wrap(i32vec4 value, int32_t max){
    return i32vec4(wrap(value.x, max), wrap(value.y, max), wrap(value.z, max), wrap(value.w, max));
}

int32_t ternary(bool c, int32_t a, int32_t b){
    return c ? a : b;
}

i32vec2 ternary( bvec2 c, i32vec2 a, i32vec2 b){
    return mix(b, a, c);
}

i32vec3 ternary( bvec3 c, i32vec3 a, i32vec3 b){
    return mix(b, a, c);
}

i32vec4 ternary( bvec4 c, i32vec4 a, i32vec4 b){
    return mix(b, a, c);
}


    //////

#endif //ALGORITHMI32_GLSL