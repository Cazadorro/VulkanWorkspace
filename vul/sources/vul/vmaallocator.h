//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_VMAALLOCATOR_H
#define VULKANWORKSPACE_VMAALLOCATOR_H


#include "vul/device.h"
#include "vul/physicaldevice.h"
#include "vul/instance.h"
#include "vul/bitmasks.h"
#include "vul/expectedresult.h"
#include "vul/buffer.h"
#include "vul/image.h"
#include "vul/commandutils.h"
#include "vul/temparrayproxyfwd.h"
#include <vk_mem_alloc.h>


namespace vul {

    class TempConstVoidArrayProxy;

    class CommandPool;

    class Queue;

    class VmaAllocator {
    public:
        VmaAllocator() = default;

        [[nodiscard]]
        static ExpectedResult<VmaAllocator>
        create(const Instance &instance, const PhysicalDevice &physicalDevice,
               const Device &device, VmaAllocatorCreateFlags flags = {});

        ~VmaAllocator();

        [[nodiscard]]
        VmaAllocatorInfo getAllocatorInfo() const;

        [[nodiscard]]
        VkDevice getVkDevice() const;

        [[nodiscard]]
        VkPhysicalDevice getVkPhysicalDevice() const;

        [[nodiscard]]
        VkInstance getVkInstance() const;

        [[nodiscard]]
        ::VmaAllocator get() const;

        VmaAllocator(VmaAllocator &&rhs) noexcept;

//        was noexcept?
        VmaAllocator &operator=(VmaAllocator &&rhs) noexcept;

        VmaAllocator(const VmaAllocator &rhs) = delete;

        VmaAllocator &operator=(const VmaAllocator &rhs) = delete;

        [[nodiscard]]
        const Device& getDevice() const;

        [[nodiscard]]
        ExpectedResult<Buffer>
        createBuffer(const VmaAllocationCreateInfo &allocInfo,
                     const VkBufferCreateInfo &bufferInfo) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createMappedCoherentBuffer(VkDeviceSize size,
                                                          vul::BufferUsageBitMask usages) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createStagingBuffer(VkDeviceSize size,
                                                   vul::BufferUsageBitMask otherUsages = {}) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createHostDestinationBuffer(VkDeviceSize size,
                                                   vul::BufferUsageBitMask otherUsages = {}) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createDeviceBuffer(VkDeviceSize size,
                                                   vul::BufferUsageBitMask usages) const;

        [[nodiscard]]
        ExpectedResult<Buffer>
        createMappedCoherentBuffer(const TempConstVoidArrayProxy &array,
                                  vul::BufferUsageBitMask otherUsages = {}) const;

        [[nodiscard]]
        ExpectedResult<Buffer>
        createStagingBuffer(const TempConstVoidArrayProxy &array,
                            vul::BufferUsageBitMask otherUsages = {}) const;


        [[nodiscard]]
        ExpectedResult<Buffer>
        createStagingBuffer(const TempArrayProxy<TempConstVoidArrayProxy> &arrayList,
                            vul::BufferUsageBitMask otherUsages = {}) const;


        [[nodiscard]]
        ExpectedResult<Buffer> createDeviceBuffer(
                CommandPool& commandPool, Queue& queue,
                const TempConstVoidArrayProxy &array,
                vul::BufferUsageBitMask usages) const;

        [[nodiscard]]
        ExpectedResult<Image>
        createImage(const VmaAllocationCreateInfo &allocInfo,
                     const VkImageCreateInfo &imageInfo) const;

        [[nodiscard]]
        ExpectedResult<Image>
        createDeviceImage(const VkImageCreateInfo &imageInfo) const;

        [[nodiscard]]
        ExpectedResult<Image> createDeviceImage(
                CommandPool& commandPool, Queue& queue,
                const TempConstVoidArrayProxy &array,
                const VkImageCreateInfo &imageInfo,
                vul::ImageAspectBitMask aspectMask,
                vul::PipelineStage2BitMask dstStageMask,
                vul::Access2BitMask dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0)const;

        [[nodiscard]]
        ExpectedResult<Image> createDeviceImage(
                CommandPool& commandPool, Queue& queue,
                const TempArrayProxy<TempConstVoidArrayProxy> &arrayList,
                const VkImageCreateInfo &imageInfo,
                vul::ImageAspectBitMask aspectMask,
                vul::PipelineStage2BitMask dstStageMask,
                vul::Access2BitMask dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0) const;

        [[nodiscard]]
        ExpectedResult<Image> createDeviceTexture( CommandPool& commandPool, Queue& queue,
                                                 const TempConstVoidArrayProxy &array,
                                                 const VkImageCreateInfo &imageInfo,
                                                 std::uint32_t mipLevel = 0) const;

        [[nodiscard]]
        ExpectedResult<Image> createDeviceTexture( CommandPool& commandPool, Queue& queue,
                                                   const TempArrayProxy<TempConstVoidArrayProxy> &array,
                                                   const VkImageCreateInfo &imageInfo,
                                                   std::uint32_t mipLevel = 0) const;

        [[nodiscard]]
        ExpectedResult<Image> createStorageImage( CommandPool& commandPool, Queue& queue,
                                                   const TempConstVoidArrayProxy &array,
                                                   const VkImageCreateInfo &imageInfo,
                                                   std::uint32_t mipLevel = 0) const;

        //TODO MOVE ONLY!
    private:
        const Device *m_pDevice = nullptr;
        ::VmaAllocator m_handle = VK_NULL_HANDLE;
    };
}


#endif //VULKANWORKSPACE_VMAALLOCATOR_H
