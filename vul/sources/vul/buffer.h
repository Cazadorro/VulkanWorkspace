//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_BUFFER_H
#define VULKANWORKSPACE_BUFFER_H

#include "vul/vmaallocation.h"
#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include "vul/commandutils.h"
#include "vul/temparrayproxyfwd.h"

#include <gsl/span>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <functional>

namespace vul {
    class Device;
    class VmaAllocation;
    class TempConstVoidArrayProxy;
    class Buffer {
    public:
        Buffer() = default;

        Buffer(vul::VmaAllocation && allocation, VkBuffer handle, VkDeviceSize size);

        Buffer(const Buffer &) = delete;

        Buffer &operator=(const Buffer &) = delete;


        Buffer(Buffer &&rhs) noexcept;

        Buffer &operator=(Buffer &&rhs) noexcept;

        ~Buffer();

        [[nodiscard]]
        const VkBuffer &get() const;


        [[nodiscard]]
        const Device& getDevice() const;

        [[nodiscard]]
        const VkDeviceSize &size() const;


        Result setDebugObjectName(const std::string &name);

        [[nodiscard]]
        VkBufferCopy genFullCopyRegion() const;

        [[nodiscard]]
        vul::VmaAllocation& getAllocation();
        [[nodiscard]]
        const vul::VmaAllocation& getAllocation() const;
        [[nodiscard]]
        VkDescriptorBufferInfo createDescriptorInfo(VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE) const;

        VkBufferMemoryBarrier2KHR createMemoryBarrier( vul::PipelineStageFlagBitMask srcStageMask,
                                                       vul::AccessFlagBitMask srcAccessMask,
                                                       vul::PipelineStageFlagBitMask dstStageMask,
                                                       vul::AccessFlagBitMask dstAccessMask,
                                                       VkDeviceSize offset = 0,
                                                       VkDeviceSize size = VK_WHOLE_SIZE,
                                                       std::uint32_t srcQueueFamilyIndex = 0,
                                                       std::uint32_t dstQueueFamilyIndex = 0,
                                                       const void* pNext = nullptr) const;

        [[nodiscard]]
        void *mapMemory();

        [[nodiscard]]
        void * getMappedMemory() const;

        void unmapMemory();

        void mappedCopyFrom(const TempConstVoidArrayProxy& array);

        void mappedCopyFrom(const TempArrayProxy<TempConstVoidArrayProxy>& arrays);

        [[nodiscard]]
        bool isMapped() const;

        void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        void
        invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        // TODO perminantly set vma allocator state to usable with bufferDeviceAddress
        // and make sure all buffers are device addressable period?
        [[nodiscard]]
        VkDeviceAddress getDeviceAddress(const void* pNext = nullptr) const;
    private:
        vul::VmaAllocation m_allocation;
        VkBuffer m_handle = VK_NULL_HANDLE;
        VkDeviceSize m_size = 0;
    };



}
#endif //VULKANWORKSPACE_BUFFER_H
