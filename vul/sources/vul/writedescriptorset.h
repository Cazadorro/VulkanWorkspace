//
// Created by Bolt on 7/8/2023.
//

#ifndef VULKANWORKSPACE_WRITEDESCRIPTORSET_H
#define VULKANWORKSPACE_WRITEDESCRIPTORSET_H

#include "vul/enums.h"
#include "vul/temparrayproxyfwd.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <span>

namespace vul {

    //TODO it's actually the descriptor info that needs type safety!
    //    enum class DescriptorType{
    //        Sampler = VK_DESCRIPTOR_TYPE_SAMPLER,
    //        CombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    //        SampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    //        StorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    //        UniformTexelBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    //        StorageTexelBuffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    //        UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //        StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    //        UniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    //        StorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    //        InputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    //        InlineUniformBlock = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
    //        InlineUniformBlockExt = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
    //        AccelerationStructureKhr = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
    //        AccelerationStructureNv = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV,
    //        MutableValve = VK_DESCRIPTOR_TYPE_MUTABLE_VALVE,
    //        SampleWeightImageQcom = VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM,
    //        BlockMatchImageQcom = VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM,
    //        MutableExt = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
    //    };




    class Buffer;

    class ImageView;

    class Sampler;


    class DescriptorBufferInfo {
    public:
        VkBuffer buffer;
        VkDeviceSize offset;
        VkDeviceSize range;

        explicit DescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

        explicit DescriptorBufferInfo(const Buffer &buffer, VkDeviceSize offset = 0,
                                      VkDeviceSize range = VK_WHOLE_SIZE);

        operator VkDescriptorBufferInfo() const noexcept;

        [[nodiscard]]
        VkDescriptorBufferInfo &get() noexcept;

        [[nodiscard]]
        const VkDescriptorBufferInfo &get() const noexcept;

    private:
    };


    //no need for setUniformBuffer, setStorageBuffer, setUniformBufferDynamic, or setStorageBufferDynamic

    class DescriptorImageInfo {
    public:
        VkSampler sampler;
        VkImageView imageView;
        vul::ImageLayout imageLayout;

        DescriptorImageInfo(VkSampler sampler, VkImageView imageView, vul::ImageLayout imageLayout);

        operator VkDescriptorImageInfo() const noexcept;

        [[nodiscard]]
        VkDescriptorImageInfo &get() noexcept;

        [[nodiscard]]
        const VkDescriptorImageInfo &get() const noexcept;

    private:
    };

    class DescriptorSamplerInfo : public DescriptorImageInfo {
    public:
        explicit DescriptorSamplerInfo(VkSampler sampler);

        explicit DescriptorSamplerInfo(const Sampler &sampler);

    private:
    };

    class DescriptorCombinedImageSamplerInfo : public DescriptorImageInfo {
    public:
        explicit DescriptorCombinedImageSamplerInfo(const Sampler &sampler, const ImageView &imageView,
                                                    vul::ImageLayout imageLayout = vul::ImageLayout::ShaderReadOnlyOptimal);

    private:
    };


    class DescriptorSampledInfo : public DescriptorImageInfo {
    public:
        explicit DescriptorSampledInfo(const ImageView &imageView,
                                       vul::ImageLayout imageLayout = vul::ImageLayout::ShaderReadOnlyOptimal);

    private:
    };


    class DescriptorStorageImageInfo : public DescriptorImageInfo {
    public:
        explicit DescriptorStorageImageInfo(const ImageView &imageView,
                                            vul::ImageLayout imageLayout = vul::ImageLayout::General);

    private:
    };


    class DescriptorInputAttachmentInfo : public DescriptorImageInfo {
    public:
        explicit DescriptorInputAttachmentInfo(const ImageView &imageView,
                                               vul::ImageLayout imageLayout = vul::ImageLayout::ShaderReadOnlyOptimal);

    private:
    };
    //setUniformTexelBuffer, setStorageTexelBuffer, not needed though?




    [[nodiscard]]
    bool validInfoForDescriptorType(vul::DescriptorType descriptorType, VkBufferView bufferView);

    [[nodiscard]]
    bool validInfoForDescriptorType(vul::DescriptorType descriptorType, const DescriptorBufferInfo &descriptorInfo);

    [[nodiscard]]
    bool validInfoForDescriptorType(vul::DescriptorType descriptorType, const DescriptorImageInfo &descriptorInfo);

    [[nodiscard]]
    bool validInfoForDescriptorType(vul::DescriptorType descriptorType, const DescriptorSamplerInfo &descriptorInfo);

    [[nodiscard]]
    bool validInfoForDescriptorType(vul::DescriptorType descriptorType,
                                    const DescriptorCombinedImageSamplerInfo &descriptorInfo);

    [[nodiscard]]
    bool validInfoForDescriptorType(vul::DescriptorType descriptorType, const DescriptorSampledInfo &descriptorInfo);

    [[nodiscard]]
    bool
    validInfoForDescriptorType(vul::DescriptorType descriptorType, const DescriptorStorageImageInfo &descriptorInfo);

    [[nodiscard]]
    bool
    validInfoForDescriptorType(vul::DescriptorType descriptorType, const DescriptorInputAttachmentInfo &descriptorInfo);


    //typedef struct VkDescriptorBufferInfo {
    //    VkBuffer        buffer;
    //    VkDeviceSize    offset;
    //    VkDeviceSize    range;
    //} VkDescriptorBufferInfo;
    //
    //typedef struct VkDescriptorImageInfo {
    //    VkSampler        sampler;
    //    VkImageView      imageView;
    //    VkImageLayout    imageLayout;
    //} VkDescriptorImageInfo;

    //typedef struct VkCopyDescriptorSet {
    //    VkStructureType    sType;
    //    const void*        pNext;
    //    VkDescriptorSet    srcSet;
    //    uint32_t           srcBinding;
    //    uint32_t           srcArrayElement;
    //    VkDescriptorSet    dstSet;
    //    uint32_t           dstBinding;
    //    uint32_t           dstArrayElement;
    //    uint32_t           descriptorCount;
    //} VkCopyDescriptorSet;
    class WriteDescriptorSet {
    private:
        const vul::StructureType sType = vul::StructureType::WriteDescriptorSet;
    public:
        const void *pNext;
        VkDescriptorSet dstSet;
        std::uint32_t dstBinding;
        std::uint32_t dstArrayElement;
        std::uint32_t descriptorCount;
        vul::DescriptorType descriptorType;
        const DescriptorImageInfo *pImageInfo;
        const DescriptorBufferInfo *pBufferInfo;
        const VkBufferView *pTexelBufferView;

        WriteDescriptorSet();

        WriteDescriptorSet(const void *pNext,
                           VkDescriptorSet dstSet,
                           std::uint32_t dstBinding,
                           std::uint32_t dstArrayElement,
                           std::uint32_t descriptorCount,
                           vul::DescriptorType descriptorType,
                           const DescriptorImageInfo *pImageInfo,
                           const DescriptorBufferInfo *pBufferInfo,
                           const VkBufferView *pTexelBufferView);


        WriteDescriptorSet(VkDescriptorSet dstSet,
                           vul::DescriptorType descriptorType,
                           const DescriptorImageInfo *pImageInfo,
                           std::uint32_t dstBinding,
                           std::uint32_t dstArrayElement = 0,
                           std::uint32_t descriptorCount = 1);

        WriteDescriptorSet(VkDescriptorSet dstSet,
                           vul::DescriptorType descriptorType,
                           const DescriptorBufferInfo *pBufferInfo,
                           std::uint32_t dstBinding,
                           std::uint32_t dstArrayElement = 0,
                           std::uint32_t descriptorCount = 1);


        WriteDescriptorSet(VkDescriptorSet dstSet,
                           vul::DescriptorType descriptorType,
                           const VkBufferView *pTexelBufferView,
                           std::uint32_t dstBinding,
                           std::uint32_t dstArrayElement = 0,
                           std::uint32_t descriptorCount = 1);

        WriteDescriptorSet(VkDescriptorSet dstSet,
                           vul::DescriptorType descriptorType,
                           const void *pNext,
                           std::uint32_t dstBinding,
                           std::uint32_t dstArrayElement = 0,
                           std::uint32_t descriptorCount = 1);


        WriteDescriptorSet(const WriteDescriptorSet &rhs) noexcept;

        WriteDescriptorSet(WriteDescriptorSet
                           &&rhs) noexcept;

        WriteDescriptorSet &operator=(const WriteDescriptorSet &rhs) noexcept;

        ~WriteDescriptorSet();

        operator VkWriteDescriptorSet() const noexcept;

        [[nodiscard]]
        VkWriteDescriptorSet &get() noexcept;

        [[nodiscard]]
        const VkWriteDescriptorSet &get() const noexcept;

    private:
    };

    //to describe a descriptor write:

    //binding
    //number of array elements.
    //so[0] binding, (0,1)

    struct DescriptorTypeCount {
        vul::DescriptorType type;
        std::uint32_t count;
    };

    class WriteDescriptorSetBuilder {
    public:
        WriteDescriptorSetBuilder();

        WriteDescriptorSetBuilder(
                VkDescriptorSet descriptorSet,
                std::vector<DescriptorTypeCount> m_descriptorTypeCounts);

        class Binding {
        public:
            class ArrayElement {
            public:
                ArrayElement &operator=(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos);

                ArrayElement &operator=(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos);

                ArrayElement &operator=(vul::TempArrayProxy<VkBufferView> bufferViews);

                ArrayElement &operator=(vul::TempArrayProxy<const Buffer *> buffers);

                ArrayElement &operator=(vul::TempArrayProxy<const ImageView *> imageViews);

                ArrayElement &operator=(vul::TempArrayProxy<const Sampler *> samplers);

                ArrayElement &set(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos);

                ArrayElement &set(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos);

                ArrayElement &set(vul::TempArrayProxy<VkBufferView> bufferViews);

                ArrayElement &set(vul::TempArrayProxy<const Buffer *> buffers, VkDeviceSize offset = 0,
                                  VkDeviceSize range = VK_WHOLE_SIZE);

                ArrayElement &set(vul::TempArrayProxy<const ImageView *> imageViews, const vul::Sampler &sampler,
                                  vul::ImageLayout imageLayout);

                ArrayElement &set(vul::TempArrayProxy<const ImageView *> imageViews, const vul::Sampler &sampler);

                ArrayElement &set(vul::TempArrayProxy<const ImageView *> imageViews, vul::ImageLayout imageLayout);

                ArrayElement &set(vul::TempArrayProxy<const ImageView *> imageViews);

                ArrayElement &set(vul::TempArrayProxy<const Sampler  *> samplers);

            private:
                friend class Binding;

                ArrayElement(WriteDescriptorSetBuilder *parent, std::uint32_t bindingIndex, std::uint32_t arrayIndex);

                WriteDescriptorSetBuilder *m_parent;
                std::uint32_t m_bindingIndex;
                std::uint32_t m_arrayIndex;
            };

            Binding &operator=(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos);

            Binding &operator=(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos);

            Binding &operator=(vul::TempArrayProxy<VkBufferView> bufferViews);

            Binding &operator=(vul::TempArrayProxy<const Buffer *> buffers);

            Binding &operator=(vul::TempArrayProxy<const ImageView *> imageViews);

            Binding &operator=(vul::TempArrayProxy<const Sampler *> samplers);

            Binding &set(vul::TempArrayProxy<DescriptorBufferInfo> descriptorInfos);

            Binding &set(vul::TempArrayProxy<DescriptorImageInfo> descriptorInfos);

            Binding &set(vul::TempArrayProxy<VkBufferView> bufferViews);

            Binding &set(vul::TempArrayProxy<const Buffer *> buffers, VkDeviceSize offset = 0,
                         VkDeviceSize range = VK_WHOLE_SIZE);

            Binding &set(vul::TempArrayProxy<const ImageView *> imageViews, const vul::Sampler &sampler,
                         vul::ImageLayout imageLayout);

            Binding &set(vul::TempArrayProxy<const ImageView *> imageViews, const vul::Sampler &sampler);

            Binding &set(vul::TempArrayProxy<const ImageView *> imageViews, vul::ImageLayout imageLayout);

            Binding &set(vul::TempArrayProxy<const ImageView *> imageViews);

            Binding &set(vul::TempArrayProxy<const Sampler  *> samplers);

            [[nodiscard]]
            ArrayElement operator[](std::uint32_t arrayIndex);

        private:
            friend class WriteDescriptorSetBuilder;

            Binding(WriteDescriptorSetBuilder *parent, std::uint32_t m_bindingIndex);

            WriteDescriptorSetBuilder *m_parent;
            std::uint32_t m_bindingIndex;
        };

        [[nodiscard]]
        Binding operator[](std::uint32_t bindingIndex);

        void clear();

        [[nodiscard]]
        std::uint32_t size() const;

        [[nodiscard]]
        bool empty() const;
        [[nodiscard]]
        std::span<const WriteDescriptorSet> getWrites() const;

        WriteDescriptorSetBuilder &append(WriteDescriptorSet writeDescriptorSet);

        [[nodiscard]]
        DescriptorTypeCount getDescriptorTypeCountAt(std::uint32_t bindingIndex) const;

    private:
        //shouldn't care about adding tonnes of updates, as long as the updates aren't invalid? Basically update validator?

        VkDescriptorSet m_descriptorSet;
        std::vector<DescriptorBufferInfo> m_bufferInfos;
        std::vector<DescriptorImageInfo> m_imageInfos;
        std::vector<VkBufferView> m_bufferViews;
        std::vector<DescriptorTypeCount> m_descriptorTypeCounts; //checks if types are valid and if counts added are valid.
        std::vector<WriteDescriptorSet> m_writeDescriptorSets; // adds as much as needed.
        //TODO is keeping m_descriptoTypeCounts even necessary? Maybe just take it from the descriptorSetLayoutBuilder?
        //Then basically just a set of writes?
    };


}
#endif //VULKANWORKSPACE_WRITEDESCRIPTORSET_H
