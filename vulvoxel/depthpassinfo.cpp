//
// Created by Bolt on 12/17/2022.
//

#include "depthpassinfo.h"
#include <vul/device.h>
#include <vul/vmaallocator.h>
#include <vul/vkstructutils.h>
#include <vul/temparrayproxy.h>
#include <string>

vul::DepthPassSystem::DepthPassSystem(Device& device, VmaAllocator& allocator,const Extent2D& extent2D, std::string_view name): m_subpass_graph(vul::SubpassGraph({vul::AttachmentDescription::DepthReadOnly(vul::Format::D24UnormS8Uint)}, 1)){

    m_subpass_graph.subpassAt(0).setWrites({0})
            .setPreDependExternal(
                    vul::PipelineStageFlagBits::FragmentShaderBit,
                    vul::PipelineStageFlagBits::EarlyFragmentTestsBit,
                    vul::AccessFlagBits::ShaderReadBit,
                    vul::AccessFlagBits::DepthStencilAttachmentWriteBit)
            .setPostExternalDepend(vul::PipelineStageFlagBits::LateFragmentTestsBit,
                                   vul::PipelineStageFlagBits::FragmentShaderBit,
                                   vul::AccessFlagBits::DepthStencilAttachmentWriteBit,
                                   vul::AccessFlagBits::ShaderReadBit);
    m_render_pass = m_subpass_graph.createRenderPass(device).assertValue();
    if(!name.empty()){
        m_render_pass.setObjectName(std::string(name));
    }
    m_shadow_map_image = allocator.createDeviceImage(
            vul::createSimple2DImageInfo(
                    vul::Format::D24UnormS8Uint,
                    extent2D,
                    vul::ImageUsageFlagBits::DepthStencilAttachmentBit | vul::ImageUsageFlagBits::SampledBit)
    ).assertValue();
    m_shadow_map_image_view =  m_shadow_map_image.createImageView(vul::ImageSubresourceRange(
            vul::ImageAspectFlagBits::DepthBit)).assertValue();
    //TODO put device in "create" instead of here to allow usage between devices easier?
    vul::SamplerBuilder sampler_builder(device);
    sampler_builder.setFilter(vul::Filter::Nearest);
    sampler_builder.setAddressMode(vul::SamplerAddressMode::ClampToEdge);
    sampler_builder.enableAnisotropy();
    sampler_builder.setMipmapMode(vul::SamplerMipmapMode::Linear);
    sampler_builder.setBorderColor(vul::BorderColor::FloatOpaqueWhite);

    m_shadow_map_sampler = sampler_builder.create().assertValue();

    vul::FramebufferBuilder framebufferBuilder(device);
    std::array<const vul::ImageView *, 1> image_views = {&m_shadow_map_image_view};
    framebufferBuilder.setAttachments(image_views);
    framebufferBuilder.setDimensions(extent2D);
    framebufferBuilder.setRenderPass(m_render_pass);
    m_shadow_map_framebuffer = framebufferBuilder.create().assertValue();


}
