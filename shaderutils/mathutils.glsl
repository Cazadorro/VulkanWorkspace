#ifndef MATHUTILS_GLSL
#define MATHUTILS_GLSL

float pow2(float value){
    return value * value;
}
float pow3(float value){
    return value * value * value;
}

float pow4(float value){
    return pow2(pow2(value));
}

float pow5(float value){
    return pow4(value) * value;
}



float copysign(float mag, float sgn){
    //we could use bit manipulation, but hypothetically the compiler is going to
    //have a way easier time at optimizing this.
    if(sgn < 0.0){
        return -mag;
    }else{
        return mag;
    }
}

float length2(vec2 value){
    return dot(value,value);
}

float length2(vec3 value){
    return dot(value,value);
}

float length2(vec4 value){
    return dot(value,value);
}
    
//float max_v(vec2 value){
//    return max(value.x, value.y);
//}
//float max_v(vec3 value){
//    return max(max_v(value.xy),value.z);
//}
//float max_v(vec4 value){
//    return max(max_v(value.xy), max_v(value.zw));
//}
//
//float min_v(vec2 value){
//    return min(value.x, value.y);
//}
//float min_v(vec3 value){
//    return min(min_v(value.xy),value.z);
//}
//float min_v(vec4 value){
//    return min(min_v(value.xy), min_v(value.zw));
//}


    #endif //MATHUTILS_GLSL