//
// Created by Shae Bolt on 8/28/2021.
//

#ifndef VULKANWORKSPACE_SAMPLER_H
#define VULKANWORKSPACE_SAMPLER_H
#include <vulkan/vulkan.h>
#include <vul/enumsfwd.h>
#include <vul/bitmasksfwd.h>
#include <cstdint>
#include <string>
namespace vul{

    template<typename T>
    class ExpectedResult;
    class Device;
    class Sampler{
    public:
        Sampler() = default;

        Sampler(const Device &device,
                VkSampler handle,
                const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkSampler get() const;

        ~Sampler();

        Sampler(Sampler &&rhs) noexcept;

//        was noexcept?
        Sampler &operator=(Sampler &&rhs) noexcept;

        Sampler(const Sampler &rhs) = delete;

        Sampler &operator=(Sampler &rhs) = delete;

        Result setObjectName(const std::string &string);
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkSampler m_handle = VK_NULL_HANDLE;
    };
    class SamplerBuilder{
    public:
        explicit SamplerBuilder(const Device &device,
                       const VkAllocationCallbacks *pAllocator = nullptr);
        void setCreateFlags(vul::SamplerCreateBitMask flags);
        void setFilter(vul::Filter filter);
        void setFilter(vul::Filter minFilter, vul::Filter magFilter);
        void setMipmapMode(vul::SamplerMipmapMode mipmapMode);
        void setAddressMode(vul::SamplerAddressMode addressMode);
        void setAddressMode(vul::SamplerAddressMode addressModeU,vul::SamplerAddressMode addressModeV,vul::SamplerAddressMode addressModeW);
        void setMipLodBias(float bias);
        void setLodRange(float min, float max);
        void setBorderColor(vul::BorderColor borderColor);
        void enableUnnormalizedCoordinates();
        void setCompareOp(vul::CompareOp compareOp);
        void enableAnisotropy();
        void enableAnisotropy(float maxAnisotropy);

        [[nodiscard]]
        ExpectedResult<Sampler> create();
    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkSamplerCreateInfo m_createInfo = {};
    };

}
#endif //VULKANWORKSPACE_SAMPLER_H
