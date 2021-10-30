//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_VMAALLOCATION_H
#define VULKANWORKSPACE_VMAALLOCATION_H


#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {
    template<typename T>
    class ExpectedResult;

    class VmaAllocator;
    class TempConstVoidArrayProxy;

    class VmaAllocation {
    public:
        VmaAllocation() = default;

        VmaAllocation(const VmaAllocator &allocator, ::VmaAllocation handle);

        VmaAllocation(VmaAllocation &&rhs) noexcept;

        VmaAllocation &operator=(VmaAllocation &&rhs) noexcept;

        VmaAllocation(const VmaAllocation &rhs) = delete;

        VmaAllocation &operator=(const VmaAllocation &rhs) = delete;

        [[nodiscard]]
        VmaAllocationInfo getAllocationInfo() const;

        [[nodiscard]]
        ::VmaAllocation get() const;

        [[nodiscard]]
        const VmaAllocator &getAllocator() const;

        [[nodiscard]]
        void *mapMemory();

        [[nodiscard]]
        void * getMappedMemory() const;

        void unmapMemory();

        void mappedCopyFrom(const TempConstVoidArrayProxy& array, std::size_t offsetBytes = 0);

        [[nodiscard]]
        bool isMapped() const;

        void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        void
        invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        void unmapAllCounts();
    private:

        const vul::VmaAllocator *m_pAllocator = nullptr;
        ::VmaAllocation m_handle = VK_NULL_HANDLE;
        std::int64_t m_mapCounter = 0;

    };
}
#endif //VULKANWORKSPACE_VMAALLOCATION_H
