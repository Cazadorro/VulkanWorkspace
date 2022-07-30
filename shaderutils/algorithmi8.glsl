#ifndef ALGORITHMI8_GLSL
#define ALGORITHMI8_GLSL

// int8_t

int8_t min_v(i8vec2 value){
    return min(value.x, value.y);
}
int8_t min_v(i8vec3 value){
    return min(min_v(value.xy),value.z);
}
int8_t min_v(i8vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

int8_t max_v(i8vec2 value){
    return max(value.x, value.y);
}
int8_t max_v(i8vec3 value){
    return max(max_v(value.xy),value.z);
}
int8_t max_v(i8vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(i8vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(i8vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(i8vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(i8vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(i8vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(i8vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

int8_t sum(i8vec2 value){
    return value.x + value.y;
}

int8_t sum(i8vec3 value){
    return sum(value.xy) + value.z;
}

int8_t sum(i8vec4 value){
    return sum(value.xy) + sum(value.zw);
}


int8_t prod(i8vec2 value){
    return value.x * value.y;
}

int8_t prod(i8vec3 value){
    return sum(value.xy) * value.z;
}

int8_t prod(i8vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout int8_t a, inout int8_t b){
    int8_t temp = b;
    b = a;
    a = temp;
}

void swap(inout i8vec2 a, inout i8vec2 b){
    i8vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout i8vec3 a, inout i8vec3 b){
    i8vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout i8vec4 a, inout i8vec4 b){
    i8vec4 temp = b;
    b = a;
    a = temp;
}

i8vec2 sort_asc(i8vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

i8vec3 sort_asc(i8vec3 value){
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

i8vec4 sort_asc(i8vec4 value){
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

i8vec2 sort_dsc(i8vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


i8vec3 sort_dsc(i8vec3 value){
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

i8vec4 sort_dsc(i8vec4 value){
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

int8_t median(int8_t a, int8_t b, int8_t c) {
    return max(min(a, b), min(max(a, b), c));
}
    //////

#endif //ALGORITHMI8_GLSL