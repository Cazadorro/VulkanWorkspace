//
// Created by Shae Bolt on 9/22/2021.
//

#ifndef VULKANWORKSPACE_COLLISION_H
#define VULKANWORKSPACE_COLLISION_H

#include <glm/vec3.hpp>
namespace gul{





    struct Box{
        glm::vec3 pos; //origin
        glm::vec3 dim; //half way dim.
        static Box FromMidPoint(const glm::vec3& mid_point, const glm::vec3& half_dim);
        static Box FromCorner(const glm::vec3& top_left_back_corner, const glm::vec3& full_dim);
    };
    struct Ray{
        glm::vec3 pos;
        glm::vec3 dir;
    };

    bool intersect(const Ray& ray, const Box& box, float &distance);
    bool intersect(const Ray& ray, const Box& box, float &distance, glm::vec3& normal);
}

#endif //VULKANWORKSPACE_COLLISION_H
