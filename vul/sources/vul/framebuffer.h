//
// Created by Shae Bolt on 8/31/2021.
//

#ifndef VULKANWORKSPACE_FRAMEBUFFER_H
#define VULKANWORKSPACE_FRAMEBUFFER_H
#include <vulkan/vulkan.h>
#include <vul/enumsfwd.h>
#include <vul/bitmasksfwd.h>
#include <cstdint>
#include <string>
#include <vector>
#include "commandbuffer.h"

namespace vul {
    template<typename T>
    class ExpectedResult;

    class Device;

    class Framebuffer {
    public:
        Framebuffer() = default;

        Framebuffer(const Device &device,
                    VkFramebuffer handle,
                    const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        VkFramebuffer get() const;

        ~Framebuffer();

        Framebuffer(Framebuffer &&rhs) noexcept;

//        was noexcept?
        Framebuffer &operator=(Framebuffer &&rhs) noexcept;

        Framebuffer(const Framebuffer &rhs) = delete;

        Framebuffer &operator=(Framebuffer &rhs) = delete;

        Result setObjectName(const std::string &string);

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkFramebuffer m_handle = VK_NULL_HANDLE;
    };

    class RenderPass;

    class ImageView;

    template<typename T>
    class TempArrayProxy;

    class FramebufferBuilder {
    public:
        explicit FramebufferBuilder(const Device &device,
                                    const VkAllocationCallbacks *pAllocator = nullptr);

        void setCreateFlags(vul::SamplerCreateBitMask flags);

        void setRenderPass(const RenderPass &renderPass);

        void setAttachments(const TempArrayProxy<const ImageView> &attachments);

        void setAttachments(
                const TempArrayProxy<std::reference_wrapper<const ImageView>> &attachments);

        void
        setAttachments(const TempArrayProxy<const ImageView *> &attachments);

        void setDimensions(std::uint32_t width, std::uint32_t height,
                           std::uint32_t layers = 1);
        void setDimensions(VkExtent2D extent, std::uint32_t layers = 1);

        void setNext(const void* pNext);

        [[nodiscard]]
        ExpectedResult<Framebuffer> create() const;

    private:
        const Device *m_pDevice = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        VkFramebufferCreateInfo m_createInfo = {};
        std::vector<VkImageView> m_attachments;
    };
}

#endif //VULKANWORKSPACE_FRAMEBUFFER_H
