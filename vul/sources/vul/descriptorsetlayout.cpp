//
// Created by Shae Bolt on 8/28/2021.
//

#include "vul/descriptorsetlayout.h"
#include "vul/descriptorset.h"
#include <uul/assert.h>
#include "vul/enums.h"
#include "vul/bitmasks.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/temparrayproxy.h"
#include <fmt/core.h>
#include <algorithm>
#include <numeric>

const VkDescriptorSetLayoutBinding &
vul::DescriptorSetLayoutBinding::get() const {
    return m_layoutBinding;
}

vul::ShaderStageBitMask &vul::DescriptorSetLayoutBinding::getStageFlags() {
    return *reinterpret_cast<vul::ShaderStageBitMask *>(&(m_layoutBinding.stageFlags));
}

const vul::ShaderStageBitMask &
vul::DescriptorSetLayoutBinding::getStageFlags() const {
    return *reinterpret_cast<const vul::ShaderStageBitMask *>(&(m_layoutBinding.stageFlags));;
}

std::uint32_t &vul::DescriptorSetLayoutBinding::getDescriptorCount() {
    return m_layoutBinding.descriptorCount;
}

const std::uint32_t &
vul::DescriptorSetLayoutBinding::getDescriptorCount() const {
    return m_layoutBinding.descriptorCount;
}

void vul::DescriptorSetLayoutBinding::setImmutableSamplers(
        const VkSampler *pImmutableSamplers) {
    UUL_ASSERT(m_layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER,
               "Expected descriptor type to be sampler");
    m_layoutBinding.pImmutableSamplers = pImmutableSamplers;
}

vul::SamplerBinding::SamplerBinding(std::uint32_t binding,
                                    vul::ShaderStageBitMask stageFlags,
                                    std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::ImmutableSamplerBinding::ImmutableSamplerBinding(std::uint32_t binding,
                                                      vul::ShaderStageBitMask stageFlags,
                                                      const VkSampler *pImmutableSamplers,
                                                      std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = pImmutableSamplers;
}

vul::CombinedSamplerBinding::CombinedSamplerBinding(std::uint32_t binding,
                                                    vul::ShaderStageBitMask stageFlags,
                                                    std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::SampledImageBinding::SampledImageBinding(std::uint32_t binding,
                                              vul::ShaderStageBitMask stageFlags,
                                              std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::StorageImageBinding::StorageImageBinding(std::uint32_t binding,
                                              vul::ShaderStageBitMask stageFlags,
                                              std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::UniformTexelBufferBinding::UniformTexelBufferBinding(std::uint32_t binding,
                                                          vul::ShaderStageBitMask stageFlags,
                                                          std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::StorageTexelBufferBinding::StorageTexelBufferBinding(std::uint32_t binding,
                                                          vul::ShaderStageBitMask stageFlags,
                                                          std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::UniformBufferBinding::UniformBufferBinding(std::uint32_t binding,
                                                vul::ShaderStageBitMask stageFlags,
                                                std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::StorageBufferBinding::StorageBufferBinding(std::uint32_t binding,
                                                vul::ShaderStageBitMask stageFlags,
                                                std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::DynamicUniformBufferBinding::DynamicUniformBufferBinding(
        std::uint32_t binding, vul::ShaderStageBitMask stageFlags,
        std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::DynamicStorageBufferBinding::DynamicStorageBufferBinding(
        std::uint32_t binding, vul::ShaderStageBitMask stageFlags,
        std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;
}

vul::InputAttachmentBinding::InputAttachmentBinding(std::uint32_t binding,
                                                    vul::ShaderStageBitMask stageFlags,
                                                    std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;

}


vul::InlineUniformBlockEXT::InlineUniformBlockEXT(std::uint32_t binding,
                                                  vul::ShaderStageBitMask stageFlags,
                                                  std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;

}

vul::AccelerationStructureKHR::AccelerationStructureKHR(std::uint32_t binding,
                                                        vul::ShaderStageBitMask stageFlags,
                                                        std::uint32_t descriptorCount) {
    m_layoutBinding.binding = binding;
    m_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    m_layoutBinding.descriptorCount = descriptorCount;
    m_layoutBinding.stageFlags = stageFlags.to_underlying();
    m_layoutBinding.pImmutableSamplers = nullptr;

}


vul::DescriptorSetLayout::DescriptorSetLayout(const vul::Device &device,
                                              VkDescriptorSetLayout handle,
                                              const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator), m_handle(handle) {
}

const VkDescriptorSetLayout& vul::DescriptorSetLayout::get() const {
    return m_handle;
}

vul::DescriptorSetLayout::~DescriptorSetLayout() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::DescriptorSetLayout::DescriptorSetLayout(
        vul::DescriptorSetLayout &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::DescriptorSetLayout &
vul::DescriptorSetLayout::operator=(vul::DescriptorSetLayout &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::DescriptorSetLayout::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}


vul::DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(
        const vul::Device &device, const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator) {

}

void vul::DescriptorSetLayoutBuilder::setFlags(
        vul::DescriptorSetLayoutCreateBitMask flags) {
    m_flags = flags.to_underlying();
}

void vul::DescriptorSetLayoutBuilder::setBindings(
        const TempArrayProxy<const VkDescriptorSetLayoutBinding> &descriptorSetBindings,
        const TempArrayProxy<vul::DescriptorBindingBitMask const>& bindingFlags) {

    // first, figure out key order.
    std::vector<std::uint32_t> idxs(descriptorSetBindings.size());
    std::iota(idxs.begin(), idxs.end(), 0);

    std::sort(idxs.begin(), idxs.end(),
              [&descriptorSetBindings](auto lhs_idx, auto rhs_idx) {
                  auto lhs = descriptorSetBindings[lhs_idx];
                  auto rhs = descriptorSetBindings[rhs_idx];
                  return lhs.binding < rhs.binding;
              });
    //now we know what the order of elements should be, fill in elements from this order.
    m_bindings.clear();
    m_bindings.reserve(descriptorSetBindings.size());
    m_bindingFlags.clear();
    UUL_ASSERT(bindingFlags.size() == descriptorSetBindings.size() || bindingFlags.size() <= 1, fmt::format(
            "Expected binding flags size to be 0,1 or the same size as descriptorSetBindings: flags {} vs {}",
            bindingFlags.size(), descriptorSetBindings.size()).c_str());
    if(bindingFlags.size() == descriptorSetBindings.size()){
        m_bindingFlags.reserve(descriptorSetBindings.size());
        for (const auto &idx : idxs) {
            m_bindings.push_back(descriptorSetBindings[idx]);
            m_bindingFlags.push_back(bindingFlags[idx]);
        }
    }else{ //if (bindingFlags.size() >= 1){  not needed because of assert.
        if(bindingFlags.size() == 1){
            m_bindingFlags = std::vector<vul::DescriptorBindingBitMask>(descriptorSetBindings.size(), bindingFlags[0]);
        }
        for (const auto &idx : idxs) {
            m_bindings.push_back(descriptorSetBindings[idx]);
        }
    }

    auto max_value = *std::max_element(m_bindings.begin(), m_bindings.end(),
                                       [](auto lhs, auto rhs) {
                                           return lhs.binding < rhs.binding;
                                       });
    UUL_ASSERT(max_value.binding == (m_bindings.size() - 1), fmt::format(
            "Expected max_value.binding {} to be the same as binding size {} - 1",
            max_value.binding, m_bindings.size()).c_str());
}

void vul::DescriptorSetLayoutBuilder::setBindings(
        const TempArrayProxy<const VkDescriptorSetLayoutBinding> &descriptorSetBindings,
        const TempArrayProxy<const std::string> &bindingName,
        const TempArrayProxy<vul::DescriptorBindingBitMask const>& bindingFlags) {

    UUL_ASSERT(descriptorSetBindings.size() == bindingName.size(),
               fmt::format(
                       "Expected descriptor and binding name sizes to be the same\n"
                       "descriptorSetBindings {} vs bindingName {}",
                       descriptorSetBindings.size(),
                       bindingName.size()).c_str());
    std::vector<std::uint32_t> idxs(descriptorSetBindings.size());
    std::iota(idxs.begin(), idxs.end(), 0);

    std::sort(idxs.begin(), idxs.end(),
              [&descriptorSetBindings](auto lhs_idx, auto rhs_idx) {
                  auto lhs = descriptorSetBindings[lhs_idx];
                  auto rhs = descriptorSetBindings[rhs_idx];
                  return lhs.binding < rhs.binding;
              });
    m_bindings.clear();
    m_bindings.reserve(descriptorSetBindings.size());
    m_bindingFlags.clear();

    m_nameBindingMap.clear();
    m_nameBindingMap.reserve(descriptorSetBindings.size());
    UUL_ASSERT(bindingFlags.size() == descriptorSetBindings.size() || bindingFlags.size() <= 1, fmt::format(
            "Expected binding flags size to be 0,1 or the same size as descriptorSetBindings: flags {} vs {}",
            bindingFlags.size(), descriptorSetBindings.size()).c_str());

    if(bindingFlags.size() == descriptorSetBindings.size()){
        m_bindingFlags.reserve(bindingFlags.size());
        for (const auto &idx : idxs) {
            m_nameBindingMap[bindingName[idx]] = static_cast<std::uint32_t>(m_bindings.size());
            m_bindings.push_back(descriptorSetBindings[idx]);
            m_bindingFlags.push_back(bindingFlags[idx]);
        }
    }else{ //if (bindingFlags.size() >= 1){  not needed because of assert.
        if(bindingFlags.size() == 1){
            m_bindingFlags = std::vector<vul::DescriptorBindingBitMask>(descriptorSetBindings.size(), bindingFlags[0]);
        }
        for (const auto &idx : idxs) {
            m_nameBindingMap[bindingName[idx]] = static_cast<std::uint32_t>(m_bindings.size());
            m_bindings.push_back(descriptorSetBindings[idx]);
        }
    }
    auto max_value = *std::max_element(m_bindings.begin(), m_bindings.end(),
                                       [](auto lhs, auto rhs) {
                                           return lhs.binding < rhs.binding;
                                       });
    UUL_ASSERT(max_value.binding == (m_bindings.size() - 1), fmt::format(
            "Expected max_value.binding {} to be the same as binding size {} - 1",
            max_value.binding, m_bindings.size()).c_str());
}

vul::ExpectedResult<vul::DescriptorSetLayout>
vul::DescriptorSetLayoutBuilder::create() const{


    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //TODO figure out what to do with this nullptr
    layoutInfo.pNext = nullptr;
    layoutInfo.flags = m_flags;
    layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
    layoutInfo.pBindings = m_bindings.data();

    if(!m_bindingFlags.empty()){
        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo={};
        bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        bindingFlagsCreateInfo.pNext = nullptr;
        bindingFlagsCreateInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        bindingFlagsCreateInfo.pBindingFlags = reinterpret_cast<const VkDescriptorBindingFlags*>(m_bindingFlags.data());
        layoutInfo.pNext = &bindingFlagsCreateInfo;

        VkDescriptorSetLayout descriptorSetLayout;
        auto result = static_cast<Result>(vkCreateDescriptorSetLayout(
                m_pDevice->get(), &layoutInfo,
                m_pAllocator, &descriptorSetLayout));
        return {result,
                DescriptorSetLayout(*m_pDevice, descriptorSetLayout, m_pAllocator)};
    }else{

        VkDescriptorSetLayout descriptorSetLayout;
        auto result = static_cast<Result>(vkCreateDescriptorSetLayout(
                m_pDevice->get(), &layoutInfo,
                m_pAllocator, &descriptorSetLayout));
        return {result,
                DescriptorSetLayout(*m_pDevice, descriptorSetLayout, m_pAllocator)};
    }
}

std::vector<VkDescriptorPoolSize>
vul::DescriptorSetLayoutBuilder::calcPoolSizes(std::uint32_t multiplier) const {
    std::unordered_map<VkDescriptorType, std::uint32_t> descriptor_count_map;
    for (const auto &binding : m_bindings) {
        auto map_itr = descriptor_count_map.find(binding.descriptorType);
        if (map_itr == descriptor_count_map.end()) {
            descriptor_count_map[binding.descriptorType] = binding.descriptorCount;
        } else {
            map_itr->second += binding.descriptorCount;
        }
    }
    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(descriptor_count_map.size());
    for (const auto&[key, value] : descriptor_count_map) {
        pool_sizes.emplace_back(VkDescriptorPoolSize{key, value * multiplier});
    }
    return pool_sizes;
}


vul::DescriptorSetLayoutBinding &
vul::DescriptorSetLayoutBuilder::getBinding(std::uint32_t i) {
    return *reinterpret_cast<DescriptorSetLayoutBinding *>(&m_bindings.at(i));
}

vul::DescriptorSetLayoutBinding &
vul::DescriptorSetLayoutBuilder::getBinding(const std::string_view &name) {
    return getBinding(m_nameBindingMap.at(std::string(name)));
}

const vul::DescriptorSetLayoutBinding &
vul::DescriptorSetLayoutBuilder::getBinding(std::uint32_t i) const {
    return *reinterpret_cast<const DescriptorSetLayoutBinding *>(&m_bindings.at(
            i));
}

const vul::DescriptorSetLayoutBinding &
vul::DescriptorSetLayoutBuilder::getBinding(
        const std::string_view &name) const {
    return getBinding(m_nameBindingMap.at(std::string(name)));
}

void vul::DescriptorSetLayoutBuilder::setStageFlags(
        vul::ShaderStageBitMask stageFlags) {
    for (auto &binding : m_bindings) {
        binding.stageFlags = stageFlags.to_underlying();
    }
}

vul::DescriptorSetUpdateBuilder
vul::DescriptorSetLayoutBuilder::createUpdateBuilder() const{
    return DescriptorSetUpdateBuilder(m_bindings, m_nameBindingMap);
}

void vul::DescriptorSetLayoutBuilder::setBindings(
        const vul::TempArrayProxy<const VkDescriptorSetLayoutBinding> &descriptorSetBindings) {
    setBindings(descriptorSetBindings, vul::TempArrayProxy<const vul::DescriptorBindingBitMask>{});
}

void vul::DescriptorSetLayoutBuilder::setBindings(const TempArrayProxy<VkDescriptorSetLayoutBinding const>& descriptorSetBindings,
                 const TempArrayProxy<std::string const>& bindingName){
    setBindings(descriptorSetBindings, bindingName, vul::TempArrayProxy<const vul::DescriptorBindingBitMask>{});
}
