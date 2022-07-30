#ifndef ALGORITHMI16_GLSL
#define ALGORITHMI16_GLSL

// int16_t

int16_t min_v(i16vec2 value){
    return min(value.x, value.y);
}
int16_t min_v(i16vec3 value){
    return min(min_v(value.xy),value.z);
}
int16_t min_v(i16vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

int16_t max_v(i16vec2 value){
    return max(value.x, value.y);
}
int16_t max_v(i16vec3 value){
    return max(max_v(value.xy),value.z);
}
int16_t max_v(i16vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(i16vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(i16vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(i16vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(i16vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(i16vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(i16vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

int16_t sum(i16vec2 value){
    return value.x + value.y;
}

int16_t sum(i16vec3 value){
    return sum(value.xy) + value.z;
}

int16_t sum(i16vec4 value){
    return sum(value.xy) + sum(value.zw);
}


int16_t prod(i16vec2 value){
    return value.x * value.y;
}

int16_t prod(i16vec3 value){
    return sum(value.xy) * value.z;
}

int16_t prod(i16vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout int16_t a, inout int16_t b){
    int16_t temp = b;
    b = a;
    a = temp;
}

void swap(inout i16vec2 a, inout i16vec2 b){
    i16vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout i16vec3 a, inout i16vec3 b){
    i16vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout i16vec4 a, inout i16vec4 b){
    i16vec4 temp = b;
    b = a;
    a = temp;
}

i16vec2 sort_asc(i16vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

i16vec3 sort_asc(i16vec3 value){
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

i16vec4 sort_asc(i16vec4 value){
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

i16vec2 sort_dsc(i16vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


i16vec3 sort_dsc(i16vec3 value){
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

i16vec4 sort_dsc(i16vec4 value){
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

int16_t median(int16_t a, int16_t b, int16_t c) {
    return max(min(a, b), min(max(a, b), c));
}
    //////

#endif //ALGORITHMI16_GLSL