//
// Created by Shae Bolt on 8/22/2021.
//

#ifndef VULKANWORKSPACE_SHADERMODULE_H
#define VULKANWORKSPACE_SHADERMODULE_H

#include "vul/enumsfwd.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <string>
#include <cstdint>

namespace vul{
    class Device;
    enum class Result;
    class ShaderModule;
    class PipelineVertexShaderStageCreateInfo{
    public:
        PipelineVertexShaderStageCreateInfo(
                const ShaderModule& module,
                const VkSpecializationInfo* pSpecializationInfo,
                const char * pName = "main",
                const void * pNext = nullptr,
                uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags = {});
        [[nodiscard]]
        VkPipelineShaderStageCreateInfo get() const;
    private:
        VkPipelineShaderStageCreateInfo m_createInfo;
    };
    class PipelineFragmentShaderStageCreateInfo{
    public:
        PipelineFragmentShaderStageCreateInfo(
                const ShaderModule& module,
                const VkSpecializationInfo* pSpecializationInfo,
                const char * pName = "main",
                const void * pNext = nullptr,
                uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags = {});
        [[nodiscard]]
        VkPipelineShaderStageCreateInfo get() const;
    private:
        VkPipelineShaderStageCreateInfo m_createInfo;
    };
    class PipelineComputeShaderStageCreateInfo{
    public:
        PipelineComputeShaderStageCreateInfo(
                const ShaderModule& module,
                const VkSpecializationInfo* pSpecializationInfo,
                const char * pName = "main",
                const void * pNext = nullptr,
                uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags = {});
        [[nodiscard]]
        VkPipelineShaderStageCreateInfo get() const;
    private:
        VkPipelineShaderStageCreateInfo m_createInfo;
    };

    //TODO I think the plan was to make the Device create the shader module
    class ShaderModule{
    public:
        ShaderModule() = default;

        ShaderModule(const Device &device, VkShaderModule handle,
                   const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkShaderModule get() const;

        ~ShaderModule();

        ShaderModule(ShaderModule &&rhs) noexcept;

//        was noexcept?
        ShaderModule &operator=(ShaderModule &&rhs) noexcept;

        ShaderModule(const ShaderModule &rhs) = delete;

        ShaderModule &operator=(const ShaderModule &rhs) = delete;

        Result setObjectName(const std::string &string);
        [[nodiscard]]
        PipelineVertexShaderStageCreateInfo createVertexStageInfo(
                const VkSpecializationInfo* specializationInfo = nullptr,
                const char * name = "main",
                const void * pNext = nullptr,
                uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags = {}) const;
        [[nodiscard]]
        PipelineFragmentShaderStageCreateInfo createFragmentStageInfo(
                const VkSpecializationInfo* specializationInfo = nullptr,
                const char * name = "main",
                const void * pNext = nullptr,
                uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags = {}) const;
        [[nodiscard]]
        PipelineComputeShaderStageCreateInfo createComputeStageInfo(
                const VkSpecializationInfo* specializationInfo = nullptr,
                const char * name = "main",
                const void * pNext = nullptr,
                uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags = {}) const;
    protected:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkShaderModule m_handle = VK_NULL_HANDLE;
    };

}

#endif //VULKANWORKSPACE_SHADERMODULE_H
