#ifndef ALGORITHMF64_GLSL
#define ALGORITHMF64_GLSL

// float64_t

float64_t min_v(f64vec2 value){
    return min(value.x, value.y);
}
float64_t min_v(f64vec3 value){
    return min(min_v(value.xy),value.z);
}
float64_t min_v(f64vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

float64_t max_v(f64vec2 value){
    return max(value.x, value.y);
}
float64_t max_v(f64vec3 value){
    return max(max_v(value.xy),value.z);
}
float64_t max_v(f64vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(f64vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(f64vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(f64vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(f64vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(f64vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(f64vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

float64_t sum(f64vec2 value){
    return value.x + value.y;
}

float64_t sum(f64vec3 value){
    return sum(value.xy) + value.z;
}

float64_t sum(f64vec4 value){
    return sum(value.xy) + sum(value.zw);
}


float64_t prod(f64vec2 value){
    return value.x * value.y;
}

float64_t prod(f64vec3 value){
    return sum(value.xy) * value.z;
}

float64_t prod(f64vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout float64_t a, inout float64_t b){
    float64_t temp = b;
    b = a;
    a = temp;
}

void swap(inout f64vec2 a, inout f64vec2 b){
    f64vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout f64vec3 a, inout f64vec3 b){
    f64vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout f64vec4 a, inout f64vec4 b){
    f64vec4 temp = b;
    b = a;
    a = temp;
}

f64vec2 sort_asc(f64vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

f64vec3 sort_asc(f64vec3 value){
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

f64vec4 sort_asc(f64vec4 value){
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

f64vec2 sort_dsc(f64vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


f64vec3 sort_dsc(f64vec3 value){
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

f64vec4 sort_dsc(f64vec4 value){
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

float64_t median(float64_t a, float64_t b, float64_t c) {
    return max(min(r, g), min(max(r, g), b));
}
    //////

#endif //ALGORITHMF64_GLSL