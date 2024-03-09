//
// Created by Shae Bolt on 6/14/2022.
//

#ifndef VULKANWORKSPACE_KDTREE_H
#define VULKANWORKSPACE_KDTREE_H

#include "gul/geometry.h"
#include <glm/vec3.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/range/conversion.hpp>
#include <gsl/span>
#include <functional>
#include <array>
#include <limits>

#include <czdr/utility/assert.h>


namespace gul {

    enum class KDTreeIndexType {
        AxisX = 0,
        AxisY = 1,
        AxisZ = 2,
        Leaf = 3
    };

    constexpr KDTreeIndexType to_kdtree_index_type(Axis axis){
        switch(axis){
            case Axis::x:
                return KDTreeIndexType::AxisX;
                break;
            case Axis::y:
                return KDTreeIndexType::AxisY;
                break;
            case Axis::z:
                return KDTreeIndexType::AxisZ;
                break;
        }
    }

    struct KDTreeIndexPair {


        constexpr KDTreeIndexPair() : m_raw_bits(0) {

        }
        constexpr KDTreeIndexPair(KDTreeIndexType index_type,
                                  std::uint32_t lhs_index,
                                  std::uint32_t rhs_index) : m_raw_bits(0){
            m_raw_bits |= static_cast<std::uint64_t>(index_type) << 62u;
            m_raw_bits |= (static_cast<std::uint64_t>(lhs_index) & m_bitmask_31) << 31;
            m_raw_bits |= (static_cast<std::uint64_t>(rhs_index) & m_bitmask_31);

        }
        constexpr KDTreeIndexPair(Axis axis,
                                  std::uint32_t lhs_index,
                                  std::uint32_t rhs_index) : KDTreeIndexPair(to_kdtree_index_type(axis), lhs_index, rhs_index) {

        }

        [[nodiscard]]
        constexpr KDTreeIndexType get_index_type() const {
            auto index_type = m_raw_bits >> 62;
            return static_cast<KDTreeIndexType>(index_type);
        }
        [[nodiscard]]
        constexpr std::uint32_t get_lhs_index() const {
            auto lhs_index = (m_raw_bits >> 31) & m_bitmask_31;
            return static_cast<std::uint32_t>(lhs_index);
        }
        [[nodiscard]]
        constexpr std::uint32_t get_rhs_index() const {
            auto rhs_index = m_raw_bits & m_bitmask_31;
            return static_cast<std::uint32_t>(rhs_index);
        }

        constexpr void set_index_type(KDTreeIndexType index_type) {
            m_raw_bits = (m_raw_bits & m_bitmask_62) | (static_cast<std::uint64_t>(index_type) << 62u);
        }
        constexpr void set_lhs_index(std::uint32_t lhs_index) {
            m_raw_bits = (m_raw_bits & ~(m_bitmask_31 << 31)) | (static_cast<std::uint64_t>(lhs_index) << 31u);
        }
        constexpr void set_rhs_index(std::uint32_t rhs_index) {
            m_raw_bits = (m_raw_bits & ~(m_bitmask_31)) | (static_cast<std::uint64_t>(rhs_index));
        }
    private:
        static constexpr std::uint64_t m_bitmask_31 = 0x0000'0000'7FFF'FFFF;
        static constexpr std::uint64_t m_bitmask_62 = 0x3FFF'FFFF'FFFF'FFFF;
        std::uint64_t m_raw_bits;
    };

    template<typename T>
    class KDTree {
    public:
        using predicte_type = std::function<bool(const LineSegment<T> &,
                                                 const LineSegment<T> &)>;

        void construct_tree(const std::vector<LineSegment<T>> &current_segments,
                            gul::Axis current_axis,
                            const std::array<predicte_type, 3> &predicates,
                            std::vector<KDTreeIndexPair>& last_indexes, std::vector<std::uint32_t>& parent_indexes) {
            CZDR_ASSERT(!current_segments.empty());

            if (current_segments.size() <= 4) {
                std::uint32_t lhs_leafs = m_leafs.size();

                //Need to push leaf indexes to the end.
                //TODO also need to make sure things that point to this are accounted for?
                //mark the previous ? then go back and add value plus current index? n
                m_leafs = ranges::views::concat(m_leafs, current_segments) |
                          ranges::to<std::vector>();

                std::uint32_t rhs_leafs = m_leafs.size();
                last_indexes.push_back(KDTreeIndexPair{KDTreeIndexType::Leaf, lhs_leafs, rhs_leafs});
                std::uint32_t last_index = (m_indexes.size() - 1) * 2;
                if(m_indexes.back().get_rhs_index() == 0xFFFFFFFF){
                    parent_indexes.push_back(last_index + 1);
                }else{
                    parent_indexes.push_back(last_index);
                }

            } else {
                auto middle = (current_segments.size() - 1) / 2;
                auto median = current_segments[middle];
                auto next_axis = (current_axis + 1);
                m_divisors.push_back(median.max()[static_cast<std::int32_t>(current_axis)]);
                auto left_objects = std::vector(current_segments.begin(),
                                                current_segments.begin() +
                                                middle) | ranges::actions::sort(
                        predicates[static_cast<std::int32_t>(next_axis)]);
                auto right_objects =
                        std::vector(current_segments.begin() + middle,
                                    current_segments.end()) |
                        ranges::actions::sort(
                                predicates[static_cast<std::int32_t>(next_axis)]);

                auto left_index_handle = m_indexes.size();


                auto current_index_handle = m_indexes.size();
                m_indexes.push_back(KDTreeIndexPair{current_axis,0xFFFFFFFF,0xFFFFFFFF});
                std::uint32_t lhs_index = m_indexes.size();
                m_indexes[current_index_handle].set_lhs_index(lhs_index);
                construct_tree(left_objects, next_axis, predicates, last_indexes, parent_indexes);
                std::uint32_t rhs_index = m_indexes.size();

                m_indexes[current_index_handle].set_rhs_index(rhs_index);
                construct_tree(right_objects, next_axis, predicates, last_indexes, parent_indexes);
                // need to add divisors, = segment max on axis.
                // need to add indexes?
            }
        }

        explicit KDTree(const gsl::span<LineSegment<T>> &objects) {
            CZDR_ASSERT(!objects.empty() && objects.size() <
                                           std::numeric_limits<std::int32_t>::max());
            predicte_type predicate_x =
                    [](const LineSegment<T> &lhs, const LineSegment<T> &rhs) {
                        return lhs.max().x < rhs.max().x;
                    };
            predicte_type predicate_y =
                    [](const LineSegment<T> &lhs, const LineSegment<T> &rhs) {
                        return lhs.max().y < rhs.max().y;
                    };
            predicte_type predicate_z =
                    [](const LineSegment<T> &lhs, const LineSegment<T> &rhs) {
                        return lhs.max().z < rhs.max().z;
                    };

            std::array<predicte_type, 3> predicates = {predicate_x,
                                                       predicate_y,
                                                       predicate_z};
            auto copied_objects = objects | ranges::to<std::vector>();
            ranges::actions::sort(copied_objects,
                                  predicates[static_cast<std::int32_t>(gul::Axis::x)]);


            m_divisors.reserve(objects.size() + objects.size() - 1);
            m_indexes.reserve(objects.size() + objects.size() - 1);
            m_leafs.reserve(objects.size());
            std::vector<KDTreeIndexPair> last_indexes;
            std::vector<std::uint32_t> parent_indexes;
            construct_tree(copied_objects, gul::Axis::x, predicates, last_indexes, parent_indexes);

            for(std::uint32_t i = 0; i < parent_indexes.size(); ++i){
                auto parent_node_index = parent_indexes[i];
                auto parent_index = parent_node_index / 2;

                if(parent_node_index % 2 == 0){
                    m_indexes[parent_index].set_lhs_index(m_indexes.size() + i);
                }else{
                    m_indexes[parent_index].set_rhs_index(m_indexes.size() + i);
                }

            }
            m_indexes = ranges::views::concat(m_indexes, last_indexes) |
                      ranges::to<std::vector>();

            m_aabb = objects.front().to_aabb();
            for (const auto &object: objects | ranges::views::drop(0)) {
                m_aabb = gul::CenterAABB<T>::merge(m_aabb, object.to_aabb());
            }

        }

        [[nodiscard]]
        const gul::CenterAABB<T> &get_aabb() const{
            return m_aabb;
        }

        [[nodiscard]]
        const std::vector<T> &get_divisors() const{
            return m_divisors;
        }

        [[nodiscard]]
        const std::vector<KDTreeIndexPair> &get_indexes() const{
            return m_indexes;
        }

        [[nodiscard]]
        const std::vector<LineSegment<T>> &get_leafs() const{
            return m_leafs;
        }


    private:
        gul::CenterAABB<T> m_aabb;
        std::vector<T> m_divisors;
        std::vector<KDTreeIndexPair> m_indexes;
        std::vector<LineSegment<T>> m_leafs;
    };

} // gul

#endif //VULKANWORKSPACE_KDTREE_H
