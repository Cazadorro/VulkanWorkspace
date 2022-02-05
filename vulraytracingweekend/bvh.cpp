//
// Created by Shae Bolt on 2/2/2022.
//

#include "bvh.h"
#include <vul/vkassert.h>
#include <glm/common.hpp>
#include <range/v3/view/zip.hpp>
#include <random>
#include <limits>
#include <algorithm>
#include <numeric>
vul::Sphere::Sphere(const glm::vec3 &pos, float r) : pos(pos), r(r) {

}

vul::PathEnd::PathEnd(const glm::vec3 &pos, float speed): pos(pos), speed(speed) {

}

glm::vec3 vul::Box::min() const {
    return {pos - dim};
}

glm::vec3 vul::Box::max() const {
    return {pos + dim};
}

vul::Box::Box(const glm::vec3 &pos, const glm::vec3 &dim) : pos(pos), dim(dim) {

}
vul::Box::Box(const vul::Sphere &sphere): pos(sphere.pos), dim(glm::vec3(sphere.r)){

}

vul::Box::Box(const vul::Box &a, const glm::vec3& b): pos((a.pos + b)/2.0f), dim(glm::abs(pos - a.pos) + a.dim){

}

vul::Box::Box(const vul::Box &a, const vul::Box &b):
    pos( (a.pos + b.pos) / 2.0f),
    dim( (glm::max(a.max(), b.max()) - glm::min(a.min(), b.min()))/2.0f ){

}

vul::Box::Box(const vul::Sphere &sphere_begin, const vul::PathEnd &path_end) {
    Box box;
    if(path_end.speed == 0.0){
        box = Box(sphere_begin);
    }else{
        Box sphere_box(sphere_begin);
        box = Box(sphere_box, path_end.pos);
    }
    pos = (box.pos);
    dim = (box.dim);
}

vul::Box vul::Box::from_corners(const glm::vec3 &min, const glm::vec3 &max) {
    glm::vec3 center = (min + max)/ 2.0f;
    glm::vec3 dim = (max - min) / 2.0f;
    return vul::Box(center, dim);
}

vul::Axis vul::Box::min_axis() const {
    if(dim.x < dim.y){
        if(dim.x < dim.z){
            return vul::Axis::x;
        }
        else{
            return vul::Axis::z;
        }
    }else if(dim.y < dim.z){
        return vul::Axis::y;
    }else{
        return vul::Axis::z;
    }
}

vul::Axis vul::Box::max_axis() const {
    if(dim.x > dim.y){
        if(dim.x > dim.z){
            return vul::Axis::x;
        }
        else{
            return vul::Axis::z;
        }
    }else if(dim.y > dim.z){
        return vul::Axis::y;
    }else{
        return vul::Axis::z;
    }
}

double vul::Box::volume() const {
    VUL_ASSERT(dim.x >= 0.0);
    VUL_ASSERT(dim.y >= 0.0);
    VUL_ASSERT(dim.z >= 0.0);
    return dim.x * dim.y * dim.z;
}


int vul::random_int(int min, int max) {
    std::random_device dev;
    static std::uniform_int_distribution<std::int32_t> distribution(min, max);
    static std::mt19937 generator(dev());
    return distribution(generator);
}


bool box_axis_compare(const vul::Box& lhs, const vul::Box& rhs, const vul::Axis axis){
//    return lhs.pos[static_cast<int>(axis)] < rhs.pos[static_cast<int>(axis)];
    return lhs.min()[static_cast<int>(axis)] < rhs.min()[static_cast<int>(axis)];
}

void vul::fill_nodes(
        const gsl::span<Sphere> &host_sphere_data,
        const gsl::span<PathEnd> &host_path_data,
        const gsl::span<std::uint32_t>& idxs,
        std::vector<Box>& bboxes,
        std::vector<BVHChildren>& children,
        std::vector<std::uint32_t>& leaves,
        std::vector<std::uint32_t>& parents,
        std::uint32_t parent){
    glm::vec3 min_corner(std::numeric_limits<float>::infinity());
    glm::vec3 max_corner(-std::numeric_limits<float>::infinity());
    for(auto idx: idxs){
        const auto& sphere_data = host_sphere_data[idx];
        const auto& path_data = host_path_data[idx];
        Box box(sphere_data, path_data);
        min_corner = glm::min(box.min(), min_corner);
        max_corner = glm::max(box.max(), max_corner);
    }
    Box total_bbox = Box::from_corners(min_corner, max_corner);
    auto spitting_axis = total_bbox.min_axis();
    spitting_axis = static_cast<vul::Axis>(random_int(0,2));
    spitting_axis = random_xz();
    std::sort(idxs.begin(), idxs.end(), [&](const auto& lhs, const auto& rhs){
        auto lhs_bbox = Box(host_sphere_data[lhs], host_path_data[lhs]);
        auto rhs_bbox = Box(host_sphere_data[rhs], host_path_data[rhs]);
        return box_axis_compare(lhs_bbox, rhs_bbox, spitting_axis);
    });



    const std::uint32_t min_size = 16;
    if(idxs.size() > min_size) {
        auto result_itr = std::max_element(idxs.begin(), idxs.end(), [&](const auto& lhs, const auto& rhs){
            auto lhs_bbox = Box(host_sphere_data[lhs], host_path_data[lhs]);
            auto rhs_bbox = Box(host_sphere_data[rhs], host_path_data[rhs]);
            return lhs_bbox.volume() < rhs_bbox.volume() ;
        });
        auto result_idx = *result_itr;
        auto result_box = Box(host_sphere_data[result_idx], host_path_data[result_idx]);
        //if volume takes up more than 75% of space, split up
        auto lhs_size = idxs.size() / 2;
        VUL_ASSERT(result_box.volume() < total_bbox.volume()); //becarefull once, iddin't work?
        if((result_box.volume() / total_bbox.volume()) > 0.99){
            //set to one so we can still use the rest of the function exactly the same.
            lhs_size = 1;
            //move the largest element to the begining
            std::swap(*result_itr, *idxs.begin());
        }
        auto rhs_size = idxs.size() - lhs_size;
        auto current_idx = bboxes.size();
        bboxes.push_back(total_bbox);
        children.push_back({static_cast<std::uint32_t>(current_idx+1),0});
        parents.push_back(parent);
        fill_nodes(host_sphere_data, host_path_data,gsl::span<std::uint32_t>(idxs.data(), lhs_size), bboxes, children, leaves, parents, current_idx);
        auto rhs_idx = static_cast<std::uint32_t>(bboxes.size());
        fill_nodes(host_sphere_data, host_path_data, gsl::span<std::uint32_t>(idxs.data() + lhs_size, rhs_size), bboxes, children, leaves, parents, current_idx);
        children[current_idx].rhs_idx = rhs_idx;
    }else{
        auto leaf_position = static_cast<std::uint32_t>(leaves.size());
        auto leaf_size = static_cast<std::uint32_t>(idxs.size());
        auto end_flag = 0b1000'0000'0000'0000'0000'0000'0000'0000;
        bboxes.push_back(total_bbox);
        children.push_back({leaf_size | end_flag,
                            leaf_position});
        parents.push_back(parent);
        for(auto idx : idxs){
            leaves.push_back({idx});
        }
    }
}
vul::FlatBVH vul::create_bvh(const gsl::span<Sphere> &host_sphere_data,
              const gsl::span<PathEnd> &host_path_data) {
    VUL_ASSERT(!host_sphere_data.empty() && !host_sphere_data.empty());
    VUL_ASSERT(host_path_data.size() == host_path_data.size());

    std::vector<std::uint32_t> idxs(host_sphere_data.size());
    std::iota(idxs.begin(), idxs.end(), 0);

    FlatBVH bvh;
    fill_nodes(host_sphere_data, host_path_data, idxs,bvh.bboxes, bvh.children, bvh.leaves, bvh.parents, 0xFFFFFFFFu);

    return bvh;
}

vul::Axis vul::random_axis() {
    return static_cast<vul::Axis>(random_int(0,2));
}

vul::Axis vul::random_xz() {
    auto value = (random_int(0,1));
    return value == 0 ? Axis::x : Axis::z;
}


