#ifndef MATHUTILS_GLSL
#define MATHUTILS_GLSL

float pow2(float value){
    return value * value;
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


#endif //MATHUTILS_GLSL