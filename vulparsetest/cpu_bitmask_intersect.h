//
// Created by Shae Bolt on 9/20/2021.
//

#ifndef VULKANWORKSPACE_CPU_BITMASK_INTERSECT_H
#define VULKANWORKSPACE_CPU_BITMASK_INTERSECT_H
#include <gul/bitmask.h>
#include <glm/vec3.hpp>
#include <cstdint>
bool bitmask_intersect(const gul::bitmask& bitmask, glm::vec3 orig, glm::vec3 dir, glm::vec3 block_offset, std::uint32_t& voxel_index);


#endif //VULKANWORKSPACE_CPU_BITMASK_INTERSECT_H
