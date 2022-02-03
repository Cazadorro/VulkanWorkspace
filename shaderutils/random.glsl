#ifndef RANDOM_GLSL
#define RANDOM_GLSL


float iqhashIII(uvec2 x){
    uvec2 q = 1103515245U * ((x>>1U) ^ (x.yx));
    uint  n = 1103515245U * ((q.x) ^ (q.y>>3U));
    return float(n) * (1.0/float(0xffffffffU));
}

vec2 hashwithoutsine23(vec3 p3){
    p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return (fract((p3.xx+p3.yz)*p3.zy));
}
vec2 hashwithoutsine23_11(vec3 p3){
    return (hashwithoutsine23(p3) * 2.0) - 1.0;
}

//3d point in, random value out.
vec3 hashwithoutsine33(vec3 p3)
{
    p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy + p3.yxx)*p3.zyx);
}

//I think these functions ranges are 0->1.
float hashwithoutsine13(vec3 p3)
{
    p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}




uvec3 iqint2(uvec3 x){
    const uint k = 1103515245u;

    x = ((x>>8U)^x.yzx)*k;
    x = ((x>>8U)^x.yzx)*k;
    x = ((x>>8U)^x.yzx)*k;

    return x;
}

float iqconv(uint val){
    return float(val) * (1.0/float(0xffffffffU));
}

vec3 iqint2(vec3 val){
    uvec3 ux = uvec3(
    floatBitsToUint(val.x),
    floatBitsToUint(val.y),
    floatBitsToUint(val.z)
    );
    uvec3 ures = iqint2(ux);
    return vec3(
    iqconv(ures.x),
    iqconv(ures.y),
    iqconv(ures.z)
    );
}

uint iqint1(uint n)
{
    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;

    return n;
}

uint iqint3(uvec2 x)
{
    uvec2 q = 1103515245U * ((x>>1U) ^ (x.yx));
    uint  n = 1103515245U * ((q.x) ^ (q.y>>3U));

    return n;
}
uvec2 iqtest(uvec3 val){
    uvec2 res;
    res.x = iqint3(val.xy);
    res.y = iqint3(val.xz);
    return res;
}

vec2 iqtest(vec3 val){
    uvec3 ux = uvec3(
    floatBitsToUint(val.x),
    floatBitsToUint(val.y),
    floatBitsToUint(val.z)
    );
    uvec2 ures = iqtest(ux);
    return vec2(
    iqconv(ures.x),
    iqconv(ures.y)
    );
}
#endif //RANDOM_GLSL