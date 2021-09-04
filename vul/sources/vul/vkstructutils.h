//
// Created by Shae Bolt on 9/3/2021.
//

#ifndef VULKANWORKSPACE_VKSTRUCTUTILS_H
#define VULKANWORKSPACE_VKSTRUCTUTILS_H

#include <vulkan/vulkan.h>

namespace vul {


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
