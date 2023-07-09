//
// Created by Shae Bolt on 8/19/2021.
//

#include "colorformatutils.h"
#include "vul/enums.h"
#include <array>

constexpr std::array nonColorFormats = {
        vul::Format::D16Unorm,
        vul::Format::D16Unorm,
        vul::Format::X8D24UnormPack32,
        vul::Format::D32Sfloat,
        vul::Format::S8Uint,
        vul::Format::D16UnormS8Uint,
        vul::Format::D24UnormS8Uint,
        vul::Format::D32SfloatS8Uint
};
constexpr std::array depthFormats = {
        vul::Format::D16Unorm,
        vul::Format::D16Unorm,
        vul::Format::X8D24UnormPack32,
        vul::Format::D32Sfloat,
        vul::Format::D16UnormS8Uint,
        vul::Format::D24UnormS8Uint,
        vul::Format::D32SfloatS8Uint
};
constexpr std::array stencilFormats = {
        vul::Format::S8Uint,
        vul::Format::D16UnormS8Uint,
        vul::Format::D24UnormS8Uint,
        vul::Format::D32SfloatS8Uint
};
constexpr std::array depthStencilFormats = {
        vul::Format::D16UnormS8Uint,
        vul::Format::D24UnormS8Uint,
        vul::Format::D32SfloatS8Uint
};
bool vul::isColorFormat(vul::Format format) {
    return std::find(nonColorFormats.begin(), nonColorFormats.end(), format) == nonColorFormats.end();
}

bool vul::isDepthFormat(vul::Format format) {
    return std::find(depthFormats.begin(), depthFormats.end(), format) != depthFormats.end();
}

bool vul::isStencilFormat(vul::Format format) {
    return std::find(stencilFormats.begin(), stencilFormats.end(), format) != stencilFormats.end();
}

bool vul::isDepthStencilFormat(vul::Format format) {
    return std::find(depthStencilFormats.begin(), depthStencilFormats.end(), format) != depthStencilFormats.end();;
}

std::span<const vul::Format> vul::getDepthStencilFormats() {
    return depthStencilFormats;
}

uul::EnumFlags<vul::ImageAspectFlagBits> vul::getImageAspect(vul::Format format) {

    uul::EnumFlags<vul::ImageAspectFlagBits> tempAspectBitMask(0);
    if (isColorFormat(static_cast<vul::Format>(format))) {
        tempAspectBitMask |= vul::ImageAspectFlagBits::ColorBit;
    }
    if (isDepthFormat(static_cast<vul::Format>(format))) {
        tempAspectBitMask |= vul::ImageAspectFlagBits::DepthBit;
    }
    if (isStencilFormat(static_cast<vul::Format>(format))) {
        tempAspectBitMask |= vul::ImageAspectFlagBits::StencilBit;
    }
    return tempAspectBitMask;
}
