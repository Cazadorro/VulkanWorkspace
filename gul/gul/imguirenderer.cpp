//
// Created by Shae Bolt on 1/24/2022.
//

#include "imguirenderer.h"
#include "glfwwindow.h"
#include <vul/commandbuffer.h>
#include <vul/imageview.h>
#include <vul/swapchain.h>
#include <vul/queue.h>
#include <vul/descriptorpool.h>
#include <vul/commandpool.h>
#include <vul/device.h>
#include <vul/instance.h>
#include <vul/expectedresult.h>
#include <vul/enums.h>
#include <vul/temparrayproxy.h>
#include <uul/assert.h>
#include <vul/containerutils.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <fmt/core.h>


gul::ImguiRenderer::ImguiRenderer(gul::GlfwWindow &window,
                                  const vul::Instance &instance,
                                  const vul::Device &device,
                                  const vul::Swapchain &swapchain,
                                  const vul::Queue &queue,
                                  vul::Format format) : m_pDevice(&device),
                                                        m_pSwapchain(&swapchain) {

    //from imgui_impl_vulkan.cpp, line 1567.
//    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
//    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    std::array exceptedSurfaceFormats = {
            static_cast<vul::Format>(VK_FORMAT_B8G8R8A8_UNORM),
            static_cast<vul::Format>(VK_FORMAT_R8G8B8A8_UNORM),
            static_cast<vul::Format>(VK_FORMAT_B8G8R8_UNORM),
            static_cast<vul::Format>(VK_FORMAT_R8G8B8_UNORM)};
    UUL_ASSERT(vul::contains(exceptedSurfaceFormats, format),
               fmt::format(
                       "Format found {} doesn't match valid imgui image formats {}, {}, {} and {}",
                       vul::to_string(format),
                       vul::to_string((exceptedSurfaceFormats[0])),
                       vul::to_string((exceptedSurfaceFormats[1])),
                       vul::to_string((exceptedSurfaceFormats[2])),
                       vul::to_string((exceptedSurfaceFormats[3]))
               ).c_str());
    auto commandPool = device.createCommandPool(
            queue.getQueueFamilyIndex(),
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();
    commandPool.setObjectName("TemporaryImguiRendererCommandBuffer");
    m_pool_sizes =
            {
                    {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
            };

    m_descriptorPool = device.createDescriptorPool(
            m_pool_sizes, 1000,
            vul::DescriptorPoolCreateFlagBits::FreeDescriptorSetBit).assertValue();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.getWindowPtr(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance.get();
    init_info.PhysicalDevice = device.getPhysicalDevice().get();
    init_info.Device = device.get();
    init_info.QueueFamily = queue.getQueueFamilyIndex();
    init_info.Queue = queue.get();
//    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = m_descriptorPool.get();
//    init_info.Allocator = g_Allocator;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
//    init_info.CheckVkResultFn = check_vk_result;

    m_renderPass = createImGuiRenderPass(device, format).assertValue();
    m_renderPass.setObjectName("ImguiRendererRenderPass");

    ImGui_ImplVulkan_Init(&init_info, m_renderPass.get());


    for (const auto &imageView:m_pSwapchain->getImageViews()) {
        std::array<const vul::ImageView *, 1> imageViews = {&imageView};
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments(imageViews);
        framebufferBuilder.setDimensions(
                m_pSwapchain->getExtent());
        framebufferBuilder.setRenderPass(m_renderPass);
        m_framebuffers.push_back(
                framebufferBuilder.create().assertValue());
        m_framebuffers.back().setObjectName("ImguiRendererFrameBuffer" +
                                            std::to_string(
                                                    m_framebuffers.size() - 1));
    }

    commandPool.singleTimeSubmit(queue,
                                 [](vul::CommandBuffer &commandBuffer) {
                                     ImGui_ImplVulkan_CreateFontsTexture(
                                             commandBuffer.get());
                                 });
    ImGui_ImplVulkan_DestroyFontUploadObjects();

}

gul::ImguiRenderer::~ImguiRenderer() {
    m_pDevice->waitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

vul::ExpectedResult<vul::RenderPass>
gul::ImguiRenderer::createImGuiRenderPass(const vul::Device &device,
                                          vul::Format surfaceFormat) {
    VkAttachmentDescription attachment = {};
    attachment.format = vul::get(surfaceFormat);
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    VkRenderPass renderPass;
    auto result = static_cast<vul::Result>(vkCreateRenderPass(device.get(),
                                                              &info, nullptr,
                                                              &renderPass));
    return {result, vul::RenderPass(device, renderPass, 1, nullptr)};
}

void gul::ImguiRenderer::newFrame() const {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

std::function<void()> gul::ImguiRenderer::createResizeCallback(const vul::Queue& renderQueue) {
    auto resizeImGuiFramebuffers = [&device = *m_pDevice, &swapchain = *m_pSwapchain, &renderpass = m_renderPass, &framebuffers = m_framebuffers, &renderQueue]() {
        renderQueue.waitIdle();
        framebuffers.clear();
        const auto &swapchainImageViews = swapchain.getImageViews();
        for (const auto &imageView: swapchainImageViews) {
            std::array<const vul::ImageView *, 1> imageViews = {&imageView};
            vul::FramebufferBuilder framebufferBuilder(device);
            framebufferBuilder.setAttachments(imageViews);
            framebufferBuilder.setDimensions(
                    swapchain.getExtent());
            framebufferBuilder.setRenderPass(renderpass);
            framebuffers.push_back(
                    framebufferBuilder.create().assertValue());
            framebuffers.back().setObjectName("ImguiRendererFrameBuffer" +
                                              std::to_string(
                                                      framebuffers.size() - 1));
        }
    };
    return resizeImGuiFramebuffers;
}

void gul::ImguiRenderer::postSubmit() const {
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void
gul::ImguiRenderer::recordCommands(vul::PrimaryCommandBuffer &commandBuffer,
                                   std::uint32_t swapchainImageIndex) const {

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = {{1.0f, 0.5f, 1.0f, 1.0f}};
    auto extent = m_pSwapchain->getExtent();
    auto renderPassBlock = commandBuffer.beginRenderPass(
            m_renderPass,
            m_framebuffers[swapchainImageIndex],
            VkRect2D{{0, 0}, extent},
            clearValues);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                    commandBuffer.get());

}

void gul::ImguiRenderer::render() const {
    ImGui::Render();
}
