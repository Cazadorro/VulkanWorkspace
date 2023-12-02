//
// Created by Shae Bolt on 8/28/2021.
//

#include "vul/writedescriptorset.h"
#include "vul/descriptorsetlayout.h"
#include "vul/descriptorset.h"
#include <uul/assert.h>
#include "vul/enums.h"
#include "vul/device.h"
#include "vul/expectedresult.h"
#include "vul/temparrayproxy.h"
#include <fmt/core.h>
#include <algorithm>
#include <numeric>


vul::DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(std::uint32_t binding, vul::DescriptorType descriptorType,
                                                            std::uint32_t descriptorCount,
                                                            uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                            const VkSampler *pImmutableSamplers) :
        binding(binding), descriptorType(descriptorType), descriptorCount(descriptorCount),
        stageFlags(stageFlags), pImmutableSamplers(pImmutableSamplers) {

}

vul::DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(VkDescriptorSetLayoutBinding descriptorSetLayoutBinding) :
        DescriptorSetLayoutBinding(descriptorSetLayoutBinding.binding,
                                   static_cast<vul::DescriptorType>(descriptorSetLayoutBinding.descriptorType),
                                   descriptorCount,
                                   static_cast<uul::EnumFlags<vul::ShaderStageFlagBits> >(stageFlags),
                                   pImmutableSamplers) {

}

VkDescriptorSetLayoutBinding &vul::DescriptorSetLayoutBinding::get() {
    return *reinterpret_cast<VkDescriptorSetLayoutBinding *>(this);
}

const VkDescriptorSetLayoutBinding &vul::DescriptorSetLayoutBinding::get() const {
    return *reinterpret_cast<const VkDescriptorSetLayoutBinding *>(this);
}

vul::DescriptorSetLayoutBinding::operator VkDescriptorSetLayoutBinding() const {
    return get();
}


vul::SamplerBinding::SamplerBinding(std::uint32_t binding,
                                    uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                    std::uint32_t descriptorCount) :
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::Sampler, descriptorCount, stageFlags, nullptr) {
}

vul::ImmutableSamplerBinding::ImmutableSamplerBinding(std::uint32_t binding,
                                                      uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                      const VkSampler *pImmutableSamplers,
                                                      std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::Sampler, descriptorCount, stageFlags, pImmutableSamplers) {
}

vul::CombinedSamplerBinding::CombinedSamplerBinding(std::uint32_t binding,
                                                    uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                    std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::CombinedImageSampler, descriptorCount, stageFlags, nullptr) {
}


vul::SampledImageBinding::SampledImageBinding(std::uint32_t binding,
                                              uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                              std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::SampledImage, descriptorCount, stageFlags, nullptr) {
}

vul::StorageImageBinding::StorageImageBinding(std::uint32_t binding,
                                              uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                              std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::StorageImage, descriptorCount, stageFlags, nullptr) {
}


vul::UniformTexelBufferBinding::UniformTexelBufferBinding(std::uint32_t binding,
                                                          uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                          std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::UniformTexelBuffer, descriptorCount, stageFlags, nullptr) {
}

vul::StorageTexelBufferBinding::StorageTexelBufferBinding(std::uint32_t binding,
                                                          uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                          std::uint32_t descriptorCount) :
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::StorageTexelBuffer, descriptorCount, stageFlags, nullptr) {
}


vul::UniformBufferBinding::UniformBufferBinding(std::uint32_t binding,
                                                uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::UniformBuffer, descriptorCount, stageFlags, nullptr) {
}


vul::StorageBufferBinding::StorageBufferBinding(std::uint32_t binding,
                                                uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::StorageBuffer, descriptorCount, stageFlags, nullptr) {
}

vul::DynamicUniformBufferBinding::DynamicUniformBufferBinding(
        std::uint32_t binding, uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
        std::uint32_t descriptorCount) :
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::UniformBufferDynamic, descriptorCount, stageFlags, nullptr) {
}


vul::DynamicStorageBufferBinding::DynamicStorageBufferBinding(
        std::uint32_t binding, uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
        std::uint32_t descriptorCount) :
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::StorageBufferDynamic, descriptorCount, stageFlags, nullptr) {
}


vul::InputAttachmentBinding::InputAttachmentBinding(std::uint32_t binding,
                                                    uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                    std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::InputAttachment, descriptorCount, stageFlags, nullptr) {
}



vul::InlineUniformBlockBinding::InlineUniformBlockBinding(std::uint32_t binding,
                                                          uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                          std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::InlineUniformBlock, descriptorCount, stageFlags, nullptr) {
}

vul::AccelerationStructureKHRBinding::AccelerationStructureKHRBinding(std::uint32_t binding,
                                                                      uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                                                      std::uint32_t descriptorCount):
        DescriptorSetLayoutBinding(binding, vul::DescriptorType::AccelerationStructureKhr, descriptorCount, stageFlags, nullptr) {
}


vul::DescriptorSetLayout::DescriptorSetLayout(const vul::Device &device,
                                              VkDescriptorSetLayout handle,
                                              const VkAllocationCallbacks *pAllocator)
        : m_pDevice(&device), m_pAllocator(pAllocator), m_handle(handle) {
}

const VkDescriptorSetLayout &vul::DescriptorSetLayout::get() const {
    return m_handle;
}

vul::DescriptorSetLayout::~DescriptorSetLayout() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::DescriptorSetLayout::DescriptorSetLayout(
        vul::DescriptorSetLayout &&rhs) noexcept : DescriptorSetLayout(){
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

vul::DescriptorSetLayoutBuilder& vul::DescriptorSetLayoutBuilder::setFlags(
        uul::EnumFlags<vul::DescriptorSetLayoutCreateFlagBits> flags) {
    m_flags = flags.to_underlying();
    return *this;
}

vul::DescriptorSetLayoutBuilder& vul::DescriptorSetLayoutBuilder::setBindings(
        const TempArrayProxy<const DescriptorSetLayoutBinding> &descriptorSetBindings,
        const TempArrayProxy<uul::EnumFlags<vul::DescriptorBindingFlagBits>  const> &bindingFlags) {

    // first, figure out key order.
    std::vector<std::uint32_t> idxs(descriptorSetBindings.size());
    std::iota(idxs.begin(), idxs.end(), 0);

    std::sort(idxs.begin(), idxs.end(),
              [&descriptorSetBindings](auto lhs_idx, auto rhs_idx) {
                  auto lhs = descriptorSetBindings[lhs_idx];
                  auto rhs = descriptorSetBindings[rhs_idx];
                  return lhs.get().binding < rhs.get().binding;
              });
    //now we know what the order of elements should be, fill in elements from this order.
    m_bindings.clear();
    m_bindings.reserve(descriptorSetBindings.size());
    m_bindingFlags.clear();
    UUL_ASSERT(bindingFlags.size() == descriptorSetBindings.size() || bindingFlags.size() <= 1, fmt::format(
            "Expected binding flags size to be 0,1 or the same size as descriptorSetBindings: flags {} vs {}",
            bindingFlags.size(), descriptorSetBindings.size()).c_str());
    if (bindingFlags.size() == descriptorSetBindings.size()) {
        m_bindingFlags.reserve(descriptorSetBindings.size());
        for (const auto &idx: idxs) {
            m_bindings.push_back(descriptorSetBindings[idx]);
            m_bindingFlags.push_back(bindingFlags[idx]);
        }
    } else { //if (bindingFlags.size() >= 1){  not needed because of assert.
        if (bindingFlags.size() == 1) {
            m_bindingFlags = std::vector<uul::EnumFlags<vul::DescriptorBindingFlagBits> >(descriptorSetBindings.size(), bindingFlags[0]);
        }
        for (const auto &idx: idxs) {
            m_bindings.push_back(descriptorSetBindings[idx]);
        }
    }

    auto max_value = *std::max_element(m_bindings.begin(), m_bindings.end(),
                                       [](auto lhs, auto rhs) {
                                           return lhs.get().binding < rhs.get().binding;
                                       });
    UUL_ASSERT(max_value.get().binding == (m_bindings.size() - 1), fmt::format(
            "Expected max_value.binding {} to be the same as binding size {} - 1",
            max_value.get().binding, m_bindings.size()).c_str());
    return *this;
}

vul::DescriptorSetLayoutBuilder& vul::DescriptorSetLayoutBuilder::setBindings(
        const TempArrayProxy<const DescriptorSetLayoutBinding> &descriptorSetBindings,
        const TempArrayProxy<const std::string> &bindingName,
        const TempArrayProxy<uul::EnumFlags<vul::DescriptorBindingFlagBits>  const> &bindingFlags) {

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
                  return lhs.get().binding < rhs.get().binding;
              });
    m_bindings.clear();
    m_bindings.reserve(descriptorSetBindings.size());
    m_bindingFlags.clear();

    m_nameBindingMap.clear();
    m_nameBindingMap.reserve(descriptorSetBindings.size());
    UUL_ASSERT(bindingFlags.size() == descriptorSetBindings.size() || bindingFlags.size() <= 1, fmt::format(
            "Expected binding flags size to be 0,1 or the same size as descriptorSetBindings: flags {} vs {}",
            bindingFlags.size(), descriptorSetBindings.size()).c_str());

    if (bindingFlags.size() == descriptorSetBindings.size()) {
        m_bindingFlags.reserve(bindingFlags.size());
        for (const auto &idx: idxs) {
            m_nameBindingMap[bindingName[idx]] = static_cast<std::uint32_t>(m_bindings.size());
            m_bindings.push_back(descriptorSetBindings[idx]);
            m_bindingFlags.push_back(bindingFlags[idx]);
        }
    } else { //if (bindingFlags.size() >= 1){  not needed because of assert.
        if (bindingFlags.size() == 1) {
            m_bindingFlags = std::vector<uul::EnumFlags<vul::DescriptorBindingFlagBits> >(descriptorSetBindings.size(), bindingFlags[0]);
        }
        for (const auto &idx: idxs) {
            m_nameBindingMap[bindingName[idx]] = static_cast<std::uint32_t>(m_bindings.size());
            m_bindings.push_back(descriptorSetBindings[idx]);
        }
    }
    auto max_value = *std::max_element(m_bindings.begin(), m_bindings.end(),
                                       [](auto lhs, auto rhs) {
                                           return lhs.get().binding < rhs.get().binding;
                                       });
    UUL_ASSERT(max_value.get().binding == (m_bindings.size() - 1), fmt::format(
            "Expected max_value.binding {} to be the same as binding size {} - 1",
            max_value.get().binding, m_bindings.size()).c_str());
    return *this;
}

vul::DescriptorSetLayoutBuilder& vul::DescriptorSetLayoutBuilder::setBindings(
        const vul::TempArrayProxy<const DescriptorSetLayoutBinding> &descriptorSetBindings) {
    return setBindings(descriptorSetBindings, vul::TempArrayProxy<const uul::EnumFlags<vul::DescriptorBindingFlagBits> >{});
}

vul::DescriptorSetLayoutBuilder& vul::DescriptorSetLayoutBuilder::setBindings(
        const TempArrayProxy<DescriptorSetLayoutBinding const> &descriptorSetBindings,
        const TempArrayProxy<std::string const> &bindingName) {
    return setBindings(descriptorSetBindings, bindingName, vul::TempArrayProxy<const uul::EnumFlags<vul::DescriptorBindingFlagBits> >{});
}


vul::ExpectedResult<vul::DescriptorSetLayout>
vul::DescriptorSetLayoutBuilder::create() const {


    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //TODO figure out what to do with this nullptr
    layoutInfo.pNext = nullptr;
    layoutInfo.flags = m_flags;
    layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
    layoutInfo.pBindings = reinterpret_cast<const VkDescriptorSetLayoutBinding *>(m_bindings.data());

    if (!m_bindingFlags.empty()) {
        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo = {};
        bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        bindingFlagsCreateInfo.pNext = nullptr;
        bindingFlagsCreateInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        bindingFlagsCreateInfo.pBindingFlags = reinterpret_cast<const VkDescriptorBindingFlags *>(m_bindingFlags.data());
        layoutInfo.pNext = &bindingFlagsCreateInfo;

        VkDescriptorSetLayout descriptorSetLayout;
        auto result = static_cast<Result>(vkCreateDescriptorSetLayout(
                m_pDevice->get(), &layoutInfo,
                m_pAllocator, &descriptorSetLayout));
        return {result,
                DescriptorSetLayout(*m_pDevice, descriptorSetLayout, m_pAllocator)};
    } else {

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
    std::unordered_map<DescriptorType, std::uint32_t> descriptor_count_map;
    for (const auto &binding: m_bindings) {
        auto map_itr = descriptor_count_map.find(binding.descriptorType);
        if (map_itr == descriptor_count_map.end()) {
            descriptor_count_map[binding.descriptorType] = binding.descriptorCount;
        } else {
            map_itr->second += binding.descriptorCount;
        }
    }
    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(descriptor_count_map.size());
    for (const auto &[key, value]: descriptor_count_map) {
        pool_sizes.emplace_back(VkDescriptorPoolSize{vul::get(key), value * multiplier});
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

vul::DescriptorSetLayoutBuilder& vul::DescriptorSetLayoutBuilder::setStageFlags(
        uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags) {
    for (auto &binding: m_bindings) {
        binding.stageFlags = stageFlags;
    }
    return *this;
}

vul::DescriptorSetUpdateBuilder
vul::DescriptorSetLayoutBuilder::createUpdateBuilder() const {
    return DescriptorSetUpdateBuilder(m_bindings, m_nameBindingMap);
}

std::vector<vul::DescriptorTypeCount> vul::DescriptorSetLayoutBuilder::createDescriptorTypeCounts() const {
    std::vector<vul::DescriptorTypeCount> result;
    result.reserve(m_bindings.size());
    for (auto &binding: m_bindings) {
        result.emplace_back(binding.descriptorType, binding.descriptorCount);
    }
    return result;
}

vul::DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder() = default;

