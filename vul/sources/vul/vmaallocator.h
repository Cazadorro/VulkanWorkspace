//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_VMAALLOCATOR_H
#define VULKANWORKSPACE_VMAALLOCATOR_H


#include "vul/device.h"
#include "vul/physicaldevice.h"
#include "vul/instance.h"
#include "vul/expectedresult.h"
#include "vul/buffer.h"
#include "vul/image.h"
#include "vul/commandutils.h"
#include "vul/temparrayproxyfwd.h"
#include "uul/concepts.h"
#include "uul/array.h"
#include <uul/enumflagsfwd.h>
#include <vk_mem_alloc.h>


namespace vul {

    class TempConstVoidArrayProxy;

    class CommandPool;

    class Queue;

    struct ImageCreateInfo;

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
        const Device &getDevice() const;

        [[nodiscard]]
        ExpectedResult<Buffer>
        createBuffer(const VmaAllocationCreateInfo &allocInfo,
                     const VkBufferCreateInfo &bufferInfo) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createMappedCoherentBuffer(VkDeviceSize size,
                                                          uul::EnumFlags<vul::BufferUsageFlagBits> usages) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createStagingBuffer(VkDeviceSize size,
                                                   uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages = {}) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createHostDestinationBuffer(VkDeviceSize size,
                                                           uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages = {}) const;

        [[nodiscard]]
        ExpectedResult<Buffer> createDeviceBuffer(VkDeviceSize size,
                                                  uul::EnumFlags<vul::BufferUsageFlagBits> usages) const;

        [[nodiscard]]
        ExpectedResult<Buffer>
        createMappedCoherentBuffer(const TempConstVoidArrayProxy &array,
                                   uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages = {}) const;

        [[nodiscard]]
        ExpectedResult<Buffer>
        createStagingBuffer(const TempConstVoidArrayProxy &array,
                            uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages = {}) const;

        template<uul::ContiguousContainer container>
        [[nodiscard]]
        vul::ExpectedResult<vul::Buffer> createStagingBufferFromMany(const vul::TempArrayProxy<container> &arrayList,
                                                                     uul::EnumFlags<vul::BufferUsageFlagBits> otherUsages = {}) const {
            VkDeviceSize totalSizeBytes = 0;
            for (const auto &array: arrayList) {
                totalSizeBytes += uul::size_bytes(array);
            }
            auto expectedResult = createStagingBuffer(totalSizeBytes,
                                                      otherUsages);
            if (!expectedResult.hasValue()) {
                return expectedResult;
            }
            expectedResult.value.mappedCopyFromArray(arrayList);
            return expectedResult;
        }


        [[nodiscard]]
        ExpectedResult<Buffer> createDeviceBuffer(
                CommandPool &commandPool, Queue &queue,
                const TempConstVoidArrayProxy &array,
                uul::EnumFlags<vul::BufferUsageFlagBits> usages) const;

        [[nodiscard]]
        ExpectedResult<Image>
        createImage(const VmaAllocationCreateInfo &allocInfo,
                    const ImageCreateInfo &imageInfo) const;


        [[nodiscard]]
        ExpectedResult<Image>
        createDeviceImage(const ImageCreateInfo &imageInfo) const;

        [[nodiscard]]
        ExpectedResult<Image> createDeviceImageInitImpl(
                CommandPool &commandPool, Queue &queue,
                ExpectedResult<Buffer> expectedStageBuffer,
                const ImageCreateInfo &imageInfo,
                uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
               uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0) const;

        [[nodiscard]]
        ExpectedResult<Image> createDeviceImage(
                CommandPool &commandPool, Queue &queue,
                const TempConstVoidArrayProxy &array,
                const ImageCreateInfo &imageInfo,
                uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
               uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0) const;

        template<uul::ContiguousContainer container>
        [[nodiscard]]
        ExpectedResult<Image> createDeviceImageArray(
                CommandPool &commandPool, Queue &queue,
                const TempArrayProxy<container> &arrayList,
                const ImageCreateInfo &imageInfo,
                uul::EnumFlags<vul::ImageAspectFlagBits> aspectMask,
                uul::EnumFlags<vul::PipelineStageFlagBits2>  dstStageMask,
               uul::EnumFlags<vul::AccessFlagBits2> dstAccessMask,
                vul::ImageLayout dstLayout,
                std::uint32_t mipLevel = 0) const {
            return createDeviceImageInitImpl(commandPool, queue, createStagingBufferFromMany(arrayList), imageInfo,
                                             aspectMask, dstStageMask, dstAccessMask, dstLayout, mipLevel);
        }

        [[nodiscard]]
        ExpectedResult<Image> createDeviceTexture(CommandPool &commandPool, Queue &queue,
                                                  const TempConstVoidArrayProxy &array,
                                                  const ImageCreateInfo &imageInfo,
                                                  std::uint32_t mipLevel = 0) const;

        template<uul::ContiguousContainer container>
        [[nodiscard]]
        ExpectedResult<Image> createDeviceTextureArray(CommandPool &commandPool, Queue &queue,
                                                       const TempArrayProxy<container> &array,
                                                       const ImageCreateInfo &imageInfo,
                                                       std::uint32_t mipLevel = 0) const{
            return createDeviceImageArray(commandPool, queue, array, imageInfo, vul::ImageAspectFlagBits::ColorBit,
                                          vul::PipelineStageFlagBits2::AllCommandsBit,
                                          vul::AccessFlagBits2::ShaderReadBit,
                                          vul::ImageLayout::ShaderReadOnlyOptimal,
                                          mipLevel);
        }

        [[nodiscard]]
        ExpectedResult<Image> createStorageImage(CommandPool &commandPool, Queue &queue,
                                                 const TempConstVoidArrayProxy &array,
                                                 const ImageCreateInfo &imageInfo,
                                                 std::uint32_t mipLevel = 0) const;

        //TODO MOVE ONLY!
    private:
        const Device *m_pDevice = nullptr;
        ::VmaAllocator m_handle = VK_NULL_HANDLE;
    };
}


#endif //VULKANWORKSPACE_VMAALLOCATOR_H
