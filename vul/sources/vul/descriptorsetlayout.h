//
// Created by Shae Bolt on 8/28/2021.
//

#ifndef VULKANWORKSPACE_DESCRIPTORSETLAYOUT_H
#define VULKANWORKSPACE_DESCRIPTORSETLAYOUT_H

#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include <vulkan/vulkan.h>
#include <gsl/span>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace vul {


    class DescriptorSetLayoutBinding {
    public:
        [[nodiscard]]
        const VkDescriptorSetLayoutBinding &get() const;

        [[nodiscard]]
        vul::ShaderStageBitMask& getStageFlags();
        [[nodiscard]]
        const vul::ShaderStageBitMask& getStageFlags() const;
        [[nodiscard]]
        std::uint32_t& getDescriptorCount();
        [[nodiscard]]
        const std::uint32_t& getDescriptorCount() const;
        void setImmutableSamplers(const VkSampler *pImmutableSamplers);
    protected:
        VkDescriptorSetLayoutBinding m_layoutBinding = {};
    };

    class SamplerBinding : public DescriptorSetLayoutBinding {
    public:
        SamplerBinding(std::uint32_t binding,
                       vul::ShaderStageBitMask stageFlags,
                       std::uint32_t descriptorCount = 1);

    private:

    };
    static_assert(sizeof(SamplerBinding) == sizeof(VkDescriptorSetLayoutBinding));

    //TODO replace VkSampler with Sampler?
    class ImmutableSamplerBinding : public DescriptorSetLayoutBinding {
    public:
        ImmutableSamplerBinding(std::uint32_t binding,
                                vul::ShaderStageBitMask stageFlags,
                                const VkSampler *pImmutableSamplers,
                                std::uint32_t descriptorCount = 1);

    private:

    };

    class CombinedSamplerBinding : public DescriptorSetLayoutBinding {
    public:
        CombinedSamplerBinding(std::uint32_t binding,
                               vul::ShaderStageBitMask stageFlags,
                               std::uint32_t descriptorCount = 1);

    private:

    };

    class SampledImageBinding : public DescriptorSetLayoutBinding {
    public:
        SampledImageBinding(std::uint32_t binding,
                            vul::ShaderStageBitMask stageFlags,
                            std::uint32_t descriptorCount = 1);

    private:

    };

    class StorageImageBinding : public DescriptorSetLayoutBinding {
    public:
        StorageImageBinding(std::uint32_t binding,
                            vul::ShaderStageBitMask stageFlags,
                            std::uint32_t descriptorCount = 1);

    private:

    };

    class UniformTexelBufferBinding : public DescriptorSetLayoutBinding {
    public:
        UniformTexelBufferBinding(std::uint32_t binding,
                                  vul::ShaderStageBitMask stageFlags,
                                  std::uint32_t descriptorCount = 1);

    private:

    };

    class StorageTexelBufferBinding : public DescriptorSetLayoutBinding {
    public:
        StorageTexelBufferBinding(std::uint32_t binding,
                                  vul::ShaderStageBitMask stageFlags,
                                  std::uint32_t descriptorCount = 1);

    private:

    };

    class UniformBufferBinding : public DescriptorSetLayoutBinding {
    public:
        UniformBufferBinding(std::uint32_t binding,
                             vul::ShaderStageBitMask stageFlags,
                             std::uint32_t descriptorCount = 1);

    private:

    };

    class StorageBufferBinding : public DescriptorSetLayoutBinding {
    public:
        StorageBufferBinding(std::uint32_t binding,
                             vul::ShaderStageBitMask stageFlags,
                             std::uint32_t descriptorCount = 1);

    private:

    };

    class DynamicUniformBufferBinding : public DescriptorSetLayoutBinding {
    public:
        DynamicUniformBufferBinding(std::uint32_t binding,
                                    vul::ShaderStageBitMask stageFlags,
                                    std::uint32_t descriptorCount = 1);

    private:

    };

    class DynamicStorageBufferBinding : public DescriptorSetLayoutBinding {
    public:
        DynamicStorageBufferBinding(std::uint32_t binding,
                                    vul::ShaderStageBitMask stageFlags,
                                    std::uint32_t descriptorCount = 1);

    private:

    };

    class InputAttachmentBinding : public DescriptorSetLayoutBinding {
    public:
        InputAttachmentBinding(std::uint32_t binding,
                               vul::ShaderStageBitMask stageFlags,
                               std::uint32_t descriptorCount = 1);

    private:

    };

    class InlineUniformBlockEXT : public DescriptorSetLayoutBinding {
    public:
        InlineUniformBlockEXT(std::uint32_t binding,
                              vul::ShaderStageBitMask stageFlags,
                              std::uint32_t descriptorCount = 1);

    private:

    };

    class AccelerationStructureKHR : public DescriptorSetLayoutBinding {
    public:
        AccelerationStructureKHR(std::uint32_t binding,
                                 vul::ShaderStageBitMask stageFlags,
                                 std::uint32_t descriptorCount = 1);

    private:

    };

    template<typename T>
    class ExpectedResult;
    class Device;

    class DescriptorSetLayout{
    public:
        DescriptorSetLayout() = default;

        DescriptorSetLayout(const Device &device,
                            VkDescriptorSetLayout handle,
                const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkDescriptorSetLayout get() const;

        ~DescriptorSetLayout();

        DescriptorSetLayout(DescriptorSetLayout &&rhs) noexcept;

//        was noexcept?
        DescriptorSetLayout &operator=(DescriptorSetLayout &&rhs) noexcept;

        DescriptorSetLayout(const DescriptorSetLayout &rhs) = delete;

        DescriptorSetLayout &operator=(DescriptorSetLayout &rhs) = delete;

        Result setObjectName(const std::string &string);
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDescriptorSetLayout m_handle = VK_NULL_HANDLE;
    };

    class DescriptorSetUpdateBuilder;
    class DescriptorSetLayoutBuilder {
    public:
        explicit DescriptorSetLayoutBuilder(const Device &device,
        const VkAllocationCallbacks *pAllocator = nullptr);
        void setFlags(vul::DescriptorSetLayoutCreateBitMask flags);
        void setStageFlags(vul::ShaderStageBitMask stageFlags);
        void setBindings(const gsl::span<VkDescriptorSetLayoutBinding const>& descriptorSetBindings);
        void setBindings(const gsl::span<VkDescriptorSetLayoutBinding const>& descriptorSetBindings,
                         const gsl::span<std::string const>& bindingName);


        [[nodiscard]]
        DescriptorSetLayoutBinding& getBinding(std::uint32_t i);
        [[nodiscard]]
        DescriptorSetLayoutBinding& getBinding(const std::string_view& name);
        [[nodiscard]]
        const DescriptorSetLayoutBinding& getBinding(std::uint32_t i) const;
        [[nodiscard]]
        const DescriptorSetLayoutBinding& getBinding(const std::string_view& name) const;


        [[nodiscard]]
        ExpectedResult<DescriptorSetLayout> create();

        [[nodiscard]]
        DescriptorSetUpdateBuilder createUpdateBuilder();
        [[nodiscard]]
        std::vector<VkDescriptorPoolSize> calcPoolSizes(std::uint32_t multiplier = 0) const;
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkDescriptorSetLayoutCreateFlags m_flags = {};
        std::vector<VkDescriptorSetLayoutBinding> m_bindings;
        std::unordered_map<std::string, std::uint32_t> m_nameBindingMap;
    };

    struct LayoutBuilderCount {
        const DescriptorSetLayoutBuilder &builder;
        std::uint32_t count;
    };
}
#endif //VULKANWORKSPACE_DESCRIPTORSETLAYOUT_H
