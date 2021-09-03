//
// Created by Shae Bolt on 9/3/2021.
//

#include "extensionfunctions.h"

namespace vul {
    PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR_ = (PFN_vkQueueSubmit2KHR) vkGetInstanceProcAddr(
            VK_NULL_HANDLE, "vkQueueSubmit2KHR");
    PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR_ = (PFN_vkCmdPipelineBarrier2KHR) vkGetInstanceProcAddr(
            VK_NULL_HANDLE, "vkCmdPipelineBarrier2KHR");
}