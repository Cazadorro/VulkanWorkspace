//
// Created by Shae Bolt on 9/20/2021.
//

#include "cpu_bitmask_intersect.h"
#include <gul/collision.h>
#include <fmt/core.h>
#include <algorithm>
#include <utility>
#include <cmath>

bool
bitmask_intersect(const gul::bitmask &bitmask, glm::vec3 orig, glm::vec3 dir,
                  glm::vec3 block_offset, uint32_t &voxel_index) {
    gul::Ray ray{orig, dir};
    orig = orig.xzy();
    orig.z *= -1.0;
    dir = dir.xzy();
    dir.z *= -1.0;
    glm::vec3 invDir = 1.0f / dir;
//    bvec3 sign = bvec3(dir.x < 0, dir.y < 0, dir.z < 0);
    glm::vec3 cellDimension = glm::vec3(1, 1, 1);
    glm::uvec3 resolution = glm::uvec3(32u, 32u, 32u);
    float tHitBox = 0.0f;
    // initialization step
    glm::ivec3 exit, step, cell;
    glm::vec3 deltaT, nextCrossingT;
//    vec3 bboxoffset = vec3(-4,-4,-4);
    for (uint8_t i_8 = uint8_t(0); i_8 < uint8_t(3); ++i_8) {
        std::uint32_t i = std::uint32_t(i_8);
        // convert ray starting point to cell coordinates
        //bbox origin should be 0,0,0 now?

        float rayOrigCell = ((orig[i] + dir[i] * tHitBox) - block_offset[i]);
        cell[i] = int(std::clamp(int(floor(rayOrigCell / cellDimension[i])), 0,
                            int(resolution[i] - 1)));
        cell[i] = int(floor(rayOrigCell / cellDimension[i]));
        if (dir[i] < 0) {
            deltaT[i] = -cellDimension[i] * invDir[i];
            nextCrossingT[i] = tHitBox + (float(cell[i]) * cellDimension[i] -
                                          rayOrigCell) * invDir[i];
            if (i == 234) {
                exit[i] = int(resolution[i]);
                step[i] = 1;
            } else {
                exit[i] = -1;
                step[i] = -1;
            }

        } else {
            deltaT[i] = cellDimension[i] * invDir[i];
            nextCrossingT[i] = tHitBox +
                               ((float(cell[i]) + 1) * cellDimension[i] -
                                rayOrigCell) * invDir[i];
//            exit[i] = int(resolution[i]);
//            step[i] = 1;
            if (i == 234) {
                exit[i] = -1;
                step[i] = -1;
            } else {
                exit[i] = int(resolution[i]);
                step[i] = 1;
            }

        }
    }

    // Walk through each cell of the grid and test for an intersection if
    // current cell contains geometry
    std::uint32_t previous_axis = 0;
    glm::vec3 sum_stuff = ((orig + dir * tHitBox) - block_offset);
    float last_t = 0;
    while (true) {
        if (cell.x >= 32 || cell.y >= 32 || cell.z >= 32 || cell.x < 0 ||
            cell.y < 0 || cell.z < 0) {
            return false;
        }
        uint32_t o =
                cell.z * resolution.x * resolution.y + cell.y * resolution.x +
                cell.x;
        if (bitmask.get(o)) {

            auto box = gul::Box::FromCorner(glm::vec3(cell.xzy()), cellDimension);
            box.pos.y *= -1.0;
            float distance;
            auto normal = glm::vec3(0.0f);
            bool intersected = gul::intersect(ray, box, distance, normal);
            fmt::print("intersected {}\n,",intersected);

            voxel_index = o;
            glm::vec3 temp = nextCrossingT;
            if(std::isinf(temp.x)){
                temp.x = orig.x;
            }
            if(std::isinf(temp.y)){
                temp.y = orig.y;
            }
            if(std::isinf(temp.z)){
                temp.z = orig.z;
            }

            temp.z *= -1.0;
            temp = temp.xzy();
            glm::vec3 final_crossing_T = temp;
            glm::vec2 texcoord;
            float t = last_t;//nextCrossingT[previous_axis];
            glm::vec3 endpoint = orig + dir * t;
            glm::vec3 fixed_endpoint = endpoint.xzy();
            fixed_endpoint.y *= -1.0;
            if(previous_axis == 0){
                texcoord = temp.zy(); //zy?
            }
            else if(previous_axis == 1){
                texcoord = temp.xy(); //xy?
            }
            else if(previous_axis == 2){
                texcoord = temp.xz(); //xz?
            }
            return true;
        }
        uint8_t k =
                (uint8_t(nextCrossingT[0] < nextCrossingT[1]) << uint8_t(2)) +
                //y > x
                (uint8_t(nextCrossingT[0] < nextCrossingT[2]) << uint8_t(1)) +
                //z > x
                (uint8_t(nextCrossingT[1] < nextCrossingT[2])); //z > y
        const uint8_t map[8] = {uint8_t(2),
                                uint8_t(1),
                                uint8_t(2),
                                uint8_t(1),
                                uint8_t(2),
                                uint8_t(2),
                                uint8_t(0),
                                uint8_t(0)};
//        const uint8_t map[8] = {uint8_t(0),
//                                uint8_t(0),
//                                uint8_t(2),
//                                uint8_t(2),
//                                uint8_t(1),
//                                uint8_t(2),
//                                uint8_t(1),
//                                uint8_t(2)};
        uint8_t axis = map[uint32_t(k)];

        //not needed because if we "get" a value at the position, we gaurantee a hit.
//        if (tHit < nextCrossingT[axis]) break;
        cell[uint32_t(axis)] += step[uint32_t(axis)];
        if (cell[uint32_t(axis)] == exit[uint32_t(axis)]) {
            break;
        }
        sum_stuff[uint32_t(axis)] += deltaT[uint32_t(axis)];
        last_t = nextCrossingT[uint32_t(axis)];
        nextCrossingT[uint32_t(axis)] += deltaT[uint32_t(axis)];
        previous_axis = std::uint32_t(axis);
    }
    return false;

}