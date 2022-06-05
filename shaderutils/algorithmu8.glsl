#ifndef ALGORITHMU8_GLSL
#define ALGORITHMU8_GLSL

// uint8_t

uint8_t min_v(u8vec2 value){
    return min(value.x, value.y);
}
uint8_t min_v(u8vec3 value){
    return min(min_v(value.xy),value.z);
}
uint8_t min_v(u8vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

uint8_t max_v(u8vec2 value){
    return max(value.x, value.y);
}
uint8_t max_v(u8vec3 value){
    return max(max_v(value.xy),value.z);
}
uint8_t max_v(u8vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(u8vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(u8vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(u8vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(u8vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(u8vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(u8vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t sum(u8vec2 value){
    return value.x + value.y;
}

uint8_t sum(u8vec3 value){
    return sum(value.xy) + value.z;
}

uint8_t sum(u8vec4 value){
    return sum(value.xy) + sum(value.zw);
}


uint8_t prod(u8vec2 value){
    return value.x * value.y;
}

uint8_t prod(u8vec3 value){
    return sum(value.xy) * value.z;
}

uint8_t prod(u8vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout uint8_t a, inout uint8_t b){
    uint8_t temp = b;
    b = a;
    a = temp;
}

void swap(inout u8vec2 a, inout u8vec2 b){
    u8vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout u8vec3 a, inout u8vec3 b){
    u8vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout u8vec4 a, inout u8vec4 b){
    u8vec4 temp = b;
    b = a;
    a = temp;
}

u8vec2 sort_asc(u8vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

u8vec3 sort_asc(u8vec3 value){
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

u8vec4 sort_asc(u8vec4 value){
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

u8vec2 sort_dsc(u8vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


u8vec3 sort_dsc(u8vec3 value){
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

u8vec4 sort_dsc(u8vec4 value){
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

uint8_t median(uint8_t a, uint8_t b, uint8_t c) {
    return max(min(r, g), min(max(r, g), b));
}
    //////

#endif //ALGORITHMU8_GLSL