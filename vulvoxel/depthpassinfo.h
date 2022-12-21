//
// Created by Bolt on 12/17/2022.
//

#ifndef VULKANWORKSPACE_DEPTHPASSINFO_H
#define VULKANWORKSPACE_DEPTHPASSINFO_H
#include <vul/renderpass.h>
#include <vul/image.h>
#include <vul/imageview.h>
#include <vul/sampler.h>
#include <vul/framebuffer.h>
#include <string_view>
namespace vul{
    class Device;
    class VmaAllocator;
    struct Extent2D;
    class DepthPassSystem{
    public:
        DepthPassSystem(Device& device, VmaAllocator& allocator, const Extent2D& extent2D, std::string_view name);
    private:
        vul::SubpassGraph m_subpass_graph;
        vul::RenderPass m_render_pass;
        vul::Image m_shadow_map_image;
        vul::ImageView m_shadow_map_image_view;
        vul::Sampler m_shadow_map_sampler;
        vul::Framebuffer m_shadow_map_framebuffer;
    };
}
#endif //VULKANWORKSPACE_DEPTHPASSINFO_H
