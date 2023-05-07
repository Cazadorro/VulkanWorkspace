#ifndef GEOMETRYUTILS_GLSL
#define GEOMETRYUTILS_GLSL

struct LineSegment{
    vec3 a;
    vec3 b;
};

struct Sphere{
    vec3 pos;
    float r;
};

struct CenterAABB{
    vec3 pos;
    vec3 dim;
};

struct CornerAABB{
    vec3 pos;
    vec3 len;
};

CenterAABB toCenterAABB(CornerAABB corner){
    CenterAABB center;
    center.pos = corner.pos + (corner.len) / 2.0f;
    center.dim = corner.len / 2.0;
    return center;
}

void split(const in CenterAABB bbox, uint index, float divisor,
           out CenterAABB lhs_bbox, out CenterAABB rhs_bbox){
    lhs_bbox = bbox;
    rhs_bbox = bbox;
    float lhs_dim_min = (bbox.pos[index] - bbox.dim[index]);
    float lhs_dim_pos = (divisor + lhs_dim_min) / 2;
    float lhs_dim_dim = (divisor - lhs_dim_min) / 2;
    lhs_bbox.pos[index] = lhs_dim_pos;
    lhs_bbox.dim[index] = lhs_dim_dim;
    float rhs_dim_min = lhs_dim_pos + lhs_dim_dim;
    float rhs_dim_max = bbox.pos[index] + bbox.dim[index];
    rhs_bbox.pos[index] = (rhs_dim_max + rhs_dim_min) / 2;
    rhs_bbox.dim[index] = (rhs_dim_max - rhs_dim_min) / 2;
}

#endif //GEOMETRYUTILS_GLSL