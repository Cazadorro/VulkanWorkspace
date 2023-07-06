//
// Created by Bolt on 6/25/2023.
//

#ifndef VULKANWORKSPACE_INDEXINGT_H
#define VULKANWORKSPACE_INDEXINGT_H

#include <glm/vec2.hpp>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <optional>

namespace uul {


    template<typename T>
    struct dim2d{
        T width;
        T height;
        dim2d(T width, T height) : width(width), height(height){}
        [[nodiscard]]
        T area() const{
            return width * height;
        }
        template<typename U>
        [[nodiscard]]
        bool is_within(const glm::vec<2, U>& pos){
            return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
        }
    };

    template<std::size_t N = 0>
    struct Index2D {
        std::size_t width;
        std::size_t height;

        Index2D(std::size_t width, std::size_t height) : width(width), height(height) {};

        std::size_t to_linear(std::size_t x, std::size_t y) {
            return y * width + x;
        }

        std::size_t to_linear_clamp(std::int64_t x, std::int64_t y) {
            return std::clamp(y, 0ll, static_cast<std::int64_t>(height)) * width +
                   std::clamp(x, 0ll, static_cast<std::int64_t>(width));
        }

        std::optional<std::size_t> to_linear_check(std::int64_t x, std::int64_t y) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                return to_linear(x, y);
            }
            return std::nullopt;
        }
    };

//    template<typename ReturnType_T, typename ... Args>
//    ReturnType_T func(Args const & ... args){
//        constexpr auto arg_count = sizeof...(Args);
//
//        /* sizeof...(Args) will give you the number of arguments */
//    }
}
#endif //VULKANWORKSPACE_INDEXINGT_H
