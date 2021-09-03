//
// Created by Shae Bolt on 8/28/2021.
//

#include "sampler.h"
#include "vul/device.h"
#include "vul/enums.h"
#include "vul/bitmasks.h"

vul::Sampler::Sampler(const vul::Device &device, VkSampler handle,
                      const VkAllocationCallbacks *pAllocator) : m_pDevice(
        &device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkSampler vul::Sampler::get() const {
    return m_handle;
}

vul::Sampler::~Sampler() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySampler(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::Sampler::Sampler(Sampler &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::Sampler &vul::Sampler::operator=(vul::Sampler &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::Sampler::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

////
//    VkSamplerCreateInfo samplerInfo{};
//    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//    samplerInfo.magFilter = VK_FILTER_LINEAR;
//    samplerInfo.minFilter = VK_FILTER_LINEAR;
//    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.anisotropyEnable = VK_TRUE;
//    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
//    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//    samplerInfo.unnormalizedCoordinates = VK_FALSE;
//    samplerInfo.compareEnable = VK_FALSE;
//    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

vul::SamplerBuilder::SamplerBuilder(const vul::Device &device,
                                    const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator) {
    m_createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    m_createInfo.pNext = nullptr;
//    m_createInfo.flags;
//    m_createInfo.magFilter;
//    m_createInfo.minFilter;
//    m_createInfo.mipmapMode;
//    m_createInfo.addressModeU;
//    m_createInfo.addressModeV;
//    m_createInfo.addressModeW;
//    m_createInfo.mipLodBias;
    m_createInfo.anisotropyEnable = VK_FALSE;
//    m_createInfo.maxAnisotropy;
    m_createInfo.compareEnable = VK_FALSE;
//    m_createInfo.compareOp;
//    m_createInfo.minLod;
//    m_createInfo.maxLod;
    m_createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    m_createInfo.unnormalizedCoordinates = VK_FALSE;
}

void vul::SamplerBuilder::setCreateFlags(vul::SamplerCreateBitMask flags) {
    m_createInfo.flags = flags.to_underlying();
}

void vul::SamplerBuilder::setFilter(vul::Filter filter) {
    setFilter(filter, filter);
}

void
vul::SamplerBuilder::setFilter(vul::Filter minFilter, vul::Filter magFilter) {
    m_createInfo.minFilter = vul::get(minFilter);
    m_createInfo.magFilter = vul::get(magFilter);
}

void vul::SamplerBuilder::setMipmapMode(vul::SamplerMipmapMode mipmapMode) {
    m_createInfo.mipmapMode = vul::get(mipmapMode);
}

void vul::SamplerBuilder::setAddressMode(vul::SamplerAddressMode addressMode) {
    setAddressMode(addressMode, addressMode, addressMode);
}

void vul::SamplerBuilder::setAddressMode(vul::SamplerAddressMode addressModeU,
                                         vul::SamplerAddressMode addressModeV,
                                         vul::SamplerAddressMode addressModeW) {
    m_createInfo.addressModeU = vul::get(addressModeU);
    m_createInfo.addressModeV = vul::get(addressModeV);
    m_createInfo.addressModeW = vul::get(addressModeW);
}

void vul::SamplerBuilder::setMipLodBias(float bias) {
    m_createInfo.mipLodBias = bias;
}

void vul::SamplerBuilder::setLodRange(float min, float max) {
    m_createInfo.minLod = min;
    m_createInfo.maxLod = max;
}

void vul::SamplerBuilder::setBorderColor(vul::BorderColor borderColor) {
    m_createInfo.borderColor = vul::get(borderColor);
}

void vul::SamplerBuilder::enableUnnormalizedCoordinates() {
    m_createInfo.unnormalizedCoordinates = VK_TRUE;
}

void vul::SamplerBuilder::setCompareOp(vul::CompareOp compareOp) {
    m_createInfo.compareEnable = VK_TRUE;
    m_createInfo.compareOp = vul::get(compareOp);
}

void vul::SamplerBuilder::enableAnisotropy() {
    m_createInfo.anisotropyEnable = VK_TRUE;
    m_createInfo.maxAnisotropy = m_pDevice->getPhysicalDevice().getLimits().maxSamplerAnisotropy;
}

void vul::SamplerBuilder::enableAnisotropy(float maxAnisotropy) {
    m_createInfo.anisotropyEnable = VK_TRUE;
    m_createInfo.maxAnisotropy = maxAnisotropy;
}


vul::ExpectedResult<vul::Sampler> vul::SamplerBuilder::create() {
    VkSampler sampler;
    auto result = static_cast<Result>(vkCreateSampler(m_pDevice->get(), &m_createInfo, m_pAllocator, &sampler));

    return vul::ExpectedResult<vul::Sampler>(result,
                                             vul::Sampler(*m_pDevice, sampler, m_pAllocator));
}



