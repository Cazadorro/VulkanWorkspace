//
// Created by Shae Bolt on 9/3/2021.
//

#include "vkstructutils.h"
#include "vkassert.h"
#include <fmt/core.h>

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

vul::Extent3D::Extent3D(const VkExtent3D &extent) : width(extent.width), height(extent.height), depth(extent.depth){

}

vul::Extent3D::Extent3D(std::uint32_t width, std::uint32_t height,
                        std::uint32_t depth) : width(width), height(height), depth(depth) {

}

vul::Extent3D vul::Extent3D::fromDim(std::uint32_t dim) {
    return {dim,dim,dim};
}

vul::Extent3D::operator VkExtent3D() const {
    return get();
}

VkExtent3D &vul::Extent3D::get() {
    return *reinterpret_cast<VkExtent3D *>(this);
}

const VkExtent3D &vul::Extent3D::get() const {
    return *reinterpret_cast<const VkExtent3D *>(this);
}

vul::Extent3D vul::Extent3D::to3D() const {
    return *this;
}

vul::Extent2D vul::Extent3D::to2D() const {
    VUL_ASSERT(depth == 1,fmt::format("Depth {} must be 1", depth).c_str());
    return {width, height};
}

std::uint32_t vul::Extent3D::to1D() const {
    VUL_ASSERT(depth == 1 && height == 1,fmt::format("Height {} and Depth {} must be 1", height, depth).c_str());
    return width;
}

vul::Extent2D::Extent2D(const VkExtent2D &extent): width(extent.width), height(extent.height){

}

vul::Extent2D::Extent2D(std::uint32_t width, std::uint32_t height): width(width), height(height){

}

vul::Extent2D vul::Extent2D::fromDim(std::uint32_t dim) {
    return {dim,dim};
}

vul::Extent2D::operator VkExtent2D() const {
    return get();
}

VkExtent2D &vul::Extent2D::get() {
    return *reinterpret_cast<VkExtent2D *>(this);
}

const VkExtent2D &vul::Extent2D::get() const {
    return *reinterpret_cast<const VkExtent2D *>(this);
}

vul::Extent3D vul::Extent2D::to3D() const {
    return {width, height, 1};
}

vul::Extent2D vul::Extent2D::to2D() const {
    return *this;
}

std::uint32_t vul::Extent2D::to1D() const {
    VUL_ASSERT(height == 1,fmt::format("Height {}must be 1", height).c_str());
    return width;
}
