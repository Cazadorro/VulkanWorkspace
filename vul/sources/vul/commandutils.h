//
// Created by Shae Bolt on 9/1/2021.
//

#ifndef VULKANWORKSPACE_COMMANDUTILS_H
#define VULKANWORKSPACE_COMMANDUTILS_H
#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul{
    class Queue;
    class CommandPool;
    class Buffer;
    class Image;


    Result copy(const Buffer& src, Buffer& dst, CommandPool& commandPool, Queue& queue);
    Result copy(const Buffer& src, Image& dst, CommandPool& commandPool, Queue& queue,
                vul::ImageAspectBitMask aspectMask,
                vul::PipelineStageFlagBits2KHR dstStageMask,
                vul::AccessFlagBits2KHR dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0);
    Result transition(const VkImageMemoryBarrier2KHR& barrier, CommandPool& commandPool, Queue& queue);
}
#endif //VULKANWORKSPACE_COMMANDUTILS_H
