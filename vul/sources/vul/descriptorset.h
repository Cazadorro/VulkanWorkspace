//
// Created by Shae Bolt on 8/29/2021.
//

#ifndef VULKANWORKSPACE_DESCRIPTORSET_H
#define VULKANWORKSPACE_DESCRIPTORSET_H
#include "vul/enums.h"
#include <vulkan/vulkan.h>
#include <gsl/span>
#include <vector>
#include <unordered_map>
#include <variant>
namespace vul{

    class DescriptorSetUpdateBuilder;
    class DescriptorSetLayoutBinding;
    class WriteDescriptorSetInfo{
    public:
        WriteDescriptorSetInfo() = default;

        [[nodiscard]]
        static WriteDescriptorSetInfo Sampler(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo CombinedImageSampler(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo SampledImage(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo StorageImage(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo UniformTexelBuffer(const std::vector<VkBufferView>& texelBufferViews, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo StorageTexelBuffer(const std::vector<VkBufferView>& texelBufferViews, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo UniformBuffer(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo StorageBuffer(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo UniformBufferDynamic(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo StorageBufferDynamic(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);
        [[nodiscard]]
        static WriteDescriptorSetInfo InputAttachment(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);

        [[nodiscard]]
        VkWriteDescriptorSet createWriteDescriptorSet(VkDescriptorSet dstSet, std::uint32_t dstBinding, std::uint32_t dstArrayElement) const;
        [[nodiscard]]
        WriteDescriptorSetInfo &operator=(WriteDescriptorSetInfo &&rhs) noexcept;
        [[nodiscard]]
        WriteDescriptorSetInfo &operator=(const WriteDescriptorSetInfo &rhs);


        void setSampler(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);

        void setCombinedImageSampler(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);

        void setSampledImage(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);

        void setStorageImage(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);

        void setUniformTexelBuffer(const std::vector<VkBufferView>& texelBufferViews, const void *pNext = nullptr);

        void setStorageTexelBuffer(const std::vector<VkBufferView>& texelBufferViews, const void *pNext = nullptr);

        void setUniformBuffer(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);

        void setStorageBuffer(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);

        void setUniformBufferDynamic(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);

        void setStorageBufferDynamic(const std::vector<VkDescriptorBufferInfo>& bufferInfos, const void *pNext = nullptr);

        void setInputAttachment(const std::vector<VkDescriptorImageInfo>& imageInfos, const void *pNext = nullptr);

        [[nodiscard]]
        std::size_t infoCount() const;
        [[nodiscard]]
        bool empty() const;
    private:
        friend class DescriptorSetUpdateBuilder;
        WriteDescriptorSetInfo(WriteDescriptorSetInfo &&rhs) noexcept = default;
        WriteDescriptorSetInfo(const WriteDescriptorSetInfo &rhs) = default;
        vul::DescriptorType m_descriptorType;
        //TODO this could be done faster with custom variant on m_descriptorType
        std::variant<std::vector<VkDescriptorImageInfo>, std::vector<VkDescriptorBufferInfo>, std::vector<VkBufferView>> m_infoVariant;
        const void* m_pNext;
    };



    class DescriptorSetUpdateBuilder{
    public:
        explicit DescriptorSetUpdateBuilder(const gsl::span<const DescriptorSetLayoutBinding>& bindings, const std::unordered_map<std::string, std::uint32_t>& nameBindingMap = {});

        [[nodiscard]]
        std::vector<WriteDescriptorSetInfo>& getDescriptorArrayAt(std::uint32_t bindingIndex);
        [[nodiscard]]
        const std::vector<WriteDescriptorSetInfo>& getDescriptorArrayAt(std::uint32_t bindingIndex) const;

        [[nodiscard]]
        std::vector<WriteDescriptorSetInfo>& getDescriptorArrayAt(const std::string_view& bindingName);
        [[nodiscard]]
        const std::vector<WriteDescriptorSetInfo>& getDescriptorArrayAt(const std::string_view& bindingName) const;

        [[nodiscard]]
        WriteDescriptorSetInfo& getDescriptorElementAt(std::uint32_t bindingIndex, std::uint32_t elementIndex = 0);
        [[nodiscard]]
        const WriteDescriptorSetInfo& getDescriptorElementAt(std::uint32_t bindingIndex, std::uint32_t elementIndex = 0) const;

        [[nodiscard]]
        WriteDescriptorSetInfo& getDescriptorElementAt(const std::string_view& bindingName, std::uint32_t elementIndex = 0);
        [[nodiscard]]
        const WriteDescriptorSetInfo& getDescriptorElementAt(const std::string_view& bindingName, std::uint32_t elementIndex = 0) const;

        [[nodiscard]]
        std::vector<VkWriteDescriptorSet> create(const VkDescriptorSet& dstSet) const;
    private:
        std::vector<vul::DescriptorType> m_descriptorTypes;
        std::unordered_map<std::string, std::uint32_t> m_nameBindingMap;
        std::vector<std::vector<WriteDescriptorSetInfo>> m_writeDescriptorInfos;
    };

}
#endif //VULKANWORKSPACE_DESCRIPTORSET_H
