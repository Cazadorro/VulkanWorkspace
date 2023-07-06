//
// Created by Bolt on 7/5/2023.
//

#ifndef VULKANWORKSPACE_PIPELINEDEPTHSTENCILSTATECREATEINFO_H
#define VULKANWORKSPACE_PIPELINEDEPTHSTENCILSTATECREATEINFO_H
#include "vul/enums.h"
#include "vul/bool32.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>

namespace vul{
    struct StencilOpState {
        StencilOp      failOp;
        StencilOp      passOp;
        StencilOp      depthFailOp;
        CompareOp      compareOp;
        std::uint32_t  compareMask;
        std::uint32_t  writeMask;
        std::uint32_t  reference;

        operator VkStencilOpState() const noexcept;
        [[nodiscard]]
        VkStencilOpState &get() noexcept;
        [[nodiscard]]
        const VkStencilOpState &get() const noexcept;
    };

    struct PipelineDepthStencilStateCreateInfo{
    private:
        const vul::StructureType sType = vul::StructureType::PipelineDepthStencilStateCreateInfo;
    public:
        const void*              pNext;
        uul::EnumFlags<PipelineDepthStencilStateCreateFlagBits> flags;
        Bool32                                  depthTestEnable;
        Bool32                                  depthWriteEnable;
        CompareOp                               depthCompareOp;
        Bool32                                  depthBoundsTestEnable;
        Bool32                                  stencilTestEnable;
        StencilOpState                          front;
        StencilOpState                          back;
        float                                   minDepthBounds;
        float                                   maxDepthBounds;


        PipelineDepthStencilStateCreateInfo();
        PipelineDepthStencilStateCreateInfo(const PipelineDepthStencilStateCreateInfo& rhs) noexcept;
        PipelineDepthStencilStateCreateInfo(PipelineDepthStencilStateCreateInfo&& rhs) noexcept;
        PipelineDepthStencilStateCreateInfo& operator=(const PipelineDepthStencilStateCreateInfo& rhs) noexcept;
        ~PipelineDepthStencilStateCreateInfo();

        operator VkPipelineDepthStencilStateCreateInfo() const noexcept;
        [[nodiscard]]
        VkPipelineDepthStencilStateCreateInfo &get() noexcept;
        [[nodiscard]]
        const VkPipelineDepthStencilStateCreateInfo &get() const noexcept;

        [[nodiscard]]
        static PipelineDepthStencilStateCreateInfo fromCompareOp(vul::CompareOp compareOp) noexcept;
    };
}
#endif //VULKANWORKSPACE_PIPELINEDEPTHSTENCILSTATECREATEINFO_H
