//
// Created by Bolt on 7/5/2023.
//

#ifndef VULKANWORKSPACE_PIPELINEMULTISAMPLESTATECREATEINFO_H
#define VULKANWORKSPACE_PIPELINEMULTISAMPLESTATECREATEINFO_H
#include "vul/enums.h"
#include "vul/bool32.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>

namespace vul{
    struct PipelineMultisampleStateCreateInfo{
    private:
        const vul::StructureType sType = vul::StructureType::PipelineMultisampleStateCreateInfo;
    public:
        const void*                                 pNext;
        //currently reserved for future use according to API
        VkPipelineMultisampleStateCreateFlags    flags = {};
        SampleCountFlagBits                      rasterizationSamples;
        Bool32                                   sampleShadingEnable;
        float                                    minSampleShading;
        //TODO sample mask is just a uint32, maybe make a wrapper around the mask?
        const VkSampleMask*                      pSampleMask;
        Bool32                                   alphaToCoverageEnable;
        Bool32                                   alphaToOneEnable;

        PipelineMultisampleStateCreateInfo();
        PipelineMultisampleStateCreateInfo(const PipelineMultisampleStateCreateInfo& rhs) noexcept;
        PipelineMultisampleStateCreateInfo(PipelineMultisampleStateCreateInfo&& rhs) noexcept;
        PipelineMultisampleStateCreateInfo& operator=(const PipelineMultisampleStateCreateInfo& rhs) noexcept;
        ~PipelineMultisampleStateCreateInfo();

        operator VkPipelineMultisampleStateCreateInfo() const noexcept;
        [[nodiscard]]
        VkPipelineMultisampleStateCreateInfo &get() noexcept;
        [[nodiscard]]
        const VkPipelineMultisampleStateCreateInfo &get() const noexcept;

        [[nodiscard]]
        static PipelineMultisampleStateCreateInfo fromSampleCount(SampleCountFlagBits sampleCount) noexcept;
    };
}
#endif //VULKANWORKSPACE_PIPELINEMULTISAMPLESTATECREATEINFO_H
