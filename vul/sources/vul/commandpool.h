//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_COMMANDPOOL_H
#define VULKANWORKSPACE_COMMANDPOOL_H
#include "vul/enumsfwd.h"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <functional>
namespace vul{
    //TODO actually fill out
    template<typename T>
    class ExpectedResult;
    class Device;
    class CommandBuffer;
    class PrimaryCommandBuffer;
    class SecondaryCommandBuffer;
    class Queue;
    class CommandPool{
    public:
        CommandPool() = default;

        CommandPool(const Device &device,
                       VkCommandPool handle,
                       const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkCommandPool get() const;

        ~CommandPool();

        CommandPool(CommandPool &&rhs) noexcept;

//        was noexcept?
        CommandPool &operator=(CommandPool &&rhs) noexcept;

        CommandPool(const CommandPool &rhs) = delete;

        CommandPool &operator=(CommandPool &rhs) = delete;

        Result setObjectName(const std::string &string);
        [[nodiscard]]
        const Device& getDevice() const;

        //TODO commandbuffers meant to be deleted as group?
        [[nodiscard]]
        vul::ExpectedResult<std::vector<PrimaryCommandBuffer>> createPrimaryCommandBuffers(std::uint32_t commandBufferCount, const void* pNext = nullptr) const;
        [[nodiscard]]
        vul::ExpectedResult<std::vector<SecondaryCommandBuffer>> createSecondaryCommandBuffers(std::uint32_t commandBufferCount, const void* pNext = nullptr) const;
        [[nodiscard]]
        vul::ExpectedResult<PrimaryCommandBuffer> createPrimaryCommandBuffer(const void* pNext = nullptr) const;
        [[nodiscard]]
        vul::ExpectedResult<SecondaryCommandBuffer> createSecondaryCommandBuffer(const void* pNext = nullptr) const;

       Result singleTimeSubmit(const Queue& queue, const std::function<void(CommandBuffer&)>& commandBufferFunction) const;
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkCommandPool m_handle = VK_NULL_HANDLE;
    };
}

#endif //VULKANWORKSPACE_COMMANDPOOL_H
