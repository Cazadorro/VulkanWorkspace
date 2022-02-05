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
    //////

    #endif //ALGORITHMU64_GLSL