#ifndef SDFUTILS_GLSL
#define SDFUTILS_GLSL

#include "geometryutils.glsl"
//https://iquilezles.org/articles/distfunctions/

float calc_distance(const in vec3 point, const in LineSegment line_segment){
    vec3 ba = line_segment.b-line_segment.a;
    vec3 pa = point-line_segment.a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0f, 1.0f );
    vec3  q = pa-h*ba;
    float d = length(q);
    return d;
}

float calc_distance(const in vec3 point, const in Sphere sphere){
    vec3 p  = point - sphere.pos;
    return length(p) - sphere.r;
}

float calc_distance(const in vec3 point, const in CenterAABB bbox){
    vec3 p = point - bbox.pos;
    vec3 q = abs(p) - bbox.dim;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}




#endif //REFTYPES_GLSL