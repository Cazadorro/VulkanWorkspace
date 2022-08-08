//
// Created by Shae Bolt on 6/14/2022.
//

#ifndef VULKANWORKSPACE_KDTREE_H
#define VULKANWORKSPACE_KDTREE_H

#include "gul/geometry.h"
#include <vul/vkassert.h>
#include <glm/vec3.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/concat.hpp>
#include <span>
#include <functional>
#include <array>
#include <limits>

namespace gul {

    template<typename T>
    class KDTree {
    public:
        using predicte_type = std::function<void(const LineSegment<T> &,
                                                 const LineSegment<T> &)>;
        void construct_tree(const std::vector<LineSegment<T>>& current_segments, gul::Axis current_axis, const std::array<predicte_type, 3>& predicates){
            VUL_ASSERT(!current_segments.empty());

            if(current_segments.size() <= 4){
                m_indexes.push_back(m_leafs.size() & 0b10000000'00000000'00000000'00000000);
                m_leafs = ranges::views::concat(m_leafs, current_segments);
                m_indexes.push_back(m_leafs.size());
            }else{
                auto middle = (current_segments.size() - 1) / 2;
                auto median = current_segments[middle];
                auto next_axis = (current_axis + 1);
                auto left_objects = std::vector(current_segments.begin(), current_segments.begin() + middle) | ranges::actions::sort(predicates[next_axis]);
                auto right_objects = std::vector(current_segments.begin() + middle, current_segments.enb()) | ranges::actions::sort(predicates[next_axis]);
                m_divisors.push_back(median.max()[current_axis]);
                auto left_index_handle = m_indexes.size();
                m_indexes.push_back(m_indexes.size());
                auto right_index_handle = m_indexes.size();
                m_indexes.push_back(0);
                construct_tree(left_objects, next_axis, predicates);
                m_indexes[right_index_handle] = m_indexes.size();
                construct_tree(right_objects, next_axis, predicates);
                // need to add divisors, = segment max on axis.
                // need to add indexes?
            }
        }

        KDTree(const std::span<LineSegment<T>> &objects) {
            VUL_ASSERT(!objects.empty() && objects.size() < std::numeric_limits<std::int32_t>::max());
            auto predicate_x =
                    [](const LineSegment<T> &lhs, const LineSegment<T> &rhs) {
                        return lhs.max().x < rhs.max().x;
                    };
            auto predicate_y =
                    [](const LineSegment<T> &lhs, const LineSegment<T> &rhs) {
                        return lhs.max().y < rhs.max().y;
                    };
            auto predicate_z =
                    [](const LineSegment<T> &lhs, const LineSegment<T> &rhs) {
                        return lhs.max().z < rhs.max().z;
                    };

            std::array<predicte_type, 3> predicates = {predicate_x,
                                                        predicate_y,
                                                       predicate_z};
            std::vector<LineSegment<T>> copied_objects = objects | ranges::actions::sort(predicates[gul::Axis::x]);


            std::vector<std::vector<LineSegment<T>>> segments_stack;
            std::vector<gul::Axis> axis_stack;

            construct_tree(copied_objects, gul::Axis::x, predicates);
            m_divisors.reserve(objects.size() + objects.size() - 1);
            m_indexes.reserve((objects.size() + objects.size() - 1) * 2);
            m_leafs.reserve(objects.size());
        }



    private:
        std::vector<float> m_divisors;
        std::vector<std::uint32_t> m_indexes;
        std::vector<T> m_leafs;
    };

} // gul

#endif //VULKANWORKSPACE_KDTREE_H
