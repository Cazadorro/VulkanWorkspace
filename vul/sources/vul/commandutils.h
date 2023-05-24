//
// Created by Shae Bolt on 9/1/2021.
//

#ifndef VULKANWORKSPACE_COMMANDUTILS_H
#define VULKANWORKSPACE_COMMANDUTILS_H
#include "vul/enumsfwd.h"
#include <uul/enumflagsfwd.h>
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul{
    class Queue;
    class CommandPool;
    class Buffer;
    class Image;


    Result copy(const Buffer& src, Buffer& dst, CommandPool& commandPool, Queue& queue, VkDeviceSize offset = 0);
    Result copy(const Buffer& src, Buffer& dst, CommandPool& commandPool, Queue& queue,std::uint32_t srcIndex, std::uint32_t dstIndex,  VkDeviceSize offset = 0);
    Result copy(const Buffer& src, Image& dst, CommandPool& commandPool, Queue& queue,
                uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
               uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0);
    Result transition(Image& image, CommandPool& commandPool, Queue& queue,
                      uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                      uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
                     uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                      vul::ImageLayout dstLayout);
    Result transition(const VkImageMemoryBarrier2KHR& barrier, CommandPool& commandPool, Queue& queue);
}
#endif //VULKANWORKSPACE_COMMANDUTILS_H
