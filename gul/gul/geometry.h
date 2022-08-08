//
// Created by Shae Bolt on 6/14/2022.
//

#ifndef VULKANWORKSPACE_GEOMETRY_H
#define VULKANWORKSPACE_GEOMETRY_H

#include <glm/vec3.hpp>
#include "glm/common.hpp"
#include "glm/geometric.hpp"
#include "glm/ext/scalar_constants.hpp"

namespace gul {

    enum class Axis {
        x = 0,
        y = 1,
        z = 2
    };
    template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    Axis operator +(const Axis lhs, T rhs){
        using AxisIntegerType = std::underlying_type_t<Axis>;
        auto lhs_int = static_cast<AxisIntegerType>(lhs);
        return static_cast<Axis>((lhs_int + rhs) % 3);
    }
    template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    Axis operator +(T lhs, const Axis rhs){
        using AxisIntegerType = std::underlying_type_t<Axis>;
        auto rhs_int = static_cast<AxisIntegerType>(rhs);
        return static_cast<Axis>((lhs + rhs_int) % 3);
    }
    template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    Axis operator -(const Axis lhs, T rhs){
        using AxisIntegerType = std::underlying_type_t<Axis>;
        auto lhs_int = static_cast<AxisIntegerType>(lhs);
        return static_cast<Axis>((lhs_int - rhs) % 3);
    }
    template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    Axis operator -(T lhs, const Axis rhs){
        using AxisIntegerType = std::underlying_type_t<Axis>;
        auto rhs_int = static_cast<AxisIntegerType>(rhs);
        return static_cast<Axis>((lhs - rhs_int) % 3);
    }


    template<typename T>
    struct CornerAABB;

    //compute center/origin
    //compute size

    template<typename T>
    struct alignas(alignof(T) * 2) CenterAABB {
        using vec3 = glm::vec<3, T, glm::defaultp>;
        vec3 pos;
        vec3 dim;


        CenterAABB() = default;

        CenterAABB(const vec3 &pos, const vec3 &dim) : pos(pos), dim(dim) {

        }

        explicit CenterAABB(const CornerAABB<T>& aabb) {
            auto corner_aabb = from_min_max(aabb.min(), aabb.max());
            pos = corner_aabb.pos;
            dim = corner_aabb.dim;
        }
        [[nodiscard]]
        static CenterAABB from_min_max(const vec3 &min, const vec3 &max) {
            auto pos = (min + max) / 2;
            auto dim = max - pos;
            return CenterAABB{pos, dim};
        }

        [[nodiscard]]
        static CenterAABB merge(const CenterAABB &a, const CenterAABB &b) {
            return from_min_max(glm::min(a.min(), b.min()),
                                glm::max(a.max(), b.max()));
        }

        [[nodiscard]]
        CornerAABB<T> to_corner_aabb() const{
            return CornerAABB<T>(min(), max() - min());
        }

        [[nodiscard]]
        vec3 min() const {
            return pos - dim;
        }

        [[nodiscard]]
        vec3 max() const {
            return pos + dim;
        }

        [[nodiscard]]
        vec3 center() const {
            return pos;
        }

        vec3 size() const {
            return dim * 2;
        }


        [[nodiscard]]
        T volume() const{
            return dim[0] * 2 * dim[1] * 2 * dim[2] * 2;
        }

        [[nodiscard]]
        Axis min_axis() const {
            if (dim[0] < dim[1]) {
                if (dim[0] < dim[2]) {
                    return Axis::x;
                } else {
                    return Axis::z;
                }
            } else if (dim[1] < dim[2]) {
                return Axis::y;
            } else {
                return Axis::z;
            }
        }

        [[nodiscard]]
        Axis max_axis() const {
            if (dim[0] > dim[1]) {
                if (dim[0] > dim[2]) {
                    return Axis::x;
                } else {
                    return Axis::z;
                }
            } else if (dim[1] > dim[2]) {
                return Axis::y;
            } else {
                return Axis::z;
            }
        }
    };

    template<typename T>
    struct alignas(alignof(T) * 2) CornerAABB {
        using vec3 = glm::vec<3, T, glm::defaultp>;
        vec3 corner;
        vec3 len;


        explicit CornerAABB(const CenterAABB<T>& aabb) : corner(aabb.min()), len(aabb.max - aabb.min()){
        }

        CornerAABB() = default;

        CornerAABB(const vec3 &corner, const vec3 &len) : corner(corner), len(len) {

        }

        [[nodiscard]]
        static CornerAABB from_min_max(const vec3 &min, const vec3 &max) {
            auto corner = min;
            auto len =  max - min;
            return CornerAABB{corner, len};
        }

        [[nodiscard]]
        static CornerAABB merge(const CornerAABB &a, const CornerAABB &b) {
            return from_min_max(glm::min(a.min(), b.min()),
                                glm::max(a.max(), b.max()));
        }

        [[nodiscard]]
        CenterAABB<T> to_center_aabb() const{
            return CenterAABB<T>{center(), len / 2};
        }

        [[nodiscard]]
        vec3 min() const {
            return corner;
        }

        [[nodiscard]]
        vec3 max() const {
            return corner + len;
        }

        [[nodiscard]]
        vec3 center() const {
            return (min() + max()) / 2;
        }

        vec3 size() const {
            return len;
        }

        [[nodiscard]]
        T volume() const{
            return len[0] * len[1] * len[2];
        }

        [[nodiscard]]
        Axis min_axis() const {
            if (len[0] < len[1]) {
                if (len[0] < len[2]) {
                    return Axis::x;
                } else {
                    return Axis::z;
                }
            } else if (len[1] < len[2]) {
                return Axis::y;
            } else {
                return Axis::z;
            }
        }

        [[nodiscard]]
        Axis max_axis() const {
            if (len[0] > len[1]) {
                if (len[0] > len[2]) {
                    return Axis::x;
                } else {
                    return Axis::z;
                }
            } else if (len[1] > len[2]) {
                return Axis::y;
            } else {
                return Axis::z;
            }
        }
    };

    template<typename T>
    struct alignas(alignof(T) * 2) LineSegment {
        using vec3 = glm::vec<3, T, glm::defaultp>;
        glm::vec<3, T, glm::defaultp> a;
        glm::vec<3, T, glm::defaultp> b;
        LineSegment() = default;

        LineSegment(const vec3 &a, const vec3 &b) : a(a), b(b) {
            
        }

        [[nodiscard]]
        CenterAABB<T> to_aabb(){
            return CenterAABB<T>::from_min_max(min(), max());
        }

        [[nodiscard]]
        vec3 min() const {
            return glm::min(a,b);
        }

        [[nodiscard]]
        vec3 max() const {
            return glm::max(a,b);
        }

        [[nodiscard]]
        T length() const{
            return glm::distance(a, b);
        }

        [[nodiscard]]
        Axis min_axis() const {
            auto len = max() - min();
            if (len[0] < len[1]) {
                if (len[0] < len[2]) {
                    return Axis::x;
                } else {
                    return Axis::z;
                }
            } else if (len[1] < len[2]) {
                return Axis::y;
            } else {
                return Axis::z;
            }
        }

        [[nodiscard]]
        Axis max_axis() const {
            auto len = max() - min();
            if (len[0] > len[1]) {
                if (len[0] > len[2]) {
                    return Axis::x;
                } else {
                    return Axis::z;
                }
            } else if (len[1] > len[2]) {
                return Axis::y;
            } else {
                return Axis::z;
            }
        }

    };

    template<typename T>
    struct alignas(alignof(T) * 2) Sphere {
        using vec3 = glm::vec<3, T, glm::defaultp>;
        glm::vec<3, T, glm::defaultp> pos;
        T rad;
        Sphere() = default;

        Sphere(const vec3 &pos, const vec3 &rad) : pos(pos), rad(rad) {

        }

        [[nodiscard]]
        CornerAABB<T> to_aabb(){
            return CenterAABB<T>::from_min_max(min(), max());
        }

        [[nodiscard]]
        vec3 min() const {
            return pos - rad;
        }

        [[nodiscard]]
        vec3 max() const {
            return pos - rad;
        }

        [[nodiscard]]
        vec3 center() const{
            return pos;
        };

        [[nodiscard]]
        vec3 volume() const{
            return (4.0/3.0) * glm::pi<T>() * rad*rad*rad;
        }
    };




    //linesegment to aabb, compare only aabb axis? compare(lhs,rhs,axis)?
    //if overlap, split based on centroid? Split physical LineSegment? Put thing in both?
    // Make box with all functions needed (and corner based box?)
    // in GPU code, check if inside first (distance calc) then used distance to remove checks,
    // then traverse KD tree, check closest distance (using previous distance calc to help to avoid new calculations)
    // use distance squared instead of real distance.
    // save offset in checked point (o-p)
    // actual calc is length(max(abs(p)- dim,0))
    // turns into dot(max(aps(p) - dim),0), max(aps(p) - dim),0));
    // calc closest AABB, use that first.
    // If closest AABB is still out of reach, quit
    // at leaf record distance of actual line.
    // go up the tree and compare AABB's until no more contain a line segment that could be less than the current one,
    // you now have the closest distance


}
#endif //VULKANWORKSPACE_GEOMETRY_H
