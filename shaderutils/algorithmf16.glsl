#ifndef ALGORITHMF16_GLSL
#define ALGORITHMF16_GLSL

// float16_t

float16_t min_v(f16vec2 value){
    return min(value.x, value.y);
}
float16_t min_v(f16vec3 value){
    return min(min_v(value.xy),value.z);
}
float16_t min_v(f16vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

float16_t max_v(f16vec2 value){
    return max(value.x, value.y);
}
float16_t max_v(f16vec3 value){
    return max(max_v(value.xy),value.z);
}
float16_t max_v(f16vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(f16vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(f16vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(f16vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(f16vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(f16vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(f16vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

float16_t sum(f16vec2 value){
    return value.x + value.y;
}

float16_t sum(f16vec3 value){
    return sum(value.xy) + value.z;
}

float16_t sum(f16vec4 value){
    return sum(value.xy) + sum(value.zw);
}


float16_t prod(f16vec2 value){
    return value.x * value.y;
}

float16_t prod(f16vec3 value){
    return sum(value.xy) * value.z;
}

float16_t prod(f16vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout float16_t a, inout float16_t b){
    float16_t temp = b;
    b = a;
    a = temp;
}

void swap(inout f16vec2 a, inout f16vec2 b){
    f16vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout f16vec3 a, inout f16vec3 b){
    f16vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout f16vec4 a, inout f16vec4 b){
    f16vec4 temp = b;
    b = a;
    a = temp;
}

f16vec2 sort_asc(f16vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

f16vec3 sort_asc(f16vec3 value){
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

f16vec4 sort_asc(f16vec4 value){
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

f16vec2 sort_dsc(f16vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


f16vec3 sort_dsc(f16vec3 value){
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

f16vec4 sort_dsc(f16vec4 value){
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

float16_t median(float16_t a, float16_t b, float16_t c) {
    return max(min(a, b), min(max(a, b), c));
}

float16_t ternary( bool c, float16_t a, float16_t b){
    return c ? a : b; 
}

f16vec2 ternary( bvec2 c,f16vec2 a, f16vec2 b){
    return mix(b, a, c);
}

f16vec3 ternary( bvec3 c,f16vec3 a, f16vec3 b){
    return mix(b, a, c);
}

f16vec4 ternary( bvec4 c,f16vec4 a, f16vec4 b){
    return mix(b, a, c);
}

    //////

#endif //ALGORITHMF16_GLSL