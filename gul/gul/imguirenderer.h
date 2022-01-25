//
// Created by Shae Bolt on 1/24/2022.
//

#ifndef VULKANWORKSPACE_IMGUIRENDERER_H
#define VULKANWORKSPACE_IMGUIRENDERER_H

#include <vul/renderpass.h>
#include <vul/framebuffer.h>
#include <vul/enumsfwd.h>

#include <vulkan/vulkan.h>
#include <functional>
#include <vector>
#include <cstdint>

namespace vul {
    class Instance;

    class Device;

    class Queue;

    class PrimaryCommandBuffer;

    class Surface;
}
namespace gul {
    class GlfwWindow;
    class ImguiRenderer {
    public:
        ImguiRenderer(
                gul::GlfwWindow &window,
                const vul::Instance &instance,
                const vul::Device &device,
                const vul::Surface& surface,
                std::uint32_t queueFamilyIndex,
                const vul::Queue &queue,
                vul::Format format);

        ~ImguiRenderer();

        void newFrame() const;
        void postSubmit() const;

        void recordCommands(vul::PrimaryCommandBuffer& commandBuffer, std::uint32_t swapchainImageIndex) const;


        [[nodiscard]]
        std::function<void()> createResizeCallback();

    private:
        const vul::Device *m_pDevice = nullptr;
        const vul::Surface *m_pSurface = nullptr;
        vul::RenderPass m_renderPass;
        std::vector<vul::Framebuffer> m_framebuffers;
        std::vector<VkDescriptorPoolSize> m_pool_sizes;
        [[nodiscard]]
        static vul::ExpectedResult<vul::RenderPass>
        createImGuiRenderPass(const vul::Device &device, vul::Format surfaceFormat);
    };
}


#endif //VULKANWORKSPACE_IMGUIRENDERER_H
