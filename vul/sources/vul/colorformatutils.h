//
// Created by Shae Bolt on 8/19/2021.
//

#ifndef VULKANWORKSPACE_COLORFORMATUTILS_H
#define VULKANWORKSPACE_COLORFORMATUTILS_H
#include "vul/enumsfwd.h"
#include <uul/enumflags.h>
#include <gsl/span>
#include <optional>
namespace vul{
    [[nodiscard]]
    bool isColorFormat(vul::Format format);
    [[nodiscard]]
    bool isDepthFormat(vul::Format format);
    [[nodiscard]]
    bool isStencilFormat(vul::Format format);
    [[nodiscard]]
    bool isDepthStencilFormat(vul::Format format);

    [[nodiscard]]
    bool isDepthOrStencilFormat(vul::Format format);

    [[nodiscard]]
    std::span<const vul::Format> getDepthStencilFormats();

    [[nodiscard]]
    uul::EnumFlags<vul::ImageAspectFlagBits> getImageAspect(vul::Format format);
}
#endif //VULKANWORKSPACE_COLORFORMATUTILS_H
