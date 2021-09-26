//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_VMAALLOCATION_H
#define VULKANWORKSPACE_VMAALLOCATION_H

#include "vul/temparrayproxy.h"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <optional>

namespace vul {
    template<typename T>
    class ExpectedResult;

    class VmaAllocator;

    class VmaAllocation {
    public:
        VmaAllocation() = default;

        VmaAllocation(const VmaAllocator &allocator, ::VmaAllocation handle);

        VmaAllocation(VmaAllocation &&rhs) noexcept;

        VmaAllocation &operator=(VmaAllocation &&rhs) noexcept;

        VmaAllocation(const VmaAllocation &rhs) = delete;

        VmaAllocation &operator=(VmaAllocation &rhs) = delete;

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

        template<typename T>
        void mappedCopyFrom(const TempArrayProxy<T>& array, const std::size_t offsetBytes = 0){
            void * mappedPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mapMemory()) + offsetBytes);
            memcpy(mappedPtr, array.data(), array.size_bytes());
        }

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
