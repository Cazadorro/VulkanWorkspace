//
// Created by Shae Bolt on 2/2/2022.
//

#ifndef VULKANWORKSPACE_BVH_H
#define VULKANWORKSPACE_BVH_H
#include <glm/vec3.hpp>
#include <gsl/span>
#include <vector>

namespace vul{
    enum class Axis{
        x =0,
        y =1,
        z =2
    };
    struct alignas(16) Sphere {
        glm::vec3 pos;
        float r;
        Sphere() = default;
        Sphere(const glm::vec3& pos, float r);
    };
    static_assert(sizeof(Sphere) == 16);

    struct alignas(16) PathEnd {
        glm::vec3 pos;
        float speed;
        PathEnd() = default;
        PathEnd(const glm::vec3& pos, float speed);
    };
    static_assert(sizeof(PathEnd) == 16);

    struct alignas(8) Box{
        glm::vec3 pos;
        glm::vec3 dim;
        [[nodiscard]]
        glm::vec3 min() const;
        [[nodiscard]]
        glm::vec3 max() const;

        Box() = default;
        Box(const glm::vec3& pos, const glm::vec3& dim);
        explicit Box(const Sphere& sphere);
        Box(const Box& a, const glm::vec3& b);
        Box(const Box& a, const Box& b);
        Box(const Sphere& sphere_begin, const PathEnd& path_end);
        [[nodiscard]]
        static Box from_corners(const glm::vec3& min, const glm::vec3& max);
        [[nodiscard]]
        Axis min_axis() const;
        [[nodiscard]]
        Axis max_axis() const;
    };
    static_assert(sizeof(Box) == 24);



    struct alignas(8) BVHNode{
        Box bbox;
        std::uint32_t lhs_idx;
        std::uint32_t rhs_idx;
    };
    static_assert(sizeof(BVHNode) == 32);
    int random_int(int min, int max);

    struct FlatBVH{
        std::vector<BVHNode> nodes;
        std::vector<std::uint32_t> leaves;
        std::vector<std::uint32_t> parents;
    };
    FlatBVH create_bvh(const gsl::span<Sphere>& host_sphere_data,
             const gsl::span<PathEnd>& host_path_data);
    void fill_nodes(const gsl::span<Sphere>& host_sphere_data,
             const gsl::span<PathEnd>& host_path_data,
             const gsl::span<std::uint32_t>& idxs,
             std::vector<BVHNode>& nodes,
          std::vector<std::uint32_t>& leaves,
          std::vector<std::uint32_t>& parents,
          std::uint32_t parent);
}

#endif //VULKANWORKSPACE_BVH_H
