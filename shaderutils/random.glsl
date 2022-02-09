#ifndef RANDOM_GLSL
#define RANDOM_GLSL
#include "murmurhash.glsl"

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

float rand01(vec2 val){
    return iqconv(iqint3(uvec2(floatBitsToUint(val.x),
    floatBitsToUint(val.y))));
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

////////////////////////////////////////
////////////////////////////////////////
vec3 randomSpherePoint(vec3 rand) {
    float ang1 = (rand.x + 1.0) * pi;// [-1..1) -> [0..2*PI)
    float u = rand.y;// [-1..1), cos and acos(2v-1) cancel each other out, so we arrive at [-1..1)
    float u2 = u * u;
    float sqrt1MinusU2 = sqrt(1.0 - u2);
    float x = sqrt1MinusU2 * cos(ang1);
    float y = sqrt1MinusU2 * sin(ang1);
    float z = u;
    return vec3(x, y, z);
}
vec3 random_unit_vector(vec3 seed){
    vec2 random = iqtest(seed);
    float z = random.x;
    float az = random.y * pi;
    float el = asin(z / 1.0);
    el = atan(z, sqrt(1.0 - z*z));
    float x = cos(el) * cos(az);
    float y = cos(el) * sin(az);
    //    return normalize(randomSpherePoint(hashwithoutsine33(seed)));
    return vec3(z, y, x);
}

vec3 random_in_unit_sphere(vec3 seed){
    return random_unit_vector(seed);
}


vec3 random_in_hemisphere(vec3 seed, vec3 normal){
    vec3 in_unit_sphere = random_unit_vector(seed);
    if (dot(in_unit_sphere, normal) > 0.0){
        return in_unit_sphere;
    } else {
        return -in_unit_sphere;
    }
}

vec3 random_vec3_01(vec3 seed){
    return hash33(seed);
}

float random_float_01(vec3 seed){
    //    return iqtest(seed).x;
    //    return hashwithoutsine13(seed);
    return hash13(seed);
}


float random_float_11(vec3 seed){
    //    return iqtest(seed).x;
    return (2.0 * random_float_01(seed)) - 1.0;
}
//https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
vec2 random_in_unit_disk(vec3 seed){
    vec2 rand = hashwithoutsine23(seed);
    float r = sqrt(rand.x);
    float theta = rand.y * tau;
    float x = r * cos(theta);
    float y = r * sin(theta);
    return vec2(x,y);
}
#endif //RANDOM_GLSL