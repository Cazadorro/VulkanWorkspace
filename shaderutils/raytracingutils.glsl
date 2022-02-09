#ifndef RAYTRACINGUTILS_GLSL
#define RAYTRACINGUTILS_GLSL
#extension GL_GOOGLE_include_directive :enable
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
    const in vec2 resolution,
    const in float fov,
    const in vec2 disk_offset,
    const in float focus_dist){
    vec2 uv = vec2(fragCoord); // resolution.xy;
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
    vec3 ray_dir = normalize(vec3(pxy - disk_offset, focus_dist));
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
    const in vec2 resolution,
    const in vec3 camera_origin,
    const in vec3 camera_rotation,
    const in float fov,
    const in vec2 disk_offset,
    const in float focus_dist){

    vec3 ray_dir = calc_frag_dir(frag_coord, resolution, fov, disk_offset, focus_dist);
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
    bool front_face; //TODO DON'T LIKE THIS AT ALL!!!
};

bool is_inside(const in HitRecord hit_record){
   return hit_record.to_end == 0.0;
}
bool is_outside(const in HitRecord hit_record){
    return !is_inside(hit_record);
}
vec3 calc_front_normal(const in HitRecord hit_record){
    return is_outside(hit_record) ? hit_record.normal : -hit_record.normal;
}

vec3 calc_front_normal(const in HitRecord hit_record, vec3 normal){
    return hit_record.front_face ? normal : -normal;
}
float get_first_surface(const in HitRecord hit_record){
    return is_outside(hit_record) ? hit_record.to_object : hit_record.to_end;
}

vec3 endpoint(const in Ray ray, const in HitRecord hit_record){
    return endpoint(ray,hit_record.to_object);
}
vec3 scatter_endpoint(const in Ray ray, const in HitRecord hit_record){
    vec3 ray_endpoint = endpoint(ray,hit_record.to_object);
    return ray_endpoint;
//    return ray_endpoint + hit_record.normal * 0.0000001;
}

bool calc_front_face(const in Ray ray, const in vec3 outward_normal){
    return dot(ray.dir, outward_normal) < 0.0;
}

bool intersect(const in Ray ray, const in Sphere obj, out HitRecord hit_record, float t_min){
    vec3 oc = ray.pos - obj.pos;
    float a = dot(ray.dir, ray.dir);
    float half_b = dot(oc, ray.dir);
    float c = dot(oc, oc) - obj.r*obj.r;
    float t0,t1;
    if(!solveQuadratic(a,2.0*half_b,c,t0,t1)){
        return false;
    }
    if(t0 < t_min && t1 < t_min){
        return false;
    }
    if(t0 < t_min){
        hit_record.to_object = t1;
        hit_record.to_end = 0.0;
    }else {
        hit_record.to_object = t0;
        hit_record.to_end = t1;
    }
    vec3 normal = (endpoint(ray, hit_record.to_object) - obj.pos) / obj.r;
    hit_record.front_face = calc_front_face(ray, normal);
    hit_record.normal = calc_front_normal(hit_record, normal);
    return true;
}

bool intersect(const in Ray ray, float t_min, float t_max, const in Sphere obj, out HitRecord hit_record) {
    float radius = obj.r;
    vec3 center = obj.pos;
    vec3 oc = ray.pos - center;
    float a = length2(ray.dir);
    float half_b = dot(oc, ray.dir);
    float c = length2(oc) - radius*radius;

    float discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
        return false;
    }

    hit_record.to_object = root;
//    rec.p = r.at(rec.t);
    vec3 outward_normal = (endpoint(ray, hit_record.to_object)  - center) / radius;
    hit_record.front_face = calc_front_face(ray, outward_normal);
    hit_record.normal = calc_front_normal(hit_record, outward_normal);

    return true;
}

struct Box{
    vec3 pos;
    vec3 dim;
};

bool intersect(const in Ray ray, const in Box box, float t_min, float t_max, out float t){
    vec3 dir_recipricol = 1.0f / ray.dir;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    vec3 lb = box.pos - box.dim;
    vec3 rt = box.pos + box.dim;
    float t1 = (lb.x - ray.pos.x)*dir_recipricol.x;
    float t2 = (rt.x - ray.pos.x)*dir_recipricol.x;

    float t3 = (lb.y - ray.pos.y)*dir_recipricol.y;
    float t4 = (rt.y - ray.pos.y)*dir_recipricol.y;

    float t5 = (lb.z - ray.pos.z)*dir_recipricol.z;
    float t6 = (rt.z - ray.pos.z)*dir_recipricol.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
//    tmin = max(tmin, t_min);
//    tmax = min(tmax, t_max);
    //what if we are inside?

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
//    if (tmax < 0.0){
    if (tmax < 0.0){


        //set to infinity?
//        intersection_distance.to_object = tmax;
        return false;
    }
//return x;
    // if tmin > tmax, ray doesn't intersect AABB
    //what if infinity (doesn't hit?), shoudl still be fine, can only be perpendicular to one?
    if (tmin > tmax){
        //set to infinity?
//        intersection_distance.to_object = tmax;
        return false;
    }
    if(tmax < t_min){
        return false;
    }
    if(tmin > t_max){
        return false;
    }
    if(tmin < 0.0){
        t = 0.0;
        t = tmax;
//        intersection_distance.to_object = 0.0;
    }else{
        t = tmin;
//        intersection_distance.to_object = tmin;
    }
    //don't need  tmax - tmin?
//    intersection_distance.to_end = tmax;
    return true;
}
struct HitRange{
    float to_object;
    float to_end;
};
bool intersect(const in Ray ray, const in Box box, float t_min, float t_max, out HitRange hit_range){
    vec3 dir_recipricol = 1.0f / ray.dir;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    vec3 lb = box.pos - box.dim;
    vec3 rt = box.pos + box.dim;
    float t1 = (lb.x - ray.pos.x)*dir_recipricol.x;
    float t2 = (rt.x - ray.pos.x)*dir_recipricol.x;

    float t3 = (lb.y - ray.pos.y)*dir_recipricol.y;
    float t4 = (rt.y - ray.pos.y)*dir_recipricol.y;

    float t5 = (lb.z - ray.pos.z)*dir_recipricol.z;
    float t6 = (rt.z - ray.pos.z)*dir_recipricol.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
    //    tmin = max(tmin, t_min);
    //    tmax = min(tmax, t_max);
    //what if we are inside?

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    //    if (tmax < 0.0){
    if (tmax < 0.0){


        //set to infinity?
        //        intersection_distance.to_object = tmax;
        return false;
    }
    //return x;
    // if tmin > tmax, ray doesn't intersect AABB
    //what if infinity (doesn't hit?), shoudl still be fine, can only be perpendicular to one?
    if (tmin > tmax){
        //set to infinity?
        //        intersection_distance.to_object = tmax;
        return false;
    }
    if(tmax < t_min){
        return false;
    }
    if(tmin > t_max){
        return false;
    }
    if(tmin < 0.0){
        hit_range.to_object = 0.0;
        //        intersection_distance.to_object = 0.0;
    }else{
        hit_range.to_object = tmin;
        //        intersection_distance.to_object = tmin;
    }
    //don't need  tmax - tmin?
    hit_range.to_end = tmax;
    return true;
}


vec3 endpoint(const in Ray ray, const in HitRange hit_range){
    return endpoint(ray, hit_range.to_object);
}
Ray move_to(const in Ray ray, float t){
    Ray new_ray = ray;
    new_ray.pos = endpoint(ray, t);
    return new_ray;
}

Ray move_to(const in Ray ray, const in HitRange hit_range){
    return move_to(ray, hit_range.to_object);
}

    #endif //RAYTRACINGUTILS_GLSL