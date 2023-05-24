//
// Created by Shae Bolt on 8/22/2021.
//

#include "shadermodule.h"
#include "vul/device.h"
#include "vul/enums.h"

vul::ShaderModule::ShaderModule(const vul::Device &device,
                                VkShaderModule handle,
                                const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkShaderModule vul::ShaderModule::get() const {
    return m_handle;
}

vul::ShaderModule::~ShaderModule() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::ShaderModule::ShaderModule(vul::ShaderModule &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::ShaderModule &
vul::ShaderModule::operator=(vul::ShaderModule &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::ShaderModule::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::PipelineVertexShaderStageCreateInfo
vul::ShaderModule::createVertexStageInfo(
        const VkSpecializationInfo *specializationInfo, const char *name,
        const void * pNext,
        uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags) const {
    return vul::PipelineVertexShaderStageCreateInfo(*this,
                                                    specializationInfo,
                                                    name, pNext, flags);
}

vul::PipelineFragmentShaderStageCreateInfo
vul::ShaderModule::createFragmentStageInfo(
        const VkSpecializationInfo *specializationInfo, const char *name,
        const void * pNext,
        uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags) const {
    return vul::PipelineFragmentShaderStageCreateInfo(*this,
                                                    specializationInfo,
                                                    name, pNext, flags);
}

vul::PipelineComputeShaderStageCreateInfo
vul::ShaderModule::createComputeStageInfo(
        const VkSpecializationInfo *specializationInfo, const char *name,
        const void * pNext,
        uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags) const {
    return vul::PipelineComputeShaderStageCreateInfo(*this,
                                                    specializationInfo,
                                                    name, pNext, flags);
}

vul::PipelineVertexShaderStageCreateInfo::PipelineVertexShaderStageCreateInfo(
        const vul::ShaderModule &module,
        const VkSpecializationInfo *pSpecializationInfo, const char *pName,
        const void *pNext, uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags) :
        m_createInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, pNext,
                     flags.to_underlying(), VK_SHADER_STAGE_VERTEX_BIT,
                     module.get(), pName, pSpecializationInfo} {

}

VkPipelineShaderStageCreateInfo
vul::PipelineVertexShaderStageCreateInfo::get() const {
    return m_createInfo;
}

vul::PipelineFragmentShaderStageCreateInfo::PipelineFragmentShaderStageCreateInfo(
        const vul::ShaderModule &module,
        const VkSpecializationInfo *pSpecializationInfo, const char *pName,
        const void *pNext, uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags) :
        m_createInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, pNext,
                     flags.to_underlying(), VK_SHADER_STAGE_FRAGMENT_BIT,
                     module.get(), pName, pSpecializationInfo} {


}

VkPipelineShaderStageCreateInfo
vul::PipelineFragmentShaderStageCreateInfo::get() const {
    return m_createInfo;
}

vul::PipelineComputeShaderStageCreateInfo::PipelineComputeShaderStageCreateInfo(
        const vul::ShaderModule &module,
        const VkSpecializationInfo *pSpecializationInfo, const char *pName,
        const void *pNext, uul::EnumFlags<vul::PipelineShaderStageCreateFlagBits> flags)  :
        m_createInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, pNext,
                     flags.to_underlying(), VK_SHADER_STAGE_COMPUTE_BIT,
                     module.get(), pName, pSpecializationInfo} {


}

VkPipelineShaderStageCreateInfo
vul::PipelineComputeShaderStageCreateInfo::get() const {
    return m_createInfo;
}
