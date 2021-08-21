//
// Created by Shae Bolt on 8/19/2021.
//

#ifndef VULKANWORKSPACE_COLORFORMATUTILS_H
#define VULKANWORKSPACE_COLORFORMATUTILS_H
#include "vul/enumsfwd.h"
#include <gsl/span>
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
    gsl::span<const vul::Format> getDepthStencilFormats();
}
#endif //VULKANWORKSPACE_COLORFORMATUTILS_H
