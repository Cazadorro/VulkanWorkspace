//
// Created by Bolt on 7/5/2023.
//

#ifndef VULKANWORKSPACE_PIPELINERASTERIZATIONSTATECREATEINFO_H
#define VULKANWORKSPACE_PIPELINERASTERIZATIONSTATECREATEINFO_H

#include "vul/enums.h"
#include "vul/bool32.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
namespace vul{
    struct PipelineRasterizationStateCreateInfo{
    private:
        const vul::StructureType sType = vul::StructureType::PipelineRasterizationStateCreateInfo;
    public:
        const void*                                 pNext;
        //currently reserved for future use according to API
        VkPipelineRasterizationStateCreateFlags    flags = {};
        Bool32                                     depthClampEnable;
        Bool32                                     rasterizerDiscardEnable;
        PolygonMode                                polygonMode;
        uul::EnumFlags<CullModeFlagBits>           cullMode;
        FrontFace                                  frontFace;
        Bool32                                     depthBiasEnable;
        float                                      depthBiasConstantFactor;
        float                                      depthBiasClamp;
        float                                      depthBiasSlopeFactor;
        float                                      lineWidth;


        PipelineRasterizationStateCreateInfo();
        PipelineRasterizationStateCreateInfo(const PipelineRasterizationStateCreateInfo& rhs) noexcept;
        PipelineRasterizationStateCreateInfo(PipelineRasterizationStateCreateInfo&& rhs) noexcept;
        PipelineRasterizationStateCreateInfo& operator=(const PipelineRasterizationStateCreateInfo& rhs) noexcept;
        ~PipelineRasterizationStateCreateInfo();

        operator VkPipelineRasterizationStateCreateInfo() const noexcept;
        [[nodiscard]]
        VkPipelineRasterizationStateCreateInfo &get() noexcept;
        [[nodiscard]]
        const VkPipelineRasterizationStateCreateInfo &get() const noexcept;

        [[nodiscard]]
        static PipelineRasterizationStateCreateInfo fromCullMode(uul::EnumFlags<CullModeFlagBits> cullMode) noexcept;
    };
}
#endif //VULKANWORKSPACE_PIPELINERASTERIZATIONSTATECREATEINFO_H
