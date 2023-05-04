//
// Created by Shae Bolt on 9/3/2021.
//

#ifndef VULKANWORKSPACE_VKSTRUCTUTILS_H
#define VULKANWORKSPACE_VKSTRUCTUTILS_H

#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {

    struct Extent2D;
    struct Extent3D{
        std::uint32_t    width;
        std::uint32_t    height;
        std::uint32_t    depth;
        Extent3D() = default;
        explicit Extent3D(const VkExtent3D& extent);
        Extent3D(std::uint32_t width, std::uint32_t height, std::uint32_t depth);
        [[nodiscard]]
        static Extent3D fromDim(std::uint32_t dim);
        operator VkExtent3D() const;
        [[nodiscard]]
        VkExtent3D &get();
        [[nodiscard]]
        const VkExtent3D &get() const;
        [[nodiscard]]
        Extent3D to3D() const;
        [[nodiscard]]
        Extent2D to2D() const;
        [[nodiscard]]
        std::uint32_t to1D() const;
    };

    struct Extent2D{
        std::uint32_t    width;
        std::uint32_t    height;
        Extent2D() = default;
        explicit Extent2D(const VkExtent2D& extent);
        Extent2D(std::uint32_t width, std::uint32_t height);
        static Extent2D fromDim(std::uint32_t dim);
        [[nodiscard]]
        operator VkExtent2D() const;
        [[nodiscard]]
        VkExtent2D &get();
        [[nodiscard]]
        const VkExtent2D &get() const;
        [[nodiscard]]
        Extent3D to3D() const;
        [[nodiscard]]
        Extent2D to2D() const;
        [[nodiscard]]
        std::uint32_t to1D() const;
    };


    struct Rect2D {
        VkOffset2D offset = {};
        VkExtent2D extent = {};

        explicit Rect2D(const VkRect2D& rect);
        Rect2D(const VkOffset2D &offset,
               const VkExtent2D &extent);

        explicit Rect2D(const VkExtent2D &extent);

        operator VkRect2D() const;
        [[nodiscard]]
        VkRect2D &get();
        [[nodiscard]]
        const VkRect2D &get() const;
    };

    struct Viewport {
        float x = 0;
        float y = 0;
        float width = 0;
        float height = 0;
        float minDepth = 0;
        float maxDepth = 0;
        explicit Viewport(const VkViewport& viewport);
        Viewport(float x,
                 float y,
                 float width,
                 float height,
                 float minDepth = 0.0,
                 float maxDepth = 1.0);

        Viewport(float width,
                 float height);
        explicit Viewport(const VkExtent2D &extent);

        operator VkViewport() const;

        [[nodiscard]]
        VkViewport &get();

        [[nodiscard]]
        const VkViewport &get() const;
    };
}
#endif //VULKANWORKSPACE_VKSTRUCTUTILS_H
