//
// Created by Shae Bolt on 9/3/2021.
//

#include "vkstructutils.h"

vul::Rect2D::Rect2D(const VkOffset2D &offset, const VkExtent2D &extent)
        : offset(offset),
          extent(extent) {

}

vul::Rect2D::Rect2D(const VkExtent2D &extent) : Rect2D({0, 0}, extent) {

}

vul::Rect2D::operator VkRect2D() const {
    return get();
}


VkRect2D &vul::Rect2D::get() {
    return *reinterpret_cast<VkRect2D *>(this);
}

const VkRect2D &vul::Rect2D::get() const {
    return *reinterpret_cast<const VkRect2D *>(this);
}

vul::Rect2D::Rect2D(const VkRect2D &rect) : Rect2D(rect.offset, rect.extent) {

}

vul::Viewport::Viewport(float x, float y, float width, float height,
                        float minDepth, float maxDepth) :
        x(x), y(y), width(width), height(height), minDepth(minDepth),
        maxDepth(maxDepth) {

}

vul::Viewport::Viewport(float width, float height) : Viewport(0, 0, width,
                                                              height) {

}

vul::Viewport::operator VkViewport() const {
    return get();
}

VkViewport &vul::Viewport::get() {
    return *reinterpret_cast<VkViewport *>(this);
}

const VkViewport &vul::Viewport::get() const {
    return *reinterpret_cast<const VkViewport *>(this);
}

vul::Viewport::Viewport(const VkExtent2D &extent) : Viewport(
        static_cast<float>(extent.width), static_cast<float>(extent.height)) {

}

vul::Viewport::Viewport(const VkViewport &viewport) : Viewport(viewport.x,
                                                               viewport.y,
                                                               viewport.width,
                                                               viewport.height,
                                                               viewport.minDepth,
                                                               viewport.maxDepth) {

}
