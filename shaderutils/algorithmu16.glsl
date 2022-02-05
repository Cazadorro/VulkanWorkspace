#ifndef ALGORITHMU16_GLSL
#define ALGORITHMU16_GLSL

// uint16_t

uint16_t min_v(u16vec2 value){
    return min(value.x, value.y);
}
uint16_t min_v(u16vec3 value){
    return min(min_v(value.xy),value.z);
}
uint16_t min_v(u16vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

uint16_t max_v(u16vec2 value){
    return max(value.x, value.y);
}
uint16_t max_v(u16vec3 value){
    return max(max_v(value.xy),value.z);
}
uint16_t max_v(u16vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(u16vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(u16vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(u16vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(u16vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(u16vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(u16vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint16_t sum(u16vec2 value){
    return value.x + value.y;
}

uint16_t sum(u16vec3 value){
    return sum(value.xy) + value.z;
}

uint16_t sum(u16vec4 value){
    return sum(value.xy) + sum(value.zw);
}


uint16_t prod(u16vec2 value){
    return value.x * value.y;
}

uint16_t prod(u16vec3 value){
    return sum(value.xy) * value.z;
}

uint16_t prod(u16vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout uint16_t a, inout uint16_t b){
    uint16_t temp = b;
    b = a;
    a = temp;
}

void swap(inout u16vec2 a, inout u16vec2 b){
    u16vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout u16vec3 a, inout u16vec3 b){
    u16vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout u16vec4 a, inout u16vec4 b){
    u16vec4 temp = b;
    b = a;
    a = temp;
}

u16vec2 sort_asc(u16vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

u16vec3 sort_asc(u16vec3 value){
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

u16vec4 sort_asc(u16vec4 value){
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

u16vec2 sort_dsc(u16vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


u16vec3 sort_dsc(u16vec3 value){
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

u16vec4 sort_dsc(u16vec4 value){
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

    #endif //ALGORITHMU16_GLSL