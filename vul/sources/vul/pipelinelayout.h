//
// Created by Shae Bolt on 8/29/2021.
//

#ifndef VULKANWORKSPACE_PIPELINELAYOUT_H
#define VULKANWORKSPACE_PIPELINELAYOUT_H
#include "vul/enums.h"
#include <vulkan/vulkan.h>
#include <string>
namespace vul{

    template<typename T>
    class ExpectedResult;
    class Device;
    class PipelineLayout{
    public:
        PipelineLayout() = default;

        PipelineLayout(const Device &device,
                       VkPipelineLayout handle,
                const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkPipelineLayout get() const;

        ~PipelineLayout();

        PipelineLayout(PipelineLayout &&rhs) noexcept;

//        was noexcept?
        PipelineLayout &operator=(PipelineLayout &&rhs) noexcept;

        PipelineLayout(const PipelineLayout &rhs) = delete;

        PipelineLayout &operator=(PipelineLayout &rhs) = delete;

        Result setObjectName(const std::string &string);
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkPipelineLayout m_handle = VK_NULL_HANDLE;

    };
}

#endif //VULKANWORKSPACE_PIPELINELAYOUT_H
