//
// Created by Shae Bolt on 6/5/2021.
//

#include <gul/imguirenderer.h>
#include <gul/noise/fbm.h>
#include <uul/math.h>
#include <gul/noise/opensimplex.h>
#include <iostream>
#include <gul/bitmask.h>
#include <gul/firstpersoncamera.h>
#include <gul/stbimage.h>
#include <gul/glfwwindow.h>
#include <vul/commandutils.h>
#include <vul/computepipeline.h>
#include <vul/vkstructutils.h>
#include <vul/sampler.h>
#include <vul/framebuffer.h>
#include <vul/imageview.h>
#include <vul/image.h>
#include <vul/buffer.h>
#include <vul/commandbuffer.h>
#include <vul/commandpool.h>
#include <vul/pipelinelayout.h>
#include <vul/graphicspipeline.h>
#include <vul/shadermodule.h>
#include <vul/ioutils.h>
#include <vul/descriptorsetlayout.h>
#include <vul/descriptorpool.h>
#include <vul/descriptorset.h>
#include <vul/renderpass.h>
#include <vul/semaphore.h>
#include <vul/vmaallocator.h>
#include <vul/queue.h>
#include <vul/device.h>
#include <vul/swapchain.h>
#include <vul/surface.h>
#include <vul/instance.h>
#include <vul/debugutils.h>
#include <vul/physicaldevice.h>
#include <vul/features.h>
#include <vul/bitmasks.h>
#include <vul/expectedresult.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>
#include <gsl/span>
#include <vul/temparrayproxy.h>
#include <optional>
#include <chrono>
#include <thread>
#include <span>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vul/traitutils.h>
#include <bitset>
#include <bit>
#include <random>


//see https://github.com/KhronosGroup/Vulkan-Samples/tree/master/samples/extensions
//see https://www.khronos.org/blog/vulkan-timeline-semaphores
//see https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/extensions/VK_KHR_synchronization2.md
//see https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    glm::vec3 camera_pos;
    float u_time;
};
static_assert(sizeof(UniformBufferObject) == 16 * 4 * 3 + 16);

struct alignas(8) FullScreenPushConstant {
    std::uint32_t image_index;
    std::uint32_t image_display_enum;
    float image_zoom;
    std::uint32_t u_ant_grid_dim;
    std::uint64_t u_ant_grid;
};

struct alignas(8) AntComputePushConstant {
    std::uint64_t u_ant_grid;
    std::uint64_t u_ant_pos;
    std::uint64_t u_ant_dir;

    std::uint32_t u_ant_grid_dim;
    std::uint32_t u_ant_count;
};

int main() {

    gul::GlfwWindow window(800, 600, "ExampleWindow");
    gul::FirstPersonCamera camera;
    camera.setPosition(glm::vec3(0.0, 0.0, -48.0));
    //camera.lookAt(glm::vec3(0.0,0.0,0.0));
    camera.setRotation(glm::vec3(0, 0, 0.0));

    bool framebufferResized = false;
    auto framebuffer_callback = [&framebufferResized](
            gul::GlfwWindow &/*window*/,
            int /*width*/, int /*height*/) {
        framebufferResized = true;
    };
    window.setFramebufferSizeCallback(framebuffer_callback);

    auto glfwExtensions = gul::GlfwWindow::getRequiredInstanceExtensions();
    auto instanceExtensions = ranges::views::concat(glfwExtensions,
                                                    vul::Instance::debugUtilsExtensions) |
                              ranges::to<std::vector>();
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    vul::Instance instance = vul::Instance::withDebugCallback(
            appInfo, vul::Instance::validationLayers,
            instanceExtensions).assertValue();

    vul::DebugUtilsMessengerEXT debugMessenger =
            vul::DebugCreateInfoBuilder(instance)
                    .setDefaults()
                    .create()
                    .assertValue();
    auto features = vul::Features::withMaxFeatureSetNVMaxwell();

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
    };
    //TODO fix issue where swapchain is both a member of surface, but must be deleted before device, need to move swapchain back out?
    vul::Surface surface = window.createSurface(instance);
    //TODO will need to capitalize letters?
    //srgb vul::Format::B8g8r8a8Srgb,
    const vul::SurfaceFormat defaultSurfaceFormat = {vul::Format::B8g8r8a8Unorm,
                                                     vul::ColorSpaceKHR::SrgbNonlinear};

    const auto defaultPresentMode = vul::PresentModeKHR::Mailbox;
    auto physicalDeviceOpt = pickPhysicalDevice(instance, surface, features,
                                                deviceExtensions,
                                                defaultSurfaceFormat,
                                                defaultPresentMode);

    if (!physicalDeviceOpt.has_value()) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    vul::PhysicalDevice physicalDevice = physicalDeviceOpt.value();

    auto graphicsComputeFamily = vul::QueueFlagBits::GraphicsBit |
                                 vul::QueueFlagBits::ComputeBit |
                                 vul::QueueFlagBits::TransferBit;
    auto computeTransferFamily = vul::QueueFlagBits::ComputeBit |
                                 vul::QueueFlagBits::TransferBit;
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    std::uint32_t presentQueueIndex = queueFamilyProperties.calcMinimumPresentationQueueFamilyIndex(surface,
                                                                                                    graphicsComputeFamily).value();
    std::uint32_t graphicsQueueIndex = queueFamilyProperties.calcMinimumQueueFamilyIndex(graphicsComputeFamily).value();
    std::uint32_t computeQueueIndex = queueFamilyProperties.calcMinimumQueueFamilyIndex(computeTransferFamily).value();
    std::vector<vul::SingleQueueCreateInfo> queueCreateInfos = {
            vul::SingleQueueCreateInfo{presentQueueIndex},
            vul::SingleQueueCreateInfo{computeQueueIndex}
    };
    auto device = physicalDevice.createDevice(queueCreateInfos, deviceExtensions,
                                              features).assertValue();
    //TODO what to do when queue is same for all but we try to parrallelize? only return references to queues? Have internal mutexes etc??
    //Make queues exclusive? that way you have to manually pass ownership? will get a queue per thing anyway, not a big deal.
    //TODO give queues knowledge of what index they correspond to so they can be returned to the device?
    //TODO Always combine present and graphics, no current implementation splits them up.
    vul::Queue presentationQueue = device.getQueueAt(0).value();
    vul::Queue computeQueue = device.getQueueAt(1).value();


    auto surfaceCapabilities = surface.getCapabilitiesFrom(physicalDevice);
    vul::SwapchainBuilder swapchainBuilder(device);
    //create surface capabilities object, create it from the surface as well, use it to create swapchain.
    //TODO take surface capabilities itself as an argument?
    swapchainBuilder
            .minImageCount(surfaceCapabilities.calcSwapchainImageCount())
            .surfaceFormat(defaultSurfaceFormat)
            .imageExtent(surfaceCapabilities.calcSwapchainExtent(
                    window.getFramebufferExtent()))
            .imageUsage(vul::ImageUsageFlagBits::ColorAttachmentBit)
            .preTransform(surfaceCapabilities.currentTransform)
            .presentMode(defaultPresentMode)
            .imageSharingMode(vul::SharingMode::Exclusive);

    //TODO graphics and present queue index as argument, or simply multiple indexes?
    //TODO Always combine present and graphics, no current implementation splits them up.
//    if (graphicsQueueIndex != presentQueueIndex) {
//        std::vector<std::uint32_t> presentQueueFamilyIndices = {graphicsQueueIndex, presentQueueIndex};
//        swapchainBuilder
//        .imageSharingMode(vul::SharingMode::Concurrent)
//        .queueFamilyIndices(presentQueueFamilyIndices);
//    } else {
//        swapchainBuilder.imageSharingMode(vul::SharingMode::Exclusive);
//    }


    auto swapchain = swapchainBuilder.create(surface).assertValue();

    auto allocator = vul::VmaAllocator::create(instance, physicalDevice,
                                               device,
                                               VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT).assertValue();

    const std::int32_t maxFramesInFlight = 2;
    std::uint64_t currentFrameIndex = 0;
    std::vector<vul::BinarySemaphore> binaryRenderFinishedSemaphores;
    std::vector<vul::BinarySemaphore> presentationFinishedSemaphores;
    std::vector<vul::TimelineSemaphore> renderFinishedSemaphores;
    for (auto i: ranges::views::ints(0, maxFramesInFlight)) {
        binaryRenderFinishedSemaphores.push_back(
                device.createBinarySemaphore().assertValue());
        presentationFinishedSemaphores.push_back(
                device.createBinarySemaphore().assertValue());
        renderFinishedSemaphores.push_back(
                device.createTimelineSemaphore(0).assertValue());
    }

    std::array validSwapchainResults = {vul::Result::SuboptimalKhr,
                                        vul::Result::ErrorOutOfDateKhr};
    std::uint64_t frame_counter = 0;

    auto subpassBuilder = vul::SubpassGraph(
            {vul::AttachmentDescription::PresentTemp(
                    swapchain.getFormat()),
             vul::AttachmentDescription::DepthTemp(
                     vul::Format::D24UnormS8Uint)}, 1);
    subpassBuilder.subpassAt(0).setWrites({0, 1})
            .setPreDependExternal(
                    vul::PipelineStageFlagBits::ColorAttachmentOutputBit |
                    vul::PipelineStageFlagBits::EarlyFragmentTestsBit,
                    vul::PipelineStageFlagBits::ColorAttachmentOutputBit |
                    vul::PipelineStageFlagBits::EarlyFragmentTestsBit,
                    {},
                    vul::AccessFlagBits::ColorAttachmentWriteBit |
                    vul::AccessFlagBits::DepthStencilAttachmentWriteBit);
    auto renderPass = subpassBuilder.createRenderPass(device).assertValue();
    renderPass.setObjectName("MyActualRenderPass");
    auto commandPool = device.createCommandPool(
            presentQueueIndex,
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();

    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setBindings({
                                                   vul::CombinedSamplerBinding(0,

                                                                               vul::ShaderStageFlagBits::FragmentBit,
                                                                               3).get(),
                                           });
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();


    vul::GraphicsPipeline graphicsPipeline;



    auto pipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            VkPushConstantRange{
                    (vul::ShaderStageFlagBits::VertexBit |
                     vul::ShaderStageFlagBits::FragmentBit).to_underlying(),
                    0,
                    sizeof(FullScreenPushConstant)
            }).assertValue();
//    auto pipelineLayout = device.createPipelineLayout(
//            descriptorLayout).assertValue();
    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);


    pipelineBuilder.setPrimitiveStateInfo(
            vul::PrimitiveTopology::TriangleList);
    pipelineBuilder.setViewportStateFromExtent(
            swapchain.getExtent());
    pipelineBuilder.setDynamicState(
            {vul::DynamicState::Viewport, vul::DynamicState::Scissor});
    pipelineBuilder.setDefaultRasterizationState();
    pipelineBuilder.setDefaultRasterizationState(vul::CullModeFlagBits::None);

    pipelineBuilder.setDefaultMultisampleState();
    pipelineBuilder.setDefaultDepthStencilStateEnable();
    VkPipelineColorBlendAttachmentState blendState = {};
    blendState.blendEnable = VK_FALSE;
    blendState.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;


    pipelineBuilder.setBlendState({blendState});
    pipelineBuilder.setRenderPass(renderPass, 0);
    pipelineBuilder.setPipelineLayout(pipelineLayout);
    {
        auto vertexShader = device.createShaderModule(
                vul::readSPIRV("spirv/fullscreen.vert.spv")).assertValue();
        auto fragmentShader = device.createShaderModule(
                vul::readSPIRV("spirv/fullscreen.frag.spv")).assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());
        graphicsPipeline = pipelineBuilder.create().assertValue();
    }


    vul::Image depthImage = allocator.createDeviceImage(
            vul::createSimple2DImageInfo(
                    vul::Format::D24UnormS8Uint,
                    swapchain.getExtent(),
                    vul::ImageUsageFlagBits::DepthStencilAttachmentBit)
    ).assertValue();

    auto depthImageView = depthImage.createImageView(vul::ImageSubresourceRange(
            vul::ImageAspectFlagBits::DepthBit)).assertValue();


    std::vector<vul::Framebuffer> swapchainFramebuffers;
    const auto &swapchainImageViews = swapchain.getImageViews();
    auto swapchainSize = static_cast<std::uint32_t>(swapchainImageViews.size());

    for (const auto &imageView: swapchainImageViews) {
        std::array<const vul::ImageView *, 2> imageViews = {&imageView,
                                                            &depthImageView};
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments(imageViews);
        framebufferBuilder.setDimensions(swapchain.getExtent());
        framebufferBuilder.setRenderPass(renderPass);
        swapchainFramebuffers.push_back(
                framebufferBuilder.create().assertValue());
    }

    auto resizeSwapchain = [&window, &presentationQueue, &swapchainBuilder, &swapchain,
            &depthImage, &depthImageView,
            &allocator, &swapchainFramebuffers, &device, &renderPass]() {
        auto size = window.getFramebufferSize();
        while (size == glm::ivec2(0)) {
            size = window.getFramebufferSize();
            glfwWaitEvents();
        }
        //TODO will need to recreate associated assets (like framebuffers or images that match the size of the window).
        //TODO need to add framebuffers to assets possibly managed by swapchain?
        presentationQueue.waitIdle();


        swapchainFramebuffers.clear();

        swapchain = swapchainBuilder.resize(swapchain, window.getFramebufferExtent()).assertValue();
//        surface.resizeSwapchain(swapchainBuilder,
//                                window.getFramebufferExtent());

//        swapchainBuilder.imageExtent(window.getFramebufferExtent());
//        surface.createSwapchain(swapchainBuilder);

        depthImage = allocator.createDeviceImage(
                vul::createSimple2DImageInfo(
                        vul::Format::D24UnormS8Uint,
                        swapchain.getExtent(),
                        vul::ImageUsageFlagBits::DepthStencilAttachmentBit)
        ).assertValue();

        depthImageView = depthImage.createImageView(vul::ImageSubresourceRange(
                vul::ImageAspectFlagBits::DepthBit)).assertValue();
        const auto &swapchainImageViews = swapchain.getImageViews();
        for (const auto &imageView: swapchainImageViews) {
            std::array<const vul::ImageView *, 2> imageViews = {&imageView,
                                                                &depthImageView};
            vul::FramebufferBuilder framebufferBuilder(device);
            framebufferBuilder.setAttachments(imageViews);
            framebufferBuilder.setDimensions(
                    swapchain.getExtent());
            framebufferBuilder.setRenderPass(renderPass);
            swapchainFramebuffers.push_back(
                    framebufferBuilder.create().assertValue());
        }
    };


    std::uint32_t grid_width = 128;
    std::uint32_t grid_height = 128;
    std::vector<vul::Buffer> host_staging_buffers;
    std::vector<vul::Image> device_grid_images;
    std::vector<vul::ImageView> device_grid_views;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        device_grid_images.push_back(allocator.createDeviceImage(
                vul::createSimple2DImageInfo(
                        vul::Format::R8g8b8a8Srgb,
                        vul::Extent2D(grid_width, grid_height),
                        vul::ImageUsageFlagBits::TransferDstBit |
                        vul::ImageUsageFlagBits::SampledBit)).assertValue());
        device_grid_views.push_back(device_grid_images.back().createImageView(vul::ImageSubresourceRange(
                vul::ImageAspectFlagBits::ColorBit)).assertValue());
        host_staging_buffers.push_back(allocator.createStagingBuffer(128 * 128 * 4).assertValue());
        device_grid_images.back().setDebugObjectName(fmt::format("DEVICE_IMAGE_{}", i));
//        device_grid_views.back().setDebugObjectName(fmt::format("IMAGE_{}", i));
        host_staging_buffers.back().setDebugObjectName(fmt::format("HOST_BUFFER_{}", i));

        vul::transition(device_grid_images.back(), commandPool,
                        presentationQueue,
                        vul::ImageAspectFlagBits::ColorBit,
                        vul::PipelineStageFlagBits2::AllCommandsBit,
                        vul::AccessFlagBits2::ShaderReadBit,
                        vul::ImageLayout::ReadOnlyOptimal);
    }

    struct alignas(4) ColorRGBA {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };

    std::vector<ColorRGBA> colors(128 * 128, ColorRGBA{0xFFu, 0xFF, 0x0FF, 0xFF});
    for (std::size_t i = 0; i < 64; ++i) {
        for (std::size_t j = 0; j < 64; ++j) {
            colors[i * 128 + j] = ColorRGBA{0xFFu, 0x00u, 0x00u, 0xFFu};
        }
    }

    for (std::size_t i = 0; i < swapchainSize; ++i) {
        host_staging_buffers[i].mappedCopyFromArray(colors);
    }

    vul::SamplerBuilder samplerBuilder(device);
    samplerBuilder.setFilter(vul::Filter::Nearest);
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::ClampToBorder);
    samplerBuilder.setBorderColor(vul::BorderColor::FloatOpaqueBlack);
    samplerBuilder.enableAnisotropy();
    samplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto sampler = samplerBuilder.create().assertValue();


    auto antWorldPipelineLayout = device.createPipelineLayout(
            VkPushConstantRange{
                    static_cast<VkShaderStageFlags>(vul::ShaderStageFlagBits::ComputeBit),
                    0,
                    sizeof(AntComputePushConstant)
            }).assertValue();
    auto computeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline antWorldPipeline;
    {
        auto computeShader = device.createShaderModule(
                vul::readSPIRV("spirv/antkernel.comp.spv")).assertValue();
        computeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        computeBuilder.setPipelineLayout(antWorldPipelineLayout);
        antWorldPipeline = computeBuilder.create().assertValue();
    }
    auto descriptorPool = device.createDescriptorPool(
            {{descriptorSetLayoutBuilder,
              swapchainSize}}).assertValue();
    //, vul::DescriptorPoolCreateFlagBits::UpdateAfterBindBit


    auto descriptorSets = descriptorPool.createDescriptorSets(
            {{descriptorLayout, swapchainSize}}).assertValue();

    for (const auto &[i, descriptorSet]: descriptorSets |
                                         ranges::views::enumerate) {
        auto updateBuilder = descriptorSetLayoutBuilder.createUpdateBuilder();
        updateBuilder.getDescriptorElementAt(0).setCombinedImageSampler(
                device_grid_views | ranges::views::transform(
                        [&sampler](auto &value) {
                            return value.createDescriptorInfo(
                                    sampler,
                                    vul::ImageLayout::ReadOnlyOptimal);
                        }) |
                ranges::to<std::vector>());
//        updateBuilder.getDescriptorElementAt(0).setCombinedImageSampler({
//                                                                                device_grid_views[i].createDescriptorInfo(
//                                                                                        sampler,
//                                                                                        vul::ImageLayout::ShaderReadOnlyOptimal
//                                                                                )}
//        );
        auto updates = updateBuilder.create(descriptorSet);
        device.updateDescriptorSets(updates);
    }


    auto commandBuffers = commandPool.createPrimaryCommandBuffers(
            swapchainSize).assertValue();

    gul::ImguiRenderer imguiRenderer(window, instance, device, swapchain,
                                     presentQueueIndex, presentationQueue,
                                     swapchain.getFormat());

    auto resizeWindow = [&resizeSwapchain, resizeImGuiFramebuffers = imguiRenderer.createResizeCallback(
            presentationQueue)]() {
        resizeSwapchain();
        resizeImGuiFramebuffers();
    };

    std::int32_t spread = 64;
    std::random_device dev;
    std::minstd_rand lcg(dev());
    std::uniform_int_distribution<std::int32_t> int_dist(-spread, spread);
    std::uniform_int_distribution<std::uint32_t> dir_dist(0u, 3u);
    std::uint32_t ant_count = 128 * 4;
    std::uint32_t grid_dim = 256 * 32;
    glm::uvec2 grid_center = glm::uvec2(grid_dim, grid_dim) / 2u;
    std::vector<std::uint8_t> ant_grid(grid_dim * grid_dim, 0u);
    std::vector<glm::uvec2> ant_pos(ant_count);
    std::vector<std::uint8_t> ant_dir(ant_count);
    for (auto [pos, dir]: ranges::views::zip(ant_pos, ant_dir)) {
        pos = static_cast<glm::uvec2>(glm::ivec2(grid_center) + glm::ivec2(int_dist(lcg), int_dist(lcg)));
        dir = static_cast<std::uint8_t>(dir_dist(lcg));
    }

    //        ant_positions += [(grid_size // 2 + random.randint(-spread, spread), grid_size // 2 + random.randint(-spread, spread))]
    //        ant_directions += [random.randint(0, 3)]
    auto device_ant_grid = allocator.createDeviceBuffer(commandPool,
                                                        presentationQueue,
                                                        ant_grid,
                                                        vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_ant_pos = allocator.createDeviceBuffer(commandPool,
                                                        presentationQueue,
                                                       ant_pos,
                                                        vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_ant_dir = allocator.createDeviceBuffer(commandPool,
                                                       presentationQueue,
                                                       ant_dir,
                                                       vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();


    AntComputePushConstant antWorldPushConstant = {
            device_ant_grid.getDeviceAddress(),
            device_ant_pos.getDeviceAddress(),
            device_ant_dir.getDeviceAddress(),
            grid_dim,
            ant_count
    };

    FullScreenPushConstant grid_push_constant = {
            0,
            0,
            1.0f,
            grid_dim,
            device_ant_grid.getDeviceAddress(),
    };

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::uint64_t> frameCounters(renderFinishedSemaphores.size(),
                                             0);


    while (!window.shouldClose()) {
        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1000us);
//        std::this_thread::sleep_for(32ms) ;
//        presentationQueue.waitIdle();
        glfwPollEvents();
        imguiRenderer.newFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        auto cursor_pos = window.getCursorPosition();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin(
                    "Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text(
                    "This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window",
                            &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f,
                               1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color",
                              (float *) &clear_color); // Edit 3 floats representing a color

            if (ImGui::Button(
                    "Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);

            ImGui::Text(fmt::format("cursor_pos x:{},y:{}", cursor_pos.x,
                                    cursor_pos.y).c_str());
            ImGui::Text(fmt::format("CameraPos {},{},{}\n", camera.getPosition().x, camera.getPosition().y,
                                    camera.getPosition().z).c_str());
            ImGui::End();
        }



        // 3. Show another simple window.
        if (show_another_window) {
            ImGui::Begin("Another Window",
                         &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;

            ImGui::End();
        }



        //TODO do actual timeline semaphore update here. use framecounter, not sure how, will need to make sure value is <= actual signal value.
        renderFinishedSemaphores[currentFrameIndex].wait(
                frameCounters[currentFrameIndex]);
        auto &presentationFinishedSemaphore = presentationFinishedSemaphores[currentFrameIndex];
        auto &binaryRenderFinishedSemaphore = binaryRenderFinishedSemaphores[currentFrameIndex];
        auto swapchainImageIndexResult = swapchain.acquireNextImage(
                presentationFinishedSemaphore);

        if (swapchainImageIndexResult.result ==
            vul::Result::ErrorOutOfDateKhr) {
            resizeWindow();
            continue;
        }
        auto swapchainImageIndex = swapchainImageIndexResult.assertValue(
                validSwapchainResults);

        {
            static auto startTime = std::chrono::high_resolution_clock::now();
            static auto lastTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(
                    currentTime - startTime).count();
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                    currentTime - lastTime).count();
            lastTime = currentTime;


            bool toggle_spread = false;
            bool toggle_density = false;
            ImGuiIO &io = ImGui::GetIO();

            if (!io.WantCaptureKeyboard) {
                float rotate_speed = 1.5;
                float move_speed = 15;

                bool forward = false;
                bool back = false;
                bool left = false;
                bool right = false;
                bool up = false;
                bool down = false;
                if (window.keyPressed(GLFW_KEY_W)) {
                    forward = true;
                }
                if (window.keyPressed(GLFW_KEY_S)) {
                    back = true;
                }
                if (window.keyPressed(GLFW_KEY_A)) {
                    left = true;
                }
                if (window.keyPressed(GLFW_KEY_D)) {
                    right = true;
                }
                if (window.keyPressed(GLFW_KEY_SPACE)) {
                    up = true;
                }
                if (window.keyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    down = true;
                }
                if (window.keyPressed(GLFW_KEY_1)) {
                    toggle_spread = !toggle_spread;
                }
                if (window.keyPressed(GLFW_KEY_2)) {
                    toggle_density = !toggle_density;
                }
                camera.move(forward, back, left, right, up, down, deltaTime,
                            move_speed);

                bool rotate_up = false;
                bool rotate_down = false;
                bool rotate_left = false;
                bool rotate_right = false;
                if (window.keyPressed(GLFW_KEY_UP)) {
                    rotate_up = true;
                }
                if (window.keyPressed(GLFW_KEY_DOWN)) {
                    rotate_down = true;
                }
                if (window.keyPressed(GLFW_KEY_LEFT)) {
                    rotate_left = true;
                }
                if (window.keyPressed(GLFW_KEY_RIGHT)) {
                    rotate_right = true;
                }
                camera.rotate(rotate_up, rotate_down, rotate_left, rotate_right,
                              deltaTime, rotate_speed);
            }


            UniformBufferObject ubo = {};
            ubo.model = glm::identity<glm::mat4x4>();
//            ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),
//                        glm::vec3(1.0f, 0.0f, 0.0f));
//            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
//                                    glm::vec3(0.0f, 1.0f, 0.0f));
//            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
//                                   glm::vec3(0.0f, 0.0f, 0.0f),
//                                   glm::vec3(0.0f, 1.0f, 0.0f));
            ubo.view = camera.calcViewMatrix();
            ubo.proj = glm::perspective(glm::radians(45.0f),
                                        swapchain.getExtent().width /
                                        (float) swapchain.getExtent().height,
                                        0.1f,
                                        1000.0f);

            ubo.proj[1][1] *= -1;
            static float u_time = 0;
            ubo.u_time = time;
            ubo.camera_pos = camera.getPosition();
            u_time += 1.0f;
        }


        imguiRenderer.render();


        auto &commandBuffer = commandBuffers[swapchainImageIndex];
        {
            commandBuffer.begin(
                    vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
            {
                commandBuffer.bindPipeline(antWorldPipeline);
                auto computeComputeBarrier = vul::createComputeBarrierRWARW();
                auto computeComputeDepInfo = vul::createDependencyInfo(
                        computeComputeBarrier, {}, {});
                commandBuffer.pushConstants(antWorldPipelineLayout,
                                            vul::ShaderStageFlagBits::ComputeBit,
                                            antWorldPushConstant);
                auto workgroup_dim = uul::integer_ceil(ant_count, 1024);
                commandBuffer.pipelineBarrier(computeComputeDepInfo);
                commandBuffer.dispatch(workgroup_dim, 1, 1);
                auto computeGraphicsBarrier = vul::createComputeFragmentBarrierRAW();
                auto computeGraphicsDepInfo = vul::createDependencyInfo(
                        computeGraphicsBarrier, {}, {});
                commandBuffer.pipelineBarrier(computeGraphicsDepInfo);
            }
#if 0
            {

                auto toTransferBarrier = device_grid_images[swapchainImageIndex].createToTransferBarrier(
                        vul::PipelineStageFlagBits2::None,
                        vul::AccessFlagBits2::None,
                        vul::ImageSubresourceRange(vul::ImageAspectFlagBits::ColorBit));
                auto fromTransferBarrier = device_grid_images[swapchainImageIndex].createFromnTransferBarrier(
                        vul::PipelineStageFlagBits2::FragmentShaderBit,
                        vul::AccessFlagBits2::ShaderReadBit,
                        vul::ImageLayout::ReadOnlyOptimal,
                        vul::ImageSubresourceRange(vul::ImageAspectFlagBits::ColorBit));
                {
                    VkDependencyInfoKHR dependencyInfo = {};
                    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
                    dependencyInfo.pNext = nullptr;
                    dependencyInfo.dependencyFlags = {};
                    dependencyInfo.memoryBarrierCount = 0;
                    dependencyInfo.pMemoryBarriers = nullptr;
                    dependencyInfo.bufferMemoryBarrierCount = 0;
                    dependencyInfo.pBufferMemoryBarriers = nullptr;
                    dependencyInfo.imageMemoryBarrierCount = 1;
                    dependencyInfo.pImageMemoryBarriers = &toTransferBarrier;
                    commandBuffer.pipelineBarrier(dependencyInfo);
                }

                commandBuffer.copyBufferToImage(
                        host_staging_buffers[swapchainImageIndex],
                        device_grid_images[swapchainImageIndex],
                        vul::ImageAspectFlagBits::ColorBit,
                        0,
                        1);

                {
                    VkDependencyInfoKHR dependencyInfo = {};
                    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
                    dependencyInfo.pNext = nullptr;
                    dependencyInfo.dependencyFlags = {};
                    dependencyInfo.memoryBarrierCount = 0;
                    dependencyInfo.pMemoryBarriers = nullptr;
                    dependencyInfo.bufferMemoryBarrierCount = 0;
                    dependencyInfo.pBufferMemoryBarriers = nullptr;
                    dependencyInfo.imageMemoryBarrierCount = 1;
                    dependencyInfo.pImageMemoryBarriers = &fromTransferBarrier;
                    commandBuffer.pipelineBarrier(dependencyInfo);
                }

            }
#endif
            {
                auto extent = swapchain.getExtent();
                commandBuffer.setViewport(vul::Viewport(extent).get());
                commandBuffer.setScissor(vul::Rect2D(extent).get());

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {{1.0f, 0.5f, 1.0f, 1.0f}};
                clearValues[0].color = {
                        {clear_color.x, clear_color.y, clear_color.z,
                         clear_color.w}};
                clearValues[1].depthStencil = {1.0f, 0};
                auto renderPassBlock = commandBuffer.beginRenderPass(
                        renderPass,
                        swapchainFramebuffers[swapchainImageIndex],
                        VkRect2D{{0, 0}, extent},
                        clearValues);
                commandBuffer.bindPipeline(graphicsPipeline);
                commandBuffer.bindDescriptorSets(
                        vul::PipelineBindPoint::Graphics, pipelineLayout,
                        descriptorSets[swapchainImageIndex]);
                grid_push_constant.image_index = frame_counter %
                                                 swapchainSize;
                commandBuffer.pushConstants(pipelineLayout,
                                            vul::ShaderStageFlagBits::VertexBit |
                                            vul::ShaderStageFlagBits::FragmentBit,
                                            grid_push_constant);
                renderPassBlock.draw(3);
            }
            imguiRenderer.recordCommands(commandBuffer, swapchainImageIndex);
            commandBuffer.end();
        }


        frameCounters[currentFrameIndex] += 1;
        auto presentationWaitInfo = presentationFinishedSemaphore.createSubmitInfo(
                vul::PipelineStageFlagBits2::ColorAttachmentOutputBit);
        std::array<VkSemaphoreSubmitInfoKHR, 2> signalInfos;
        signalInfos[0] = renderFinishedSemaphores[currentFrameIndex].createSubmitInfo(
                frameCounters[currentFrameIndex],
                vul::PipelineStageFlagBits2::AllCommandsBit);
        signalInfos[1] = binaryRenderFinishedSemaphore.createSubmitInfo(
                vul::PipelineStageFlagBits2::AllCommandsBit);
        auto commandBufferInfo = commandBuffer.createSubmitInfo();

        VkSubmitInfo2KHR submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
        submitInfo.pNext = nullptr; // All 3 struct above are built into VkSubmitInfo2KHR
        submitInfo.flags = 0; // VK_SUBMIT_PROTECTED_BIT_KHR also can be zero, replaces VkProtectedSubmitInfo
        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = &presentationWaitInfo;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferInfo;
        submitInfo.signalSemaphoreInfoCount = signalInfos.size();
        submitInfo.pSignalSemaphoreInfos = signalInfos.data();


        presentationQueue.submit(submitInfo);
        //TODO do actual render here

        imguiRenderer.postSubmit();

        auto presentResult = swapchain.present(presentationQueue,
                                               binaryRenderFinishedSemaphore,
                                               swapchainImageIndex);


        if (presentResult == vul::Result::ErrorOutOfDateKhr ||
            presentResult == vul::Result::SuboptimalKhr || framebufferResized) {
            framebufferResized = false;
            resizeWindow();
        } else if (presentResult != vul::Result::Success) {
            throw std::runtime_error("failed to present swapchain image");
        }


        currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight;
        frame_counter += 1;

    }
    device.waitIdle();
    return 0;
}