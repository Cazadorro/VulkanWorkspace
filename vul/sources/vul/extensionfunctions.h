//
// Created by Shae Bolt on 9/3/2021.
//

#ifndef VULKANWORKSPACE_EXTENSIONFUNCTIONS_H
#define VULKANWORKSPACE_EXTENSIONFUNCTIONS_H
#include <vulkan/vulkan.h>

namespace vul {
    extern PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR_;
    extern PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR_;
}
#endif //VULKANWORKSPACE_EXTENSIONFUNCTIONS_H
