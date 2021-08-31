//
// Created by Shae Bolt on 8/29/2021.
//

#include "descriptorset.h"
#include "vkassert.h"


vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::Sampler(
        const std::vector<VkDescriptorImageInfo> &imageInfos,
        const void *pNext) {
    for (const auto &imageInfo :imageInfos) {
        VUL_ASSERT(imageInfo.sampler != VK_NULL_HANDLE,
                   "imageInfo.sampler must not be empty");
        VUL_DEBUG_WARNING(imageInfo.imageView == VK_NULL_HANDLE,
                          "imageInfo.imageView is not used in sampler descriptor write");
        VUL_DEBUG_WARNING(imageInfo.imageLayout == VK_IMAGE_LAYOUT_UNDEFINED,
                          "imageInfo.imageLayout is not used in sampler descriptor write");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::Sampler;
    info.m_infoVariant = imageInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::CombinedImageSampler(
        const std::vector<VkDescriptorImageInfo> &imageInfos,
        const void *pNext) {
    for (const auto &imageInfo :imageInfos) {
        VUL_ASSERT(imageInfo.sampler != VK_NULL_HANDLE,
                   "imageInfo.sampler must not be empty");
        VUL_DEBUG_WARNING(imageInfo.imageView != VK_NULL_HANDLE,
                          "imageInfo.imageView must not be empty");
        VUL_DEBUG_WARNING(imageInfo.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED,
                          "imageInfo.imageLayout must not be empty");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::CombinedImageSampler;
    info.m_infoVariant = imageInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::SampledImage(
        const std::vector<VkDescriptorImageInfo> &imageInfos,
        const void *pNext) {
    for (const auto &imageInfo :imageInfos) {
        VUL_DEBUG_WARNING(imageInfo.sampler != VK_NULL_HANDLE,
                          "imageInfo.sampler is not used in VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE");
        VUL_ASSERT(imageInfo.imageView != VK_NULL_HANDLE,
                   "imageInfo.imageView must not be empty");
        VUL_ASSERT(imageInfo.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED,
                   "imageInfo.imageLayout must not be empty");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::SampledImage;
    info.m_infoVariant = imageInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::StorageImage(
        const std::vector<VkDescriptorImageInfo> &imageInfos,
        const void *pNext) {
    for (const auto &imageInfo :imageInfos) {
        VUL_DEBUG_WARNING(imageInfo.sampler != VK_NULL_HANDLE,
                          "imageInfo.sampler is not used in VK_DESCRIPTOR_TYPE_STORAGE_IMAGE");
        VUL_ASSERT(imageInfo.imageView != VK_NULL_HANDLE,
                   "imageInfo.imageView must not be empty");
        VUL_ASSERT(imageInfo.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED,
                   "imageInfo.imageLayout must not be empty");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::StorageImage;
    info.m_infoVariant = imageInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::UniformTexelBuffer(
        const std::vector<VkBufferView> &texelBufferViews, const void *pNext) {
    for (const auto &texelBufferView :texelBufferViews) {
        VUL_ASSERT(texelBufferView != VK_NULL_HANDLE,
                   "texelBufferView must not be VK_NULL_HANDLE");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::UniformTexelBuffer;
    info.m_infoVariant = texelBufferViews;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::StorageTexelBuffer(
        const std::vector<VkBufferView> &texelBufferViews, const void *pNext) {
    for (const auto &texelBufferView :texelBufferViews) {
        VUL_ASSERT(texelBufferView != VK_NULL_HANDLE,
                   "texelBufferView must not be VK_NULL_HANDLE");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::StorageTexelBuffer;
    info.m_infoVariant = texelBufferViews;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::UniformBuffer(
        const std::vector<VkDescriptorBufferInfo> &bufferInfos,
        const void *pNext) {
    for (const auto &bufferInfo :bufferInfos) {
        VUL_ASSERT(bufferInfo.buffer != VK_NULL_HANDLE,
                   "bufferInfo.buffer must not be VK_NULL_HANDLE");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::UniformBuffer;
    info.m_infoVariant = bufferInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::StorageBuffer(
        const std::vector<VkDescriptorBufferInfo> &bufferInfos,
        const void *pNext) {
    for (const auto &bufferInfo :bufferInfos) {
        VUL_ASSERT(bufferInfo.buffer != VK_NULL_HANDLE,
                   "bufferInfo.buffer must not be VK_NULL_HANDLE");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::StorageBuffer;
    info.m_infoVariant = bufferInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::UniformBufferDynamic(
        const std::vector<VkDescriptorBufferInfo> &bufferInfos,
        const void *pNext) {
    for (const auto &bufferInfo :bufferInfos) {
        VUL_ASSERT(bufferInfo.buffer != VK_NULL_HANDLE,
                   "bufferInfo.buffer must not be VK_NULL_HANDLE");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::UniformBufferDynamic;
    info.m_infoVariant = bufferInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::StorageBufferDynamic(
        const std::vector<VkDescriptorBufferInfo> &bufferInfos,
        const void *pNext) {
    for (const auto &bufferInfo :bufferInfos) {
        VUL_ASSERT(bufferInfo.buffer != VK_NULL_HANDLE,
                   "bufferInfo.buffer must not be VK_NULL_HANDLE");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::StorageBufferDynamic;
    info.m_infoVariant = bufferInfos;
    info.m_pNext = pNext;
    return info;
}

vul::WriteDescriptorSetInfo vul::WriteDescriptorSetInfo::InputAttachment(
        const std::vector<VkDescriptorImageInfo> &imageInfos,
        const void *pNext) {
    for (const auto &imageInfo :imageInfos) {
        VUL_DEBUG_WARNING(imageInfo.sampler != VK_NULL_HANDLE,
                          "imageInfo.sampler is not used in VK_DESCRIPTOR_TYPE_STORAGE_IMAGE");
        VUL_ASSERT(imageInfo.imageView != VK_NULL_HANDLE,
                   "imageInfo.imageView must not be empty");
        VUL_ASSERT(imageInfo.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED,
                   "imageInfo.imageLayout must not be empty");
    }
    WriteDescriptorSetInfo info;
    info.m_descriptorType = vul::DescriptorType::InputAttachment;
    info.m_infoVariant = imageInfos;
    info.m_pNext = pNext;
    return info;
}

VkWriteDescriptorSet
vul::WriteDescriptorSetInfo::createWriteDescriptorSet(VkDescriptorSet dstSet,
                                                      std::uint32_t dstBinding,
                                                      std::uint32_t dstArrayElement) const {
    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = m_pNext;
    writeDescriptorSet.dstSet = dstSet;
    writeDescriptorSet.dstBinding = dstBinding;
    writeDescriptorSet.dstArrayElement = dstArrayElement;
    writeDescriptorSet.descriptorType = vul::get(m_descriptorType);

    switch (m_infoVariant.index()) {
        case 0: {
            auto &infoArray = std::get<std::vector<VkDescriptorImageInfo>>(
                    m_infoVariant);
            writeDescriptorSet.descriptorCount = infoArray.size();
            writeDescriptorSet.pImageInfo = infoArray.data();
            break;
        }
        case 1: {
            auto &infoArray = std::get<std::vector<VkDescriptorBufferInfo>>(
                    m_infoVariant);
            writeDescriptorSet.descriptorCount = infoArray.size();
            writeDescriptorSet.pBufferInfo = infoArray.data();
            break;
        }
        case 2: {
            auto &infoArray = std::get<std::vector<VkBufferView>>(
                    m_infoVariant);
            writeDescriptorSet.descriptorCount = infoArray.size();
            writeDescriptorSet.pTexelBufferView = infoArray.data();
            break;
        }
    }
    return writeDescriptorSet;
}

vul::WriteDescriptorSetInfo &vul::WriteDescriptorSetInfo::operator=(
        vul::WriteDescriptorSetInfo &&rhs) noexcept {
    VUL_ASSERT(m_descriptorType == rhs.m_descriptorType);
    using std::swap;
    std::swap(m_infoVariant, rhs.m_infoVariant);
    std::swap( m_pNext, rhs.m_pNext);
    return *this;
}

vul::WriteDescriptorSetInfo &
vul::WriteDescriptorSetInfo::operator=(const vul::WriteDescriptorSetInfo &rhs) {
    if(&rhs != this){
        VUL_ASSERT(m_descriptorType == rhs.m_descriptorType);
        m_infoVariant = rhs.m_infoVariant;
        m_pNext = rhs.m_pNext;
    }
    return *this;
}

vul::DescriptorSetUpdateBuilder::DescriptorSetUpdateBuilder(
        const gsl::span<const VkDescriptorSetLayoutBinding> &bindings,
        const std::unordered_map<std::string, std::uint32_t> &nameBindingMap)
        : m_nameBindingMap(nameBindingMap) {

    VUL_ASSERT(
            bindings.size() == nameBindingMap.size() || nameBindingMap.empty(),
            "Expected name binding map size to either match bindings or be empty");
    m_descriptorTypes.reserve(bindings.size());
    m_writeDescriptorInfos.reserve(bindings.size());
    for (const auto &binding : bindings) {
        m_descriptorTypes.push_back(
                static_cast<vul::DescriptorType>(binding.descriptorType));
        m_writeDescriptorInfos.emplace_back(binding.descriptorCount);
        for(auto& info : m_writeDescriptorInfos.back()){
            info.m_descriptorType = static_cast<vul::DescriptorType>(binding.descriptorType);
        }
    }
}

std::vector<vul::WriteDescriptorSetInfo> &
vul::DescriptorSetUpdateBuilder::getDescriptorArrayAt(
        std::uint32_t bindingIndex) {
    return m_writeDescriptorInfos.at(bindingIndex);
}

const std::vector<vul::WriteDescriptorSetInfo> &
vul::DescriptorSetUpdateBuilder::getDescriptorArrayAt(
        std::uint32_t bindingIndex) const {
    return m_writeDescriptorInfos.at(bindingIndex);
}

std::vector<vul::WriteDescriptorSetInfo> &
vul::DescriptorSetUpdateBuilder::getDescriptorArrayAt(
        const std::string_view &bindingName) {
    return getDescriptorArrayAt(m_nameBindingMap.at(std::string(bindingName)));
}

const std::vector<vul::WriteDescriptorSetInfo> &
vul::DescriptorSetUpdateBuilder::getDescriptorArrayAt(
        const std::string_view &bindingName) const {
    return getDescriptorArrayAt(m_nameBindingMap.at(std::string(bindingName)));
}

vul::WriteDescriptorSetInfo &
vul::DescriptorSetUpdateBuilder::getDescriptorElementAt(
        std::uint32_t bindingIndex, std::uint32_t elementIndex) {
    return getDescriptorArrayAt(bindingIndex)[elementIndex];
}

const vul::WriteDescriptorSetInfo &
vul::DescriptorSetUpdateBuilder::getDescriptorElementAt(
        std::uint32_t bindingIndex, std::uint32_t elementIndex) const {
    return getDescriptorArrayAt(bindingIndex)[elementIndex];
}

vul::WriteDescriptorSetInfo &
vul::DescriptorSetUpdateBuilder::getDescriptorElementAt(
        const std::string_view &bindingName, std::uint32_t elementIndex) {
    return getDescriptorArrayAt(bindingName)[elementIndex];
}

const vul::WriteDescriptorSetInfo &
vul::DescriptorSetUpdateBuilder::getDescriptorElementAt(
        const std::string_view &bindingName, std::uint32_t elementIndex) const {
    return getDescriptorArrayAt(bindingName)[elementIndex];
}

std::vector<VkWriteDescriptorSet> vul::DescriptorSetUpdateBuilder::create(
        VkDescriptorSet const &dstSet) const {
    std::vector<VkWriteDescriptorSet> descriptorSetWrites;
    descriptorSetWrites.reserve(m_writeDescriptorInfos.size());
    for (std::uint32_t dstBinding = 0;
         dstBinding < m_writeDescriptorInfos.size(); ++dstBinding) {
        for (std::uint32_t dstArrayElement = 0; dstArrayElement <
                                                m_writeDescriptorInfos[dstBinding].size(); ++dstArrayElement) {
            descriptorSetWrites.push_back(
                    m_writeDescriptorInfos[dstBinding][dstArrayElement].createWriteDescriptorSet(
                            dstSet, dstBinding, dstArrayElement));
        }
    }
    return descriptorSetWrites;
}
