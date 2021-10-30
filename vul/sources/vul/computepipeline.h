//
// Created by Shae Bolt on 8/30/2021.
//

#ifndef VULKANWORKSPACE_COMPUTEPIPELINE_H
#define VULKANWORKSPACE_COMPUTEPIPELINE_H


#include "vul/bitmasksfwd.h"
#include "vul/shadermodule.h"
#include <vulkan/vulkan.h>
#include <vector>
namespace vul{
    template<typename T>
    class ExpectedResult;

    class Device;

    class PipelineLayout;

    class RenderPass;

    class PipelineCache;

    class ComputePipeline {
    public:
        ComputePipeline() = default;

        ComputePipeline(const Device &device,
                         VkPipeline handle,
                         const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkPipeline get() const;

        ~ComputePipeline();

        ComputePipeline(ComputePipeline &&rhs) noexcept;

//        was noexcept?
        ComputePipeline &operator=(ComputePipeline &&rhs) noexcept;

        ComputePipeline(const ComputePipeline &rhs) = delete;

        ComputePipeline &operator=(const ComputePipeline &rhs) = delete;

        Result setObjectName(const std::string &string);

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkPipeline m_handle = VK_NULL_HANDLE;
    };

    class PipelineComputeShaderStageCreateInfo;
    class ComputePipelineBuilder {
    public:

        explicit ComputePipelineBuilder(const Device &device,
                                         const VkAllocationCallbacks *pAllocator = nullptr);

        void setFlags(vul::PipelineCreateBitMask flags);

        void setShaderCreateInfo(
                PipelineComputeShaderStageCreateInfo computeInfo);


        void setPipelineLayout(const PipelineLayout &pipelineLayout);

        void setBasePipeline(const ComputePipeline &basePipeline,
                             std::int32_t basePipelineIndex);

        [[nodiscard]]
        ExpectedResult<ComputePipeline>
        create(VkPipelineCache pipelineCache) const;
        [[nodiscard]]
        ExpectedResult<ComputePipeline>
        create(const PipelineCache &pipelineCache) const;
        [[nodiscard]]
        ExpectedResult<ComputePipeline>
        create() const;

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkPipelineCreateFlags m_flags = {};
        VkPipelineShaderStageCreateInfo m_computeStage;
        VkPipelineLayout m_layout = VK_NULL_HANDLE;
        VkPipeline m_basePipelineHandle = VK_NULL_HANDLE;
        std::int32_t m_basePipelineIndex = 0;
    };
}
#endif //VULKANWORKSPACE_COMPUTEPIPELINE_H
