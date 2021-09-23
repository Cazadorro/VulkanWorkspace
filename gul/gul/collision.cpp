//
// Created by Shae Bolt on 9/22/2021.
//

#include "gul/collision.h"
#include <glm/glm.hpp>


bool gul::intersect(const gul::Ray &ray, const gul::Box &box, float &distance) {
    assert(ray.dir != glm::vec3(0.0f));
    glm::vec3 dir_recipricol = 1.0f / ray.dir;
    //min corner is lower left hand back corner
    //max corner is upper right hand front corner
    glm::vec3 min_corner = box.pos - box.dim;
    glm::vec3 max_corner = box.pos + box.dim;

    float t1 = (min_corner.x - ray.pos.x) * dir_recipricol.x;
    float t2 = (max_corner.x - ray.pos.x) * dir_recipricol.x;

    float t3 = (min_corner.y - ray.pos.y) * dir_recipricol.y;
    float t4 = (max_corner.y - ray.pos.y) * dir_recipricol.y;

    float t5 = (min_corner.z - ray.pos.z) * dir_recipricol.z;
    float t6 = (max_corner.z - ray.pos.z) * dir_recipricol.z;

    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)),
                          glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)),
                          glm::max(t5, t6));
    //what if we are inside?

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0.0) {
        //set to infinity?
        distance = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    //what if infinity (doesn't hit?), shoudl still be fine, can only be perpendicular to one?
    if (tmin > tmax) {
        distance = std::numeric_limits<float>::infinity();
        return false;
    }

    if (tmin < 0.0) {
//            tmin = 0.0;
    }
    distance = tmin;
    //dist_thru_cube = tmax - tmin;
    return true;
}

bool gul::intersect(const gul::Ray &ray, const gul::Box &box, float &distance,
                    glm::vec3 &normal) {
    assert(ray.dir != glm::vec3(0.0f));
    glm::vec3 dir_recipricol = 1.0f / ray.dir;
    //min corner is lower left hand back corner
    //max corner is upper right hand front corner
    glm::vec3 min_corner = box.pos - box.dim;
    glm::vec3 max_corner = box.pos + box.dim;

    float t1 = (min_corner.x - ray.pos.x) * dir_recipricol.x;
    float t2 = (max_corner.x - ray.pos.x) * dir_recipricol.x;

    float t3 = (min_corner.y - ray.pos.y) * dir_recipricol.y;
    float t4 = (max_corner.y - ray.pos.y) * dir_recipricol.y;

    float t5 = (min_corner.z - ray.pos.z) * dir_recipricol.z;
    float t6 = (max_corner.z - ray.pos.z) * dir_recipricol.z;

    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)),
                          glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)),
                          glm::max(t5, t6));
    //what if we are inside?

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0.0) {
        //set to infinity?
        distance = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    //what if infinity (doesn't hit?), shoudl still be fine, can only be perpendicular to one?
    if (tmin > tmax) {
        distance = std::numeric_limits<float>::infinity();
        return false;
    }

    if (tmin < 0.0) {
//            tmin = 0.0;
    }
    distance = tmin;
    //dist_thru_cube = tmax - tmin;
    if (distance == t1) {
        normal = glm::vec3(-1.0,0.0,0.0);
    } else if (distance == t2) {
        normal = glm::vec3(1.0,0.0,0.0);
    } else if (distance == t3) {
        normal = glm::vec3(0.0,-1.0,0.0);
    } else if (distance == t4) {
        normal = glm::vec3(0.0,1.0,0.0);
    } else if (distance == t5) {
        normal = glm::vec3(0.0,0.0,-1.0);
    } else { //if(distance == t6){
        normal = glm::vec3(0.0,0.0,1.0);
    }
    return true;
}

gul::Box
gul::Box::FromMidPoint(const glm::vec3 &mid_point, const glm::vec3 &half_dim) {
    return gul::Box{mid_point, half_dim};
}

gul::Box gul::Box::FromCorner(const glm::vec3 &top_left_back_corner,
                              const glm::vec3 &full_dim) {
    return gul::Box{top_left_back_corner + full_dim/2.0f, full_dim/2.0f};
}
