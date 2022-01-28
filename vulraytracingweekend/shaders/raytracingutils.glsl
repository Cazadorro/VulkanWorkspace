#ifndef RAYTRACINGUTILS_GLSL
#define RAYTRACINGUTILS_GLSL
#include "mathutils.glsl"

struct Ray{
    vec3 pos;
    vec3 dir;
};

vec3 endpoint(const in Ray ray, float t){
    return ray.pos + ray.dir * t;
}

vec3 calc_frag_dir(
    const in vec2 fragCoord,
    const in uvec2 resolution,
    const in float fov){
    vec2 uv = vec2(fragCoord) / resolution.xy;
    uv.x = (uv.x * 2.0) - 1.0;
    uv.y = (2.0 * uv.y) - 1.0;
    uv.y *= -1.0;
    if(resolution.x >= resolution.y){
        uv.x *= float(resolution.x)/resolution.y;
    }else{
        uv.y *= float(resolution.y)/resolution.x;
    }
//    uv.x *= 2.0;
    float tan_fov = tan(fov/2.0);
    vec2 pxy = uv * tan_fov;
    vec3 ray_dir = normalize(vec3(pxy, 1));
    return ray_dir;
}

vec3 rotVecX(in vec3 vector, in float theta){
    mat3 rotmat = mat3(1, 0, 0,
    0, cos(theta), -sin(theta),
    0, sin(theta), cos(theta));
    return vector * rotmat;
}

vec3 rotVecY(in vec3 vector, in float theta){
    mat3 rotmat = mat3(cos(theta), 0, sin(theta),
    0, 1, 0,
    -sin(theta), 0, cos(theta));
    return vector * rotmat;
}
vec3 rotVecZ(in vec3 vector, in float theta){
    mat3 rotmat = mat3(cos(theta), -sin(theta), 0,
    sin(theta), cos(theta), 0,
    0, 0, 1);
    return vector * rotmat;
}

vec3 rotate_dir(in vec3 ray_dir, in vec3 rotation){
    // Normalized pixel coordinates (from -1 to 1)
    ray_dir = rotVecX(ray_dir, rotation.x);
    ray_dir = rotVecY(ray_dir, rotation.y);
    return ray_dir;
}


Ray create_Ray(
    const in vec2 frag_coord,
    const in uvec2 resolution,
    const in vec3 camera_origin,
    const in vec3 camera_rotation,
    const in float fov){

    vec3 ray_dir = calc_frag_dir(frag_coord, resolution, fov);
    vec3 rot_ray_dir = rotate_dir(ray_dir, camera_rotation);
    vec3 ray_origin = camera_origin;
    float cosA = ray_dir.z;// needed for depth calculation.
    return Ray(ray_origin, rot_ray_dir);
}

struct Sphere{
    vec3 pos;
    float r;
};

//more accurate difference of products using FMA:
//https://pharr.org/matt/blog/2019/11/03/difference-of-floats
//used for (a*b) - (c*d)
float diff_of_products (float a, float b, float c, float d){
    float cd = d * c;
    float err = fma (-d, c, cd);
    float dop = fma (a, b, -cd);
    return dop + err;
}
float dop_discriminant_half_b(float a, float half_b, float c){
    return diff_of_products(half_b,half_b,a,c);
}
float dop_discriminant(float a, float b, float c){
    return diff_of_products(b,b,4.0*a,c);
}

//https://stackoverflow.com/a/50065711/
bool solveQuadratic(const in float a, const in float b, const in float c, out float t0, out float t1){
    float x0, x1;
    float discriminant = dop_discriminant(a,b,c);
    if(discriminant < 0.0){
        return false;
    }else if(discriminant == 0.0){
        x0 = -b/a;
        x1 = -b/a;
    }else{
        float q = -0.5 * (b + copysign(sqrt(discriminant), b));
        x0 = q / a;
        x1 = c / q;
    }

    if(x0 > x1){
        t0 = x1;
        t1 = x0;
    } else {
        t0 = x0;
        t1 = x1;
    }
    return true;
}

bool solveQuadraticHalfB(const in float a, const in float half_b, const in float c, out float t0, out float t1){
    float x0, x1;
    float discriminant = dop_discriminant_half_b(a,half_b,c);
    if(discriminant < 0.0){
        return false;
    }else if(discriminant == 0.0){
        x0 = -half_b/a;
        x1 = -half_b/a;
    }else{
        float q = -1 * (half_b + copysign(sqrt(discriminant), half_b));
        x0 = q / a;
        x1 = c / q;
    }

    if(x0 > x1){
        t0 = x1;
        t1 = x0;
    } else {
        t0 = x0;
        t1 = x1;
    }
    return true;
}

struct IntersectPair{
    float to_object;
    float to_end;
};

bool intersect(const in Ray ray, const in Sphere obj, out IntersectPair intersection){
    vec3 oc = ray.pos - obj.pos;
    float a = dot(ray.dir, ray.dir);
    float half_b = dot(oc, ray.dir);
    float c = dot(oc, oc) - obj.r*obj.r;
    float t0,t1;
    if(!solveQuadratic(a,2.0*half_b,c,t0,t1)){
        return false;
    }
    if(t0 < 0.0 && t1 < 0.0){
        return false;
    }
    intersection.to_end = t1;
    if(t0 < 0.0){
        intersection.to_object = 0.0;
    }else {
        intersection.to_object = t0;
    }
    return true;
}

struct HitRecord{
    float to_object;
    float to_end;
    vec3 normal;
};
bool is_inside(const in HitRecord hit_record){
   return hit_record.to_object == 0.0;
}
bool is_outside(const in HitRecord hit_record){
    return !is_inside(hit_record);
}
vec3 calc_front_normal(const in HitRecord hit_record){
    return is_outside(hit_record) ? hit_record.normal : -hit_record.normal;
}
float get_first_surface(const in HitRecord hit_record){
    return is_outside(hit_record) ? hit_record.to_object : hit_record.to_end;
}
bool intersect(const in Ray ray, const in Sphere obj, out HitRecord hit_record){
    vec3 oc = ray.pos - obj.pos;
    float a = dot(ray.dir, ray.dir);
    float half_b = dot(oc, ray.dir);
    float c = dot(oc, oc) - obj.r*obj.r;
    float t0,t1;
    if(!solveQuadratic(a,2.0*half_b,c,t0,t1)){
        return false;
    }
    if(t0 < 0.0 && t1 < 0.0){
        return false;
    }

    hit_record.to_end = t1;
    if(t0 < 0.0){
        hit_record.to_object = 0.0;
    }else {
        hit_record.to_object = t0;
    }
    hit_record.normal = (endpoint(ray, hit_record.to_object) - obj.pos) / obj.r;
    hit_record.normal = calc_front_normal(hit_record);
    return true;
}



#endif //RAYTRACINGUTILS_GLSL