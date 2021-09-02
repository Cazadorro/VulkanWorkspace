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
#include <vk_mem_alloc.h>


namespace vul {

    template<typename T>
    class TempArrayProxy;

    class CommandPool;

    class Queue;

    class VmaAllocator {
    public:
        VmaAllocator() = default;

        [[nodiscard]]
        static ExpectedResult<VmaAllocator>
        create(const Instance &instance, const PhysicalDevice &physicalDevice,
               const Device &device);

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

        VmaAllocator &operator=(VmaAllocator &rhs) = delete;

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
        ExpectedResult<Buffer> createDeviceBuffer(VkDeviceSize size,
                                                   vul::BufferUsageBitMask usages) const;

        template<typename T>
        [[nodiscard]]
        ExpectedResult<Buffer>
        createMappedCoherentBuffer(const TempArrayProxy<const T> &array,
                                  vul::BufferUsageBitMask otherUsages = {}) const {
            auto expectedResult = createMappedCoherentBuffer(array.size(),
                                                             otherUsages);
            if (!expectedResult.hasValue()) {
                return expectedResult;
            }
            expectedResult.value.getAllocation().copyToMapped(array);
            return expectedResult;
        }

        template<typename T>
        [[nodiscard]]
        ExpectedResult<Buffer>
        createStagingBuffer(const TempArrayProxy<const T> &array,
                            vul::BufferUsageBitMask otherUsages = {}) const {
            auto expectedResult = createStagingBuffer(array.size(),
                                                      otherUsages);
            if (!expectedResult.hasValue()) {
                return expectedResult;
            }
            expectedResult.value.getAllocation().copyToMapped(array);
            return expectedResult;
        }

        template<typename T>
        [[nodiscard]]
        ExpectedResult<Buffer> createDeviceBuffer(
                CommandPool& commandPool, Queue& queue,
                const TempArrayProxy<const T> &array,
                vul::BufferUsageBitMask usages) const {
            auto expectedStageBuffer = createStagingBuffer(array);
            if(!expectedStageBuffer.hasValue()){
                return {expectedStageBuffer.result, {}};
            }
            auto stagingBuffer = std::move(expectedStageBuffer.value);
            auto expectedBuffer = createDeviceBuffer(array.size(), vul::BufferUsageFlagBits::TransferDstBit | usages);
            if(!expectedBuffer.hasValue()){
                return {expectedBuffer.result, {}};
            }
            auto buffer = std::move(expectedBuffer.value);
            auto result = vul::copy(stagingBuffer, buffer, commandPool, queue);
            return {result, buffer};
        }

        [[nodiscard]]
        ExpectedResult<Image>
        createImage(const VmaAllocationCreateInfo &allocInfo,
                     const VkImageCreateInfo &imageInfo) const;

        [[nodiscard]]
        ExpectedResult<Image>
        createDeviceImage(const VkImageCreateInfo &imageInfo) const;

        template<typename T>
        [[nodiscard]]
        ExpectedResult<Image> createDeviceImage(
                CommandPool& commandPool, Queue& queue,
                const TempArrayProxy<const T> &array,
                const VkImageCreateInfo &imageInfo,
                vul::ImageAspectBitMask aspectMask,
                vul::PipelineStageFlagBits2KHR dstStageMask,
                vul::AccessFlagBits2KHR dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0) const {
            auto expectedStageBuffer = createStagingBuffer(array);
            if(!expectedStageBuffer.hasValue()){
                return {expectedStageBuffer.result, {}};
            }
            auto stagingBuffer = std::move(expectedStageBuffer.value);
            auto tempImageInfo = imageInfo;
            tempImageInfo.usage |= vul::get(vul::ImageUsageFlagBits::TransferDstBit);
            auto expectedImage = createDeviceImage(tempImageInfo);
            if(!expectedImage.hasValue()){
                return {expectedImage.result, {}};
            }
            auto image = std::move(expectedImage.value);
            auto result = vul::copy(stagingBuffer, image, commandPool, queue, aspectMask, dstStageMask, dstAccessMask, dstLayout, mipLevel);
            return {result, image};
        }

        //TODO MOVE ONLY!
    private:
        const Device *m_pDevice = nullptr;
        ::VmaAllocator m_handle = VK_NULL_HANDLE;
    };
}


#endif //VULKANWORKSPACE_VMAALLOCATOR_H
