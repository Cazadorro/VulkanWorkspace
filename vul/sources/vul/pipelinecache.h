//
// Created by Shae Bolt on 8/30/2021.
//

#ifndef VULKANWORKSPACE_PIPELINECACHE_H
#define VULKANWORKSPACE_PIPELINECACHE_H
#include "vul/enumsfwd.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
namespace vul{
    template<typename T>
    class ExpectedResult;
    class Device;
    class PipelineCache{
    public:
        PipelineCache() = default;

        PipelineCache(const Device &device,
                VkPipelineCache handle,
                const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkPipelineCache get() const;

        ~PipelineCache();

        PipelineCache(PipelineCache &&rhs) noexcept;

//        was noexcept?
        PipelineCache &operator=(PipelineCache &&rhs) noexcept;

        PipelineCache(const PipelineCache &rhs) = delete;

        PipelineCache &operator=(const PipelineCache &rhs) = delete;

        Result setObjectName(const std::string &string);

        [[nodiscard]]
        ExpectedResult<std::vector<std::byte>> getCacheData() const;
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkPipelineCache m_handle = VK_NULL_HANDLE;
    };
}
#endif //VULKANWORKSPACE_PIPELINECACHE_H
