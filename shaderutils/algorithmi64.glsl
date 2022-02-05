#ifndef ALGORITHMI64_GLSL
#define ALGORITHMI64_GLSL

// int64_t

int64_t min_v(i64vec2 value){
    return min(value.x, value.y);
}
int64_t min_v(i64vec3 value){
    return min(min_v(value.xy),value.z);
}
int64_t min_v(i64vec4 value){
    return min(min_v(value.xy), min_v(value.zw));
}

int64_t max_v(i64vec2 value){
    return max(value.x, value.y);
}
int64_t max_v(i64vec3 value){
    return max(max_v(value.xy),value.z);
}
int64_t max_v(i64vec4 value){
    return max(max_v(value.xy), max_v(value.zw));
}

uint8_t arg_min(i64vec2 value){
    if(value.x < value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_min(i64vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] < value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_min(i64vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] < value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

uint8_t arg_max(i64vec2 value){
    if(value.x > value.y){
        return uint8_t(0);
    }else{
        return uint8_t(1);
    }
}

uint8_t arg_max(i64vec3 value){
    uint8_t idx = arg_min(value.xy);
    if(value[uint(idx)] > value.z){
        return idx;
    }else{
        return uint8_t(2);
    }
}

uint8_t arg_max(i64vec4 value){
    uint8_t idx = arg_min(value.xyz);
    if(value[uint(idx)] > value.w){
        return idx;
    }else{
        return uint8_t(3);
    }
}

int64_t sum(i64vec2 value){
    return value.x + value.y;
}

int64_t sum(i64vec3 value){
    return sum(value.xy) + value.z;
}

int64_t sum(i64vec4 value){
    return sum(value.xy) + sum(value.zw);
}


int64_t prod(i64vec2 value){
    return value.x * value.y;
}

int64_t prod(i64vec3 value){
    return sum(value.xy) * value.z;
}

int64_t prod(i64vec4 value){
    return sum(value.xy) * sum(value.zw);
}

void swap(inout int64_t a, inout int64_t b){
    int64_t temp = b;
    b = a;
    a = temp;
}

void swap(inout i64vec2 a, inout i64vec2 b){
    i64vec2 temp = b;
    b = a;
    a = temp;
}

void swap(inout i64vec3 a, inout i64vec3 b){
    i64vec3 temp = b;
    b = a;
    a = temp;
}

void swap(inout i64vec4 a, inout i64vec4 b){
    i64vec4 temp = b;
    b = a;
    a = temp;
}

i64vec2 sort_asc(i64vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] > value[1]){
        swap(value[0], value[1]);
    }
    return value;
}

i64vec3 sort_asc(i64vec3 value){
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

i64vec4 sort_asc(i64vec4 value){
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

i64vec2 sort_dsc(i64vec2 value){
    //http://pages.ripco.net/~jgamble/nw.html
    if (value[0] < value[1]){
        swap(value[0], value[1]);
    }
    return value;
}


i64vec3 sort_dsc(i64vec3 value){
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

i64vec4 sort_dsc(i64vec4 value){
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

#endif //ALGORITHMI64_GLSL