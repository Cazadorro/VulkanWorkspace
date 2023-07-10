//
// Created by Bolt on 7/8/2023.
//

#include "writedescriptorset.h"
#include "vul/buffer.h"
#include "vul/imageview.h"
#include "vul/sampler.h"
#include "vul/temparrayproxy.h"
#include <uul/container.h>
#include <fmt/core.h>
//#include <range/v3/view/iota.hpp>
//#include <range/v3/view/concat.hpp>
#include <range/v3/range/conversion.hpp>
//#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/transform.hpp>
//#include <range/v3/view/zip.hpp>


vul::DescriptorBufferInfo::operator VkDescriptorBufferInfo() const noexcept {
    return get();
}

VkDescriptorBufferInfo &vul::DescriptorBufferInfo::get() noexcept {
    return *reinterpret_cast<VkDescriptorBufferInfo *>(this);
}

const VkDescriptorBufferInfo &vul::DescriptorBufferInfo::get() const noexcept {
    return *reinterpret_cast<const VkDescriptorBufferInfo *>(this);
}

vul::DescriptorBufferInfo::DescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) :
        buffer(buffer), offset(offset), range(range) {

}

vul::DescriptorBufferInfo::DescriptorBufferInfo(const vul::Buffer &buffer, VkDeviceSize offset, VkDeviceSize range)
        : DescriptorBufferInfo(buffer.get(), offset, range) {

}

vul::DescriptorImageInfo::operator VkDescriptorImageInfo() const noexcept {
    return get();
}

VkDescriptorImageInfo &vul::DescriptorImageInfo::get() noexcept {
    return *reinterpret_cast<VkDescriptorImageInfo *>(this);
}

const VkDescriptorImageInfo &vul::DescriptorImageInfo::get() const noexcept {
    return *reinterpret_cast<const VkDescriptorImageInfo *>(this);
}

vul::DescriptorImageInfo::DescriptorImageInfo(VkSampler sampler, VkImageView imageView, vul::ImageLayout imageLayout) :
        sampler(sampler), imageView(imageView), imageLayout(imageLayout) {

}


vul::DescriptorSamplerInfo::DescriptorSamplerInfo(VkSampler sampler) : DescriptorImageInfo(sampler, VK_NULL_HANDLE,
                                                                                           {}) {

}

vul::DescriptorSamplerInfo::DescriptorSamplerInfo(const vul::Sampler &sampler) : DescriptorSamplerInfo(sampler.get()) {

}

vul::DescriptorCombinedImageSamplerInfo::DescriptorCombinedImageSamplerInfo(const vul::Sampler &sampler,
                                                                            const vul::ImageView &imageView,
                                                                            vul::ImageLayout imageLayout) :
        DescriptorImageInfo(sampler.get(), imageView.get(), imageLayout) {

}

vul::DescriptorSampledInfo::DescriptorSampledInfo(const vul::ImageView &imageView, vul::ImageLayout imageLayout) :
        DescriptorImageInfo(VK_NULL_HANDLE, imageView.get(), imageLayout) {

}

vul::DescriptorStorageImageInfo::DescriptorStorageImageInfo(const vul::ImageView &imageView,
                                                            vul::ImageLayout imageLayout) :
        DescriptorImageInfo(VK_NULL_HANDLE, imageView.get(), imageLayout) {

}

vul::DescriptorInputAttachmentInfo::DescriptorInputAttachmentInfo(const vul::ImageView &imageView,
                                                                  vul::ImageLayout imageLayout) :
        DescriptorImageInfo(VK_NULL_HANDLE, imageView.get(), imageLayout) {

}

bool vul::validInfoForDescriptorType(vul::DescriptorType descriptorType, VkBufferView bufferView) {
    switch (descriptorType) {
        case DescriptorType::Sampler:
        case DescriptorType::CombinedImageSampler:
        case DescriptorType::SampledImage:
        case DescriptorType::StorageImage:
            return false;
        case DescriptorType::UniformTexelBuffer:
        case DescriptorType::StorageTexelBuffer:
            return true;
        case DescriptorType::UniformBuffer:
        case DescriptorType::StorageBuffer:
        case DescriptorType::UniformBufferDynamic:
        case DescriptorType::StorageBufferDynamic:
        case DescriptorType::InputAttachment:
        case DescriptorType::InlineUniformBlock:
        case DescriptorType::AccelerationStructureKhr:
        case DescriptorType::AccelerationStructureNv:
        case DescriptorType::MutableValve:
        case DescriptorType::SampleWeightImageQcom:
        case DescriptorType::BlockMatchImageQcom:
            return false;
    }
    return false;
}

bool
vul::validInfoForDescriptorType(vul::DescriptorType descriptorType, const vul::DescriptorBufferInfo &descriptorInfo) {
    switch (descriptorType) {
        case DescriptorType::Sampler:
        case DescriptorType::CombinedImageSampler:
        case DescriptorType::SampledImage:
        case DescriptorType::StorageImage:
        case DescriptorType::UniformTexelBuffer:
        case DescriptorType::StorageTexelBuffer:
            return false;
        case DescriptorType::UniformBuffer:
        case DescriptorType::StorageBuffer:
        case DescriptorType::UniformBufferDynamic:
        case DescriptorType::StorageBufferDynamic:
            return true;
        case DescriptorType::InputAttachment:
            return false;
        case DescriptorType::InlineUniformBlock:
            return true;
        case DescriptorType::AccelerationStructureKhr:
        case DescriptorType::AccelerationStructureNv:
        case DescriptorType::MutableValve:
        case DescriptorType::SampleWeightImageQcom:
        case DescriptorType::BlockMatchImageQcom:
            return false;
    }
    return false;
}

bool
vul::validInfoForDescriptorType(vul::DescriptorType descriptorType, const vul::DescriptorImageInfo &descriptorInfo) {
    switch (descriptorType) {
        case DescriptorType::Sampler:
            return descriptorInfo.sampler != VK_NULL_HANDLE && descriptorInfo.imageView == VK_NULL_HANDLE;
        case DescriptorType::CombinedImageSampler:
            return descriptorInfo.sampler != VK_NULL_HANDLE && descriptorInfo.imageView != VK_NULL_HANDLE;
        case DescriptorType::SampledImage:
        case DescriptorType::StorageImage:
            return descriptorInfo.sampler == VK_NULL_HANDLE && descriptorInfo.imageView != VK_NULL_HANDLE;
        case DescriptorType::UniformTexelBuffer:
        case DescriptorType::StorageTexelBuffer:
        case DescriptorType::UniformBuffer:
        case DescriptorType::StorageBuffer:
        case DescriptorType::UniformBufferDynamic:
        case DescriptorType::StorageBufferDynamic:
            return false;
        case DescriptorType::InputAttachment:
            return descriptorInfo.sampler == VK_NULL_HANDLE && descriptorInfo.imageView != VK_NULL_HANDLE;
        case DescriptorType::InlineUniformBlock:
        case DescriptorType::AccelerationStructureKhr:
        case DescriptorType::AccelerationStructureNv:
        case DescriptorType::MutableValve:
        case DescriptorType::SampleWeightImageQcom:
        case DescriptorType::BlockMatchImageQcom:
            return false;
    }
    return false;
}

bool
vul::validInfoForDescriptorType(vul::DescriptorType descriptorType, const vul::DescriptorSamplerInfo &descriptorInfo) {
    return descriptorType == DescriptorType::Sampler;
}

bool vul::validInfoForDescriptorType(vul::DescriptorType descriptorType,
                                     const vul::DescriptorCombinedImageSamplerInfo &descriptorInfo) {
    return descriptorType == DescriptorType::CombinedImageSampler;
}

bool
vul::validInfoForDescriptorType(vul::DescriptorType descriptorType, const vul::DescriptorSampledInfo &descriptorInfo) {
    return descriptorType == DescriptorType::SampledImage;
}

bool
vul::validInfoForDescriptorType(vul::DescriptorType descriptorType,
                                const vul::DescriptorStorageImageInfo &descriptorInfo) {
    return descriptorType == DescriptorType::StorageImage;
}

bool
vul::validInfoForDescriptorType(vul::DescriptorType descriptorType,
                                const vul::DescriptorInputAttachmentInfo &descriptorInfo) {
    return descriptorType == DescriptorType::InputAttachment;
}


vul::WriteDescriptorSet::WriteDescriptorSet() :
        pNext(nullptr),
        dstSet(VK_NULL_HANDLE),
        dstBinding(0),
        dstArrayElement(0),
        descriptorCount(0),
        descriptorType(vul::DescriptorType{}),
        pImageInfo(nullptr),
        pBufferInfo(nullptr),
        pTexelBufferView(nullptr) {

}

vul::WriteDescriptorSet::WriteDescriptorSet(const void *pNext, VkDescriptorSet dstSet, std::uint32_t dstBinding,
                                            std::uint32_t dstArrayElement, std::uint32_t descriptorCount,
                                            vul::DescriptorType descriptorType,
                                            const vul::DescriptorImageInfo *pImageInfo,
                                            const vul::DescriptorBufferInfo *pBufferInfo,
                                            const VkBufferView *pTexelBufferView)
        : pNext(pNext),
          dstSet(dstSet),
          dstBinding(dstBinding),
          dstArrayElement(dstArrayElement),
          descriptorCount(descriptorCount),
          descriptorType(descriptorType),
          pImageInfo(pImageInfo),
          pBufferInfo(pBufferInfo),
          pTexelBufferView(pTexelBufferView) {

}

vul::WriteDescriptorSet::WriteDescriptorSet(const vul::WriteDescriptorSet &rhs) noexcept:
        pNext(rhs.pNext),
        dstSet(rhs.dstSet),
        dstBinding(rhs.dstBinding),
        dstArrayElement(rhs.dstArrayElement),
        descriptorCount(rhs.descriptorCount),
        descriptorType(rhs.descriptorType),
        pImageInfo(rhs.pImageInfo),
        pBufferInfo(rhs.pBufferInfo),
        pTexelBufferView(rhs.pTexelBufferView) {

}

vul::WriteDescriptorSet::WriteDescriptorSet(vul::WriteDescriptorSet &&rhs) noexcept:
        pNext(rhs.pNext),
        dstSet(rhs.dstSet),
        dstBinding(rhs.dstBinding),
        dstArrayElement(rhs.dstArrayElement),
        descriptorCount(rhs.descriptorCount),
        descriptorType(rhs.descriptorType),
        pImageInfo(rhs.pImageInfo),
        pBufferInfo(rhs.pBufferInfo),
        pTexelBufferView(rhs.pTexelBufferView) {

}

vul::WriteDescriptorSet &vul::WriteDescriptorSet::operator=(const vul::WriteDescriptorSet &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        dstSet = rhs.dstSet;
        dstBinding = rhs.dstBinding;
        dstArrayElement = rhs.dstArrayElement;
        descriptorCount = rhs.descriptorCount;
        descriptorType = rhs.descriptorType;
        pImageInfo = rhs.pImageInfo;
        pBufferInfo = rhs.pBufferInfo;
        pTexelBufferView = rhs.pTexelBufferView;
    }
    return *this;
}

vul::WriteDescriptorSet::~WriteDescriptorSet() = default;

vul::WriteDescriptorSet::operator VkWriteDescriptorSet() const noexcept {
    return get();
}

VkWriteDescriptorSet &vul::WriteDescriptorSet::get() noexcept {
    return *reinterpret_cast<VkWriteDescriptorSet *>(this);
}

const VkWriteDescriptorSet &vul::WriteDescriptorSet::get() const noexcept {
    return *reinterpret_cast<const VkWriteDescriptorSet *>(this);
}

vul::WriteDescriptorSet::WriteDescriptorSet(VkDescriptorSet dstSet, vul::DescriptorType descriptorType,
                                            const vul::DescriptorImageInfo *pImageInfo, std::uint32_t dstBinding,
                                            std::uint32_t dstArrayElement, std::uint32_t descriptorCount) :
        WriteDescriptorSet(nullptr, dstSet, dstBinding, dstArrayElement, descriptorCount, descriptorType, pImageInfo,
                           nullptr, nullptr) {

}

vul::WriteDescriptorSet::WriteDescriptorSet(VkDescriptorSet dstSet, vul::DescriptorType descriptorType,
                                            const vul::DescriptorBufferInfo *pBufferInfo, std::uint32_t dstBinding,
                                            std::uint32_t dstArrayElement, std::uint32_t descriptorCount) :
        WriteDescriptorSet(nullptr, dstSet, dstBinding, dstArrayElement, descriptorCount, descriptorType, nullptr,
                           pBufferInfo, nullptr) {

}

vul::WriteDescriptorSet::WriteDescriptorSet(VkDescriptorSet dstSet, vul::DescriptorType descriptorType,
                                            const VkBufferView *pTexelBufferView, std::uint32_t dstBinding,
                                            std::uint32_t dstArrayElement, std::uint32_t descriptorCount) :
        WriteDescriptorSet(nullptr, dstSet, dstBinding, dstArrayElement, descriptorCount, descriptorType, nullptr,
                           nullptr, pTexelBufferView) {

}

vul::WriteDescriptorSet::WriteDescriptorSet(VkDescriptorSet dstSet, vul::DescriptorType descriptorType,
                                            const void *pNext, std::uint32_t dstBinding, std::uint32_t dstArrayElement,
                                            std::uint32_t descriptorCount) :
        WriteDescriptorSet(pNext, dstSet, dstBinding, dstArrayElement, descriptorCount, descriptorType, nullptr,
                           nullptr, nullptr) {

}

vul::WriteDescriptorSetBuilder::WriteDescriptorSetBuilder() = default;

vul::WriteDescriptorSetBuilder::WriteDescriptorSetBuilder(VkDescriptorSet descriptorSet,
                                                          std::vector<DescriptorTypeCount> m_descriptorTypeCounts) :
        m_descriptorSet(descriptorSet),
        m_descriptorTypeCounts(std::move(m_descriptorTypeCounts)) {}

vul::WriteDescriptorSetBuilder::Binding vul::WriteDescriptorSetBuilder::operator[](std::uint32_t bindingIndex) {
    return {this, bindingIndex};
}

void vul::WriteDescriptorSetBuilder::clear() {
    m_writeDescriptorSets.clear();
}

std::uint32_t vul::WriteDescriptorSetBuilder::size() const {
    return static_cast<std::uint32_t>(m_writeDescriptorSets.size());
}

bool vul::WriteDescriptorSetBuilder::empty() const {
    return m_writeDescriptorSets.empty();
}

vul::WriteDescriptorSetBuilder &vul::WriteDescriptorSetBuilder::append(vul::WriteDescriptorSet writeDescriptorSet) {
    m_writeDescriptorSets.push_back(writeDescriptorSet);
    return *this;
}

vul::DescriptorTypeCount vul::WriteDescriptorSetBuilder::getDescriptorTypeCountAt(std::uint32_t bindingIndex) const {
    return m_descriptorTypeCounts.at(bindingIndex);
}

std::span<const vul::WriteDescriptorSet> vul::WriteDescriptorSetBuilder::getWrites() const {
    return {m_writeDescriptorSets.begin(), m_writeDescriptorSets.end()};
}


vul::WriteDescriptorSetBuilder::Binding::ArrayElement
vul::WriteDescriptorSetBuilder::Binding::operator[](std::uint32_t arrayIndex) {
    return {m_parent, m_bindingIndex, arrayIndex};
}

vul::WriteDescriptorSetBuilder::Binding::Binding(vul::WriteDescriptorSetBuilder *parent,
                                                 std::uint32_t bindingIndex) :
        m_parent(parent), m_bindingIndex(bindingIndex) {

}




vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::operator=(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos) {
    return set(descriptorInfos);
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::operator=(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos) {
    return set(descriptorInfos);
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::operator=(vul::TempArrayProxy<VkBufferView> bufferViews) {
    return set(bufferViews);
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::operator=(vul::TempArrayProxy<const Buffer  *> buffers) {
    return set(buffers);
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::operator=(vul::TempArrayProxy<const ImageView  *> imageViews) {
    return set(imageViews);
}
vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::operator=(vul::TempArrayProxy<const Sampler *> samplers) {
    return set(samplers);
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos) {
    (*this)[0].set(descriptorInfos);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos) {
    (*this)[0].set(descriptorInfos);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<VkBufferView> bufferViews) {
    (*this)[0].set(bufferViews);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<const Buffer  *> buffers, VkDeviceSize offset,
                                             VkDeviceSize range) {
    (*this)[0].set(buffers, offset, range);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<const ImageView  *> imageViews, const vul::Sampler & sampler,
                                             vul::ImageLayout imageLayout) {
    (*this)[0].set(imageViews, sampler, imageLayout);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<const ImageView  *> imageViews, const vul::Sampler & sampler) {
    (*this)[0].set(imageViews, sampler);
    return *this;
}
vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<const ImageView  *> imageViews, vul::ImageLayout imageLayout) {
    (*this)[0].set(imageViews, imageLayout);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<const ImageView  *> imageViews) {
    (*this)[0].set(imageViews);
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding &
vul::WriteDescriptorSetBuilder::Binding::set(vul::TempArrayProxy<const Sampler *> samplers) {
    (*this)[0].set(samplers);
    return *this;
}







vul::WriteDescriptorSetBuilder::Binding::ArrayElement::ArrayElement(vul::WriteDescriptorSetBuilder *parent,
                                                                    std::uint32_t bindingIndex,
                                                                    std::uint32_t arrayIndex) : m_parent(parent), m_bindingIndex(bindingIndex), m_arrayIndex(arrayIndex) {

}



vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::operator=(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos) {
    return set(descriptorInfos);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::operator=(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos) {
    return set(descriptorInfos);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::operator=(vul::TempArrayProxy<VkBufferView> bufferViews) {
    return set(bufferViews);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::operator=(vul::TempArrayProxy<const Buffer  *> buffers) {
    return set(buffers);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::operator=(vul::TempArrayProxy<const ImageView  *> imageViews) {
    return set(imageViews);
}
vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::operator=(vul::TempArrayProxy<const Sampler *> samplers) {
    return set(samplers);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos) {
    auto descriptorTypeCount = m_parent->getDescriptorTypeCountAt(m_bindingIndex);
    UUL_ASSERT(descriptorTypeCount.count >= (descriptorInfos.size() + m_arrayIndex) && !descriptorInfos.empty(),
               fmt::format("Number of array elements written larger than"
                           "DescriptorSetLayout array element count: expected {} vs {}",
                           descriptorTypeCount.count,
                           descriptorInfos.size()).c_str());

    UUL_ASSERT(validInfoForDescriptorType(descriptorTypeCount.type, descriptorInfos[0]),
               fmt::format("DescriptorBufferInfo input not valid for type {}",
                           vul::to_string(descriptorTypeCount.type)).c_str());
    auto offset_data_ptr = &(*uul::append_offset(m_parent->m_bufferInfos, descriptorInfos));
    m_parent->append(WriteDescriptorSet(m_parent->m_descriptorSet, descriptorTypeCount.type, offset_data_ptr,
                                        m_bindingIndex, m_arrayIndex, descriptorInfos.size()));
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos) {
    auto descriptorTypeCount = m_parent->getDescriptorTypeCountAt(m_bindingIndex);
    UUL_ASSERT(descriptorTypeCount.count >=  (descriptorInfos.size() + m_arrayIndex)  && !descriptorInfos.empty(),
               fmt::format("Number of array elements written larger than"
                           "DescriptorSetLayout array element count: expected {} vs {}",
                           descriptorTypeCount.count,
                           descriptorInfos.size()).c_str());

    UUL_ASSERT(validInfoForDescriptorType(descriptorTypeCount.type, descriptorInfos[0]),
               fmt::format("DescriptorImageInfo input not valid for type {}",
                           vul::to_string(descriptorTypeCount.type)).c_str());
    auto offset_data_ptr = &(*uul::append_offset(m_parent->m_imageInfos, descriptorInfos));
    m_parent->append(WriteDescriptorSet(m_parent->m_descriptorSet, descriptorTypeCount.type, offset_data_ptr,
                                        m_bindingIndex, m_arrayIndex, descriptorInfos.size()));
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<VkBufferView> bufferViews) {
    auto descriptorTypeCount = m_parent->getDescriptorTypeCountAt(m_bindingIndex);
    UUL_ASSERT(descriptorTypeCount.count >= (bufferViews.size() + m_arrayIndex)  && !bufferViews.empty(),
               fmt::format("Number of array elements written larger than"
                           "DescriptorSetLayout array element count: expected {} vs {}",
                           descriptorTypeCount.count,
                           bufferViews.size()).c_str());

    UUL_ASSERT(validInfoForDescriptorType(descriptorTypeCount.type, bufferViews[0]),
               fmt::format("VkBufferView input not valid for type {}",
                           vul::to_string(descriptorTypeCount.type)).c_str());
    auto offset_data_ptr = &(*uul::append_offset(m_parent->m_bufferViews, bufferViews));
    m_parent->append(WriteDescriptorSet(m_parent->m_descriptorSet, descriptorTypeCount.type, offset_data_ptr,
                                        m_bindingIndex, m_arrayIndex, bufferViews.size()));
    return *this;
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<const Buffer  *> buffers, VkDeviceSize offset,
                                             VkDeviceSize range) {
    auto descriptorInfos = buffers | ranges::views::transform([offset, range](const auto& value){
        return DescriptorBufferInfo(*value, offset, range);
    }) | ranges::to<std::vector>();
    return set(descriptorInfos);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<const ImageView  *> imageViews, const vul::Sampler & sampler,
                                             vul::ImageLayout imageLayout) {
    auto descriptorType = m_parent->getDescriptorTypeCountAt(m_bindingIndex).type;
    UUL_ASSERT(descriptorType == vul::DescriptorType::CombinedImageSampler,
               fmt::format("CombinedImageSampler expected, Unexpected imageviews with sampler, when descriptor type is {}",
                           vul::to_string(descriptorType)).c_str());
    auto descriptorInfos = imageViews | ranges::views::transform([&sampler, imageLayout](const auto& value){
        return  static_cast<DescriptorImageInfo>(DescriptorCombinedImageSamplerInfo(sampler, *value, imageLayout));
    }) | ranges::to<std::vector>();
    return set(descriptorInfos);
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<const ImageView  *> imageViews, const vul::Sampler & sampler) {
    auto descriptorType = m_parent->getDescriptorTypeCountAt(m_bindingIndex).type;
    UUL_ASSERT(descriptorType == vul::DescriptorType::CombinedImageSampler,
               fmt::format("CombinedImageSampler expected, Unexpected imageviews with sampler, when descriptor type is {}",
                           vul::to_string(descriptorType)).c_str());
    auto descriptorInfos = imageViews | ranges::views::transform([&sampler](const auto& value){
        return  static_cast<DescriptorImageInfo>(DescriptorCombinedImageSamplerInfo(sampler, *value));
    }) | ranges::to<std::vector>();
    return set(descriptorInfos);
}
vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<const ImageView  *> imageViews, vul::ImageLayout imageLayout) {
    auto descriptorType = m_parent->getDescriptorTypeCountAt(m_bindingIndex).type;
    switch(descriptorType){
        case DescriptorType::SampledImage: {
            auto descriptorInfos = imageViews | ranges::views::transform([imageLayout](const auto &value) {
                return static_cast<DescriptorImageInfo>(DescriptorSampledInfo(*value, imageLayout));
            }) | ranges::to<std::vector>();
            return set(descriptorInfos);
        }
        case DescriptorType::StorageImage: {
            auto descriptorInfos = imageViews | ranges::views::transform([imageLayout](const auto &value) {
                return static_cast<DescriptorImageInfo>(DescriptorStorageImageInfo(*value, imageLayout));
            }) | ranges::to<std::vector>();
            return set(descriptorInfos);
        }
        case DescriptorType::InputAttachment: {
            auto descriptorInfos = imageViews | ranges::views::transform([imageLayout](const auto &value) {
                return static_cast<DescriptorImageInfo>(DescriptorInputAttachmentInfo(*value, imageLayout));
            }) | ranges::to<std::vector>();
            return set(descriptorInfos);
        }
        default:
            UUL_ASSERT(false,
                       fmt::format("SampledImage, StorageImage, or InputAttachment expected,"
                                   "Unexpected input ImageView without sampler, when descriptor type is {}",
                                   vul::to_string(descriptorType)).c_str());
    }
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<const ImageView  *> imageViews) {
    auto descriptorType = m_parent->getDescriptorTypeCountAt(m_bindingIndex).type;
    switch(descriptorType){
        case DescriptorType::SampledImage: {
            auto descriptorInfos = imageViews | ranges::views::transform([](const auto &value) {
                return  static_cast<DescriptorImageInfo>(DescriptorSampledInfo(*value));
            }) | ranges::to<std::vector>();
            return set(descriptorInfos);
        }
        case DescriptorType::StorageImage: {
            auto descriptorInfos = imageViews | ranges::views::transform([](const auto &value) {
                return static_cast<DescriptorImageInfo>(DescriptorStorageImageInfo(*value));
            }) | ranges::to<std::vector>();
            return set(descriptorInfos);
        }
        case DescriptorType::InputAttachment: {
            auto descriptorInfos = imageViews | ranges::views::transform([](const auto &value) {
                return static_cast<DescriptorImageInfo>(DescriptorInputAttachmentInfo(*value));
            }) | ranges::to<std::vector>();
            return set(descriptorInfos);
        }
        default:
            UUL_ASSERT(false,
                       fmt::format("SampledImage, StorageImage, or InputAttachment expected,"
                                   "Unexpected input ImageView without sampler, when descriptor type is {}",
                                   vul::to_string(descriptorType)).c_str());
    }
}

vul::WriteDescriptorSetBuilder::Binding::ArrayElement &
vul::WriteDescriptorSetBuilder::Binding::ArrayElement::set(vul::TempArrayProxy<const Sampler *> samplers) {
    auto descriptorType = m_parent->getDescriptorTypeCountAt(m_bindingIndex).type;
    UUL_ASSERT(descriptorType == vul::DescriptorType::Sampler,
               fmt::format("Sampler expected, Unexpected Sampler with out image, when descriptor type is {}",
                           vul::to_string(descriptorType)).c_str());
    auto descriptorInfos = samplers | ranges::views::transform([](const auto value){
        return  static_cast<DescriptorImageInfo>(DescriptorSamplerInfo(value->get()));
    }) | ranges::to<std::vector>();
    return set(descriptorInfos);
}


