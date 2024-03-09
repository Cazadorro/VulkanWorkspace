#ifndef ALGORITHMF32_GLSL
#define ALGORITHMF32_GLSL

// float32_t

float32_t min_v(f32vec2 value){
    return min(value.x, value.y);
}
float32_t min_v(f32vec3 value){
    return min(min_v(value.xy),value.z);
}
float32_t min_v(f32vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

float32_t max_v(f32vec2 value){
    return max(value.x, value.y);
}
float32_t max_v(f32vec3 value){
    return max(max_v(value.xy),value.z);
}
float32_t max_v(f32vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(f32vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(f32vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(f32vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(f32vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(f32vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(f32vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

float32_t sum(f32vec2 value){
    return value.x + value.y;
}

float32_t sum(f32vec3 value){
    return sum(value.xy) + value.z;
}

float32_t sum(f32vec4 value){
    return sum(value.xy) + sum(value.zw);
}


float32_t prod(f32vec2 value){
    return value.x * value.y;
}

float32_t prod(f32vec3 value){
    return sum(value.xy) * value.z;
}

float32_t prod(f32vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout float32_t a, inout float32_t b){
    float32_t temp = b;
    b = a;
    a = temp;
}

void swap(inout f32vec2 a, inout f32vec2 b){
    f32vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout f32vec3 a, inout f32vec3 b){
    f32vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout f32vec4 a, inout f32vec4 b){
    f32vec4 temp = b;
    b = a;
    a = temp;
}

f32vec2 sort_asc(f32vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

f32vec3 sort_asc(f32vec3 value){
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

f32vec4 sort_asc(f32vec4 value){
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

f32vec2 sort_dsc(f32vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


f32vec3 sort_dsc(f32vec3 value){
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

f32vec4 sort_dsc(f32vec4 value){
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

float32_t median(float32_t a, float32_t b, float32_t c) {
    return max(min(a, b), min(max(a, b), c));
}


float32_t ternary(bool c, float32_t a, float32_t b){
    return c ? a : b;
}

f32vec2 ternary(bvec2 c, f32vec2 a, f32vec2 b){
    //https://community.khronos.org/t/vector-version-of-ternary-operator/105282
    //wtf glsl? mix basically has an overload to act like vectorized ternary, false = first, true = second.

    return mix(b, a, c);
}

f32vec3 ternary(bvec3 c, f32vec3 a, f32vec3 b){
    return mix(b, a, c);
}

f32vec4 ternary(bvec4 c, f32vec4 a, f32vec4 b){
    return mix(b, a, c);
}





    //////

#endif //ALGORITHMF32_GLSL