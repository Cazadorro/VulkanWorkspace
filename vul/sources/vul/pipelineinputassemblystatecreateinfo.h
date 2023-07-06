//
// Created by Bolt on 7/5/2023.
//

#ifndef VULKANWORKSPACE_PIPELINEINPUTASSEMBLYSTATECREATEINFO_H
#define VULKANWORKSPACE_PIPELINEINPUTASSEMBLYSTATECREATEINFO_H
#include "vul/enums.h"
#include "vul/bool32.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>

namespace vul{
    struct PipelineInputAssemblyStateCreateInfo{
    private:
        const vul::StructureType sType = vul::StructureType::PipelineInputAssemblyStateCreateInfo;
    public:
        const void*              pNext;
        //currently reserved for future use according to API
        VkPipelineInputAssemblyStateCreateFlags flags = {};
        vul::PrimitiveTopology   topology;
        Bool32                   primitiveRestartEnable;

        PipelineInputAssemblyStateCreateInfo();
        PipelineInputAssemblyStateCreateInfo(const PipelineInputAssemblyStateCreateInfo& rhs) noexcept;
        PipelineInputAssemblyStateCreateInfo(PipelineInputAssemblyStateCreateInfo&&info) noexcept;
        PipelineInputAssemblyStateCreateInfo& operator=(const PipelineInputAssemblyStateCreateInfo& rhs) noexcept;
        ~PipelineInputAssemblyStateCreateInfo();

        operator VkPipelineInputAssemblyStateCreateInfo() const noexcept;
        [[nodiscard]]
        VkPipelineInputAssemblyStateCreateInfo &get() noexcept;
        [[nodiscard]]
        const VkPipelineInputAssemblyStateCreateInfo &get() const noexcept;

        [[nodiscard]]
        static PipelineInputAssemblyStateCreateInfo fromTopology(vul::PrimitiveTopology topology) noexcept;
    };
}
#endif //VULKANWORKSPACE_PIPELINEINPUTASSEMBLYSTATECREATEINFO_H
