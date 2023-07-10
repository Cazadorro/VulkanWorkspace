//
// Created by Shae Bolt on 8/28/2021.
//

#ifndef VULKANWORKSPACE_DESCRIPTORSETLAYOUT_H
#define VULKANWORKSPACE_DESCRIPTORSETLAYOUT_H

#include "vul/enumsfwd.h"
#include "vul/temparrayproxyfwd.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <gsl/span>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vul {
    struct DescriptorTypeCount;

    class DescriptorSetLayoutBinding {
    public:
        std::uint32_t binding;
        vul::DescriptorType descriptorType;
        std::uint32_t descriptorCount;
        uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags;
        const VkSampler *pImmutableSamplers;


        DescriptorSetLayoutBinding() = default;

        explicit DescriptorSetLayoutBinding(std::uint32_t binding,
                                            vul::DescriptorType descriptorType,
                                            std::uint32_t descriptorCount,
                                            uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                            const VkSampler *pImmutableSamplers);

        explicit DescriptorSetLayoutBinding(VkDescriptorSetLayoutBinding descriptorSetLayoutBinding);

        [[nodiscard]]
        VkDescriptorSetLayoutBinding & get();

        [[nodiscard]]
        const VkDescriptorSetLayoutBinding & get() const;

        operator VkDescriptorSetLayoutBinding() const;
    protected:
    };

    class SamplerBinding : public DescriptorSetLayoutBinding {
    public:
        SamplerBinding(std::uint32_t binding,
                       uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                       std::uint32_t descriptorCount = 1);

    private:

    };

    static_assert(sizeof(SamplerBinding) == sizeof(VkDescriptorSetLayoutBinding));

    //TODO replace VkSampler with Sampler?
    class ImmutableSamplerBinding : public DescriptorSetLayoutBinding {
    public:
        ImmutableSamplerBinding(std::uint32_t binding,
                                uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                const VkSampler *pImmutableSamplers,
                                std::uint32_t descriptorCount = 1);

    private:

    };

    class CombinedSamplerBinding : public DescriptorSetLayoutBinding {
    public:
        CombinedSamplerBinding(std::uint32_t binding,
                               uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                               std::uint32_t descriptorCount = 1);

    private:

    };

    class SampledImageBinding : public DescriptorSetLayoutBinding {
    public:
        SampledImageBinding(std::uint32_t binding,
                            uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                            std::uint32_t descriptorCount = 1);

    private:

    };

    class StorageImageBinding : public DescriptorSetLayoutBinding {
    public:
        StorageImageBinding(std::uint32_t binding,
                            uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                            std::uint32_t descriptorCount = 1);

    private:

    };

    class UniformTexelBufferBinding : public DescriptorSetLayoutBinding {
    public:
        UniformTexelBufferBinding(std::uint32_t binding,
                                  uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                  std::uint32_t descriptorCount = 1);

    private:

    };

    class StorageTexelBufferBinding : public DescriptorSetLayoutBinding {
    public:
        StorageTexelBufferBinding(std::uint32_t binding,
                                  uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                  std::uint32_t descriptorCount = 1);

    private:

    };

    class UniformBufferBinding : public DescriptorSetLayoutBinding {
    public:
        UniformBufferBinding(std::uint32_t binding,
                             uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                             std::uint32_t descriptorCount = 1);

    private:

    };

    class StorageBufferBinding : public DescriptorSetLayoutBinding {
    public:
        StorageBufferBinding(std::uint32_t binding,
                             uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                             std::uint32_t descriptorCount = 1);

    private:

    };

    class DynamicUniformBufferBinding : public DescriptorSetLayoutBinding {
    public:
        DynamicUniformBufferBinding(std::uint32_t binding,
                                    uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                    std::uint32_t descriptorCount = 1);

    private:

    };

    class DynamicStorageBufferBinding : public DescriptorSetLayoutBinding {
    public:
        DynamicStorageBufferBinding(std::uint32_t binding,
                                    uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                    std::uint32_t descriptorCount = 1);

    private:

    };

    class InputAttachmentBinding : public DescriptorSetLayoutBinding {
    public:
        InputAttachmentBinding(std::uint32_t binding,
                               uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                               std::uint32_t descriptorCount = 1);

    private:

    };

    class InlineUniformBlockBinding : public DescriptorSetLayoutBinding {
    public:
        InlineUniformBlockBinding(std::uint32_t binding,
                                  uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                  std::uint32_t descriptorCount = 1);

    private:

    };

    class AccelerationStructureKHRBinding : public DescriptorSetLayoutBinding {
    public:
        AccelerationStructureKHRBinding(std::uint32_t binding,
                                        uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags,
                                        std::uint32_t descriptorCount = 1);

    private:

    };

    template<typename T>
    class ExpectedResult;

    class Device;

    class DescriptorSetLayout {
    public:
        DescriptorSetLayout() = default;

        DescriptorSetLayout(const Device &device,
                            VkDescriptorSetLayout handle,
                            const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        const VkDescriptorSetLayout &get() const;

        ~DescriptorSetLayout();

        DescriptorSetLayout(DescriptorSetLayout &&rhs) noexcept;

//        was noexcept?
        DescriptorSetLayout &operator=(DescriptorSetLayout &&rhs) noexcept;

        DescriptorSetLayout(const DescriptorSetLayout &rhs) = delete;

        DescriptorSetLayout &operator=(const DescriptorSetLayout &rhs) = delete;

        Result setObjectName(const std::string &string);

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDescriptorSetLayout m_handle = VK_NULL_HANDLE;
    };

    class DescriptorSetUpdateBuilder;

    class DescriptorSetLayoutBuilder {
    public:
        DescriptorSetLayoutBuilder();
        explicit DescriptorSetLayoutBuilder(const Device &device,
                                            const VkAllocationCallbacks *pAllocator = nullptr);

        DescriptorSetLayoutBuilder& setFlags(uul::EnumFlags<vul::DescriptorSetLayoutCreateFlagBits> flags);

        DescriptorSetLayoutBuilder& setStageFlags(uul::EnumFlags<vul::ShaderStageFlagBits>  stageFlags);


        DescriptorSetLayoutBuilder& setBindings(const TempArrayProxy<DescriptorSetLayoutBinding const> &descriptorSetBindings,
                         const TempArrayProxy<std::string const> &bindingName);

        DescriptorSetLayoutBuilder& setBindings(const TempArrayProxy<DescriptorSetLayoutBinding const> &descriptorSetBindings,
                         const TempArrayProxy<std::string const> &bindingName,
                         const TempArrayProxy<uul::EnumFlags<vul::DescriptorBindingFlagBits>  const> &bindingFlags);

        DescriptorSetLayoutBuilder& setBindings(const TempArrayProxy<DescriptorSetLayoutBinding const> &descriptorSetBindings);

        DescriptorSetLayoutBuilder& setBindings(const TempArrayProxy<DescriptorSetLayoutBinding const> &descriptorSetBindings,
                         const TempArrayProxy<uul::EnumFlags<vul::DescriptorBindingFlagBits>  const> &bindingFlags);

        [[nodiscard]]
        DescriptorSetLayoutBinding &getBinding(std::uint32_t i);

        [[nodiscard]]
        DescriptorSetLayoutBinding &getBinding(const std::string_view &name);

        [[nodiscard]]
        const DescriptorSetLayoutBinding &getBinding(std::uint32_t i) const;

        [[nodiscard]]
        const DescriptorSetLayoutBinding &getBinding(const std::string_view &name) const;


        [[nodiscard]]
        ExpectedResult<DescriptorSetLayout> create() const;

        [[nodiscard]]
        DescriptorSetUpdateBuilder createUpdateBuilder() const;

        [[nodiscard]]
        std::vector<DescriptorTypeCount> createDescriptorTypeCounts() const;

        [[nodiscard]]
        std::vector<VkDescriptorPoolSize> calcPoolSizes(std::uint32_t multiplier = 0) const;

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDescriptorSetLayoutCreateFlags m_flags = {};
        std::vector<DescriptorSetLayoutBinding> m_bindings;
        std::unordered_map<std::string, std::uint32_t> m_nameBindingMap;
        std::vector<uul::EnumFlags<vul::DescriptorBindingFlagBits> > m_bindingFlags;
    };

    struct LayoutBuilderCount {
        const DescriptorSetLayoutBuilder &builder;
        std::uint32_t count;
    };
}
#endif //VULKANWORKSPACE_DESCRIPTORSETLAYOUT_H
