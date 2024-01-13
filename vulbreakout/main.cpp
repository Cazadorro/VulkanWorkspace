//
// Created by Shae Bolt on 6/5/2021.
//

#include <vul/writedescriptorset.h>
#include <vul/dependencyinfo.h>
#include <vul/memorybarrier.h>
#include <vul/imagecreateinfo.h>
#include <vul/imagememorybarrier.h>
#include <vul/buffermemorybarrier.h>
#include <vul/colorformatutils.h>
#include <gul/imguirenderer.h>
#include <gul/noise/fbm.h>
#include <czdr/bitutil/bit.h>
#include <czdr/stdutil/math.h>
#include <czdr/stdutil/math_literals.h>
#include <gul/noise/opensimplex.h>
#include <iostream>
#include <gul/bitmask.h>
#include <gul/firstpersoncamera.h>
#include <gul/stbimage.h>

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
#include <vul/pushconstantrange.h>
#include <vul/debugutils.h>
#include <vul/physicaldevice.h>
#include <vul/features.h>
#include <vul/expectedresult.h>
#include <vul/deviceaddress.h>
#include <vul/submitinfo.h>
#include <vul/submitinfobuilder.h>
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
#include <vulkan/vulkan.hpp>

#include <vector>
#include <array>
//see https://github.com/KhronosGroup/Vulkan-Samples/tree/master/samples/extensions
//see https://www.khronos.org/blog/vulkan-timeline-semaphores
//see https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/extensions/VK_KHR_synchronization2.md
//see https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples

#include <string_view>

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    glm::vec3 camera_pos;
    float u_time;
};
static_assert(sizeof(UniformBufferObject) == 16 * 4 * 3 + 16);


struct RenderSprites2dPushConstantBlock {
    vul::DeviceAddress u_affine_matrices; // 4 * 2 * 3 = 24 bytes. starts at 0.
    vul::DeviceAddress u_material_ids; // 24;
};
static_assert(sizeof(RenderSprites2dPushConstantBlock) == 16);

struct alignas(8) RenderFullscreenPushConstantBlock {
    std::uint32_t u_material_id;
};
static_assert(sizeof(RenderFullscreenPushConstantBlock) == 8);


struct alignas(8) RenderGridSpritesPushConstantBlock {
    vul::DeviceAddress u_complex_material_ids; //8
    float u_element_width; //16
    float u_element_height; //24
    std::uint32_t u_width;
    std::uint32_t u_height;  //32
    glm::mat3x2 u_affine_matrix;
};
static_assert(sizeof(RenderGridSpritesPushConstantBlock) == 48);



#include <czdr/glfw/core.h>
#include <glm/gtx/matrix_transform_2d.hpp>
int main() {

    glfw::Library glfw_lib;
    glfw_lib.window_hint_client_api(glfw::OpenglClientApiType::NoApi);
    auto window = glfw_lib.create_window(800, 600, "Example Window").value();
    gul::FirstPersonCamera camera;
    camera.setPosition(glm::vec3(0.0, 0.0, -48.0));
    //camera.lookAt(glm::vec3(0.0,0.0,0.0));
    camera.setRotation(glm::vec3(0, 0, 0.0));

    bool framebufferResized = false;
    auto framebuffer_callback = [&framebufferResized](
            glfw::Window */*window*/,
            glfw::PixelSize<int> /**/) {
        framebufferResized = true;
    };

    window.set_framebuffer_size_callback(framebuffer_callback);

    auto glfwExtensions = glfw_lib.get_required_instance_extensions();
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
//    features.physicalDeviceVulkan12Features.shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE;
//    features.physicalDeviceVulkan12Features.shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
//    features.physicalDeviceVulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
//    features.physicalDeviceVulkan12Features.descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
    };
    //TODO fix issue where swapchain is both a member of surface, but must be deleted before device, need to move swapchain back out?
    vul::Surface surface = window.create_surface(instance).assertValue();
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
    auto transferFamily = vul::QueueFlagBits::TransferBit;

    auto device = physicalDevice.createDevice(surface,
                                              {graphicsComputeFamily},
                                              {computeTransferFamily, transferFamily},
                                              deviceExtensions,
                                              features).assertValue();
    //TODO what to do when queue is same for all but we try to parrallelize? only return references to queues? Have internal mutexes etc??
    //Make queues exclusive? that way you have to manually pass ownership? will get a queue per thing anyway, not a big deal.
    //TODO give queues knowledge of what index they correspond to so they can be returned to the device?
    //TODO Always combine present and graphics, no current implementation splits them up.
    vul::Queue presentationQueue = device.getQueueAt(0).value();
    vul::Queue computeQueue = device.getQueueAt(1).value();
    vul::Queue transferQueue = device.getQueueAt(2).value();


    auto surfaceCapabilities = surface.getCapabilitiesFrom(physicalDevice);
    vul::SwapchainBuilder swapchainBuilder(device);
    //create surface capabilities object, create it from the surface as well, use it to create swapchain.
    //TODO take surface capabilities itself as an argument?
    swapchainBuilder
            .minImageCount(surfaceCapabilities.calcSwapchainImageCount())
            .surfaceFormat(defaultSurfaceFormat)
            .imageExtent(surfaceCapabilities.calcSwapchainExtent(
                    window.get_framebuffer_extent()))
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
    const auto &swapchainImageViews = swapchain.getImageViews();
    auto swapchainSize = static_cast<std::uint32_t>(swapchainImageViews.size());
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
    //value used to determine which host buffer to use to copy on transfer.
    //next buffer = 0 + 1, current semaphore = 0; t % 2 if !=, upload index, swap buffers to use. otherwise keep updating current buffer.
    vul::TimelineSemaphore uploadFinishedSemaphore =  device.createTimelineSemaphore(0).assertValue();
    //value used to determine which device buffer can be used to copy to.
    //copy to next buffer when all frames in flight sit on buffer , swapchain size used here
    // swapchain size of 3 means x % 6 = 0->5, if 0,1,2,3 0,3 good, if 1,2,4,5 not done.  if 0 means done at first.
    // signal when specifying using given semiphore.
    vul::TimelineSemaphore framesUsingSemaphore =  device.createTimelineSemaphore(0).assertValue();

    std::array validSwapchainResults = {vul::Result::SuboptimalKhr,
                                        vul::Result::ErrorOutOfDateKhr};
    std::uint64_t frame_counter = 0;

    //TODO make single subpass renderpass vulkan automatically?
    //TODO could just do binary search through texels
    //TODO need to merge texel coordinates with subsequent frames.
    //TODO LRU cache design for texels?

//https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#renderpass-compatibility Note renderpasses may be compatible.

//    8.3. Render Pass Compatibility
//
//    Framebuffers and graphics pipelines are created based on a specific render pass object. They must only be used with that render pass object, *or* one compatible with it.
//
//    Two attachment references are compatible if they have matching format and sample count, or are both VK_ATTACHMENT_UNUSED or the pointer that would contain the reference is NULL.
//
//    Two arrays of attachment references are compatible if all corresponding pairs of attachments are compatible. If the arrays are of different lengths, attachment references not present in the smaller array are treated as VK_ATTACHMENT_UNUSED.
//
//    Two render passes are compatible if their corresponding color, input, resolve, and depth/stencil attachment references are compatible and if they are otherwise identical except for:
//
//    * Initial and final image layout in attachment descriptions
//    * Load and store operations in attachment descriptions
//    * Image layout in attachment references
//
//    As an additional special case, if two render passes have a single subpass, the resolve attachment reference and depth/stencil resolve mode compatibility requirements are ignored.
//
//    A framebuffer is compatible with a render pass if it was created using the same render pass or a compatible render pass.
//
// Basically if we use the same attachments with the same formats, we should have compatible renderpasses and pipelines with the same renderpass this means:
// * we don't have to create different framebuffers for minor changes in renderpasses, for example, load/store ops in attachments
// * we don't have to create different graphicspipelines for minor changes in renderpasses for exmaple, load/store ops in attachments
// * renderpasses with supersets of another renderpass are compatible.
// * you can ignore the highligheted fields below in terms of compatibliity (flags format and samples need to be the same).
//    VkAttachmentDescriptionFlags    flags;
//    VkFormat                        format;
//    VkSampleCountFlagBits           samples;
//    *VkAttachmentLoadOp              loadOp;
//    *VkAttachmentStoreOp             storeOp;
//    *VkAttachmentLoadOp              stencilLoadOp;
//    *VkAttachmentStoreOp             stencilStoreOp;
//    *VkImageLayout                   initialLayout;
//    *VkImageLayout                   finalLayout;
// * Renderpasses need to have the *same* number of subpasses to be compatible
// * AND subpasses and *subpass dependencies* must *exactly match*
// * There's special exceptions for single subpass renderpasses for compatibliity with resolve attachments and depthstencil resolve mode compatibliity
// * *if* we impose no subpass dependency we can just use presentationRenderPass for all pipelines since we assume the same renderpass comatibility all the way through the program.
// * we need to ignore subpass dependency otherwise. based on this:
// https://github.com/KhronosGroup/Vulkan-Docs/issues/726
// and instead use the equivalent pipeline barriers.
    auto firstColorPassBuilder = vul::RenderPassBuilder::createSimpleRenderPassBuilder(vul::AttachmentReusePolicy::First, {swapchain.getFormat()}, vul::AttachmentReusePolicy::First, vul::Format::D24UnormS8Uint);
    auto nextColorPassBuilder = vul::RenderPassBuilder::createSimpleRenderPassBuilder(vul::AttachmentReusePolicy::Next, {swapchain.getFormat()}, vul::AttachmentReusePolicy::Next, vul::Format::D24UnormS8Uint);
    auto presentationRenderPassBuilder = vul::RenderPassBuilder::createLastPresentationRenderPassBuilder(swapchain, vul::Format::D24UnormS8Uint);
// this was for depth being re-used later. We now have seperate depth attachments for each frame in flight.
//    subpassBuilder.add(vul::ExternalPreSubpassDependency{
//            vul::PipelineStageFlagBits::ColorAttachmentOutputBit |
//            vul::PipelineStageFlagBits::EarlyFragmentTestsBit,
//            vul::PipelineStageFlagBits::ColorAttachmentOutputBit |
//            vul::PipelineStageFlagBits::EarlyFragmentTestsBit,
//            {},
//            vul::AccessFlagBits::ColorAttachmentWriteBit |
//            vul::AccessFlagBits::DepthStencilAttachmentWriteBit
//    });
    auto firstColorPass = firstColorPassBuilder.createRenderPass(device).assertValue();
    firstColorPass.setObjectName("firstColorPass");
    auto nextColorPass = nextColorPassBuilder.createRenderPass(device).assertValue();
    firstColorPass.setObjectName("nextColorPass");
    auto presentationRenderPass = presentationRenderPassBuilder.createRenderPass(device).assertValue();
    presentationRenderPass.setObjectName("presentationRenderPass");
    auto commandPool = device.createCommandPool(
            presentationQueue.getQueueFamilyIndex(),
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();

    auto images_to_load = 5;
    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setFlags(vul::DescriptorSetLayoutCreateFlagBits::UpdateAfterBindPoolBit)
    .setBindings({vul::UniformBufferBinding(0,
                                                                      vul::ShaderStageFlagBits::VertexBit |
                                                                      vul::ShaderStageFlagBits::FragmentBit),
                                            vul::SamplerBinding(1,
                                                                vul::ShaderStageFlagBits::FragmentBit),
                                            vul::SampledImageBinding(2,
                                                                     vul::ShaderStageFlagBits::FragmentBit,
                                                                     images_to_load),
                                           },
                                           {vul::DescriptorBindingFlagBits::UpdateAfterBindBit});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();


    auto org = glm::vec3(6.5f, -3.0f, 6.5f);
    auto dir = normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    auto org_offset = glm::vec3(0.0f, 1.0f, 0.0f) * 0.001f;


//    std::vector<std::array<glm::mat3x2, 1024>> host_n_buffered_affine_matrices;
//    std::vector<std::array<std::uint32_t, 1024>> host_n_buffered_material_ids;


    std::array<glm::mat3x2, 1024> affine_matrices{};
    std::array<std::uint32_t, 1024> material_ids{};

    std::vector<vul::Buffer> host_staging_affine_matrices_arrays;
    std::vector<vul::Buffer> host_staging_material_id_arrays;
    //simplest way is to update data every time?
    //or hold on to currently updated,
    //a buffer for an array of values for each swapchain.
    std::vector<vul::Buffer> device_affine_matrices_arrays;
    std::vector<vul::Buffer> device_material_id_arrays;
    constexpr std::size_t  n_buffer_size = 2;
    for (std::size_t i = 0; i < n_buffer_size; ++i) {
        host_staging_affine_matrices_arrays.push_back(allocator.createMappedCoherentBuffer(
                sizeof(glm::mat3x2) * 1024,
                vul::BufferUsageFlagBits::TransferSrcBit).assertValue());
        host_staging_material_id_arrays.push_back(allocator.createMappedCoherentBuffer(
                sizeof(std::uint32_t) * 1024,
                vul::BufferUsageFlagBits::TransferSrcBit).assertValue());
        device_affine_matrices_arrays.push_back(allocator.createDeviceBuffer(sizeof(glm::mat3x2) * 1024,
                                                                             vul::BufferUsageFlagBits::ShaderDeviceAddressBit |
                                                                             vul::BufferUsageFlagBits::TransferDstBit).assertValue());
        device_material_id_arrays.push_back(allocator.createDeviceBuffer(sizeof(std::uint32_t) * 1024,
                                                                         vul::BufferUsageFlagBits::ShaderDeviceAddressBit |
                                                                         vul::BufferUsageFlagBits::TransferDstBit).assertValue());
    }

    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);
    pipelineBuilder.setPrimitiveStateInfo();
    pipelineBuilder.setViewportStateFromExtent(swapchain.getExtent());
    pipelineBuilder.setDynamicState({vul::DynamicState::Viewport, vul::DynamicState::Scissor});
    pipelineBuilder.setRasterizationState(vul::PipelineRasterizationStateCreateInfo::fromCullMode(vul::CullModeFlagBits::None));
    pipelineBuilder.setMultisampleState();
    pipelineBuilder.setDepthStencilState(vul::PipelineDepthStencilStateCreateInfo::fromCompareOp(vul::CompareOp::LessOrEqual));
    pipelineBuilder.setBlendState({vul::PipelineColorBlendAttachmentState::disableBlendRGBA()});
    pipelineBuilder.setRenderPass(presentationRenderPass, 0);

    vul::GraphicsPipeline renderSprites2dPipeline;

    auto renderSprites2dPipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            vul::PushConstantRange::create<RenderSprites2dPushConstantBlock>((vul::ShaderStageFlagBits::VertexBit |
                                                                              vul::ShaderStageFlagBits::FragmentBit))
    ).assertValue();
    {
        pipelineBuilder.setPipelineLayout(renderSprites2dPipelineLayout);
        {
            auto vertexShader = device.createShaderModule("spirv/render_sprites_2d.vert.spv").assertValue();
            auto fragmentShader = device.createShaderModule("spirv/render_sprites_2d.frag.spv").assertValue();
            pipelineBuilder.setShaderCreateInfo(
                    vertexShader.createVertexStageInfo(),
                    fragmentShader.createFragmentStageInfo());
            renderSprites2dPipeline = pipelineBuilder.create().assertValue();
        }
    }
    vul::GraphicsPipeline renderFullscreenTexturePipeline;

    auto renderFullscreenTexturePipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            vul::PushConstantRange::create<RenderSprites2dPushConstantBlock>((vul::ShaderStageFlagBits::VertexBit |
                                                                              vul::ShaderStageFlagBits::FragmentBit))
    ).assertValue();
    {

        pipelineBuilder.setPipelineLayout(renderFullscreenTexturePipelineLayout);
        {
            auto vertexShader = device.createShaderModule("spirv/render_fullscreen_texture.vert.spv").assertValue();
            auto fragmentShader = device.createShaderModule("spirv/render_fullscreen_texture.frag.spv").assertValue();
            pipelineBuilder.setShaderCreateInfo(
                    vertexShader.createVertexStageInfo(),
                    fragmentShader.createFragmentStageInfo());
            renderFullscreenTexturePipeline = pipelineBuilder.create().assertValue();
        }
    }

    vul::GraphicsPipeline renderGridSprites2dPipeline;

    auto renderGridSprites2dPipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            vul::PushConstantRange::create<RenderGridSpritesPushConstantBlock>((vul::ShaderStageFlagBits::VertexBit |
                                                                              vul::ShaderStageFlagBits::FragmentBit))
    ).assertValue();
    {

        pipelineBuilder.setPipelineLayout(renderGridSprites2dPipelineLayout);
        {
            auto vertexShader = device.createShaderModule("spirv/render_grid_sprites_2d.vert.spv").assertValue();
            auto fragmentShader = device.createShaderModule("spirv/render_grid_sprites_2d.frag.spv").assertValue();
            pipelineBuilder.setShaderCreateInfo(
                    vertexShader.createVertexStageInfo(),
                    fragmentShader.createFragmentStageInfo());
            renderGridSprites2dPipeline = pipelineBuilder.create().assertValue();
        }
    }

    std::vector<vul::Image> depthImages(swapchainSize);

    for (auto &depthImage: depthImages) {
        depthImage = allocator.createDeviceImage(
                vul::ImageCreateInfo::depthStencilAttachment2D(swapchain.getExtent())).assertValue();
    }
    std::vector<vul::ImageView> depthImageViews(swapchainSize);
    for (auto [i, depthImageView]: depthImageViews | ranges::views::enumerate) {
        depthImageView = depthImages[i].createImageView().assertValue();
    }


    std::vector<vul::Framebuffer> swapchainFramebuffers;
    for (const auto &[imageView, depthImageView]: ranges::views::zip(swapchainImageViews, depthImageViews)) {
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments({&imageView,
                                           &depthImageView});
        framebufferBuilder.setDimensions(swapchain.getExtent());
        framebufferBuilder.setRenderPass(presentationRenderPass);
        swapchainFramebuffers.push_back(
                framebufferBuilder.create().assertValue());
    }

    auto resizeSwapchain = [&window, &swapchainBuilder, &swapchain, &surface, &physicalDevice]() {
        auto temp_size = window.get_framebuffer_size().value();
        auto size = glm::ivec2(temp_size.width, temp_size.height);
        while (size == glm::ivec2(0)) {
            temp_size = window.get_framebuffer_size().value();
            size = glm::ivec2(temp_size.width, temp_size.height);
            glfwWaitEvents();
        }
        // swapchain = swapchainBuilder.resize(swapchain, window.getFramebufferExtent()).assertValue();
        // use surface capabilities used instead of framebuffer extent (should be the same) so that validation layers
        // get properly updated cached values for vkGetPhysicalDeviceSurfaceCapabilitiesKHR, otherwise they don't know
        // the current extent of the surface and use the old initial one.
        swapchain = swapchainBuilder.resize(swapchain,
                                            surface.getCapabilitiesFrom(physicalDevice).currentExtent).assertValue();
    };

    auto resizeSwapchainFramebuffers = [&](std::size_t swapchainImageIndex) {
        depthImages[swapchainImageIndex] = allocator.createDeviceImage(
                vul::ImageCreateInfo::depthStencilAttachment2D(swapchain.getExtent())).assertValue();
        depthImageViews[swapchainImageIndex] = depthImages[swapchainImageIndex].createImageView().assertValue();
        const auto &imageView = swapchain.getImageViews()[swapchainImageIndex];
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments({&imageView, &depthImageViews[swapchainImageIndex]});
        framebufferBuilder.setDimensions(swapchain.getExtent());
        framebufferBuilder.setRenderPass(presentationRenderPass);
        swapchainFramebuffers[swapchainImageIndex] = framebufferBuilder.create().assertValue();
    };


    gul::StbImage statue_of_david_img;
    gul::load("../../textures/texture.jpg", statue_of_david_img,
              gul::StbImage::Channels::rgb_alpha);

    gul::StbImage background_img;
    gul::load("../../textures/background.jpg", background_img,
              gul::StbImage::Channels::rgb_alpha);

    gul::StbImage block_img;
    gul::load("../../textures/block.png", block_img,
              gul::StbImage::Channels::rgb_alpha);

    gul::StbImage block_solid_img;
    gul::load("../../textures/block_solid.png", block_solid_img,
              gul::StbImage::Channels::rgb_alpha);

    gul::StbImage paddle_img;
    gul::load("../../textures/paddle.png", paddle_img,
              gul::StbImage::Channels::rgb_alpha);


    auto statue_of_david_texture_img = allocator.createDeviceTexture(commandPool,
                                                                     presentationQueue,
                                                                     vul::TempArrayProxy(
                                                              statue_of_david_img.size(),
                                                              statue_of_david_img.data()),
                                                                     vul::ImageCreateInfo::texture2D(
                                                              statue_of_david_img.getExtent2D(),
                                                              vul::Format::R8g8b8a8Srgb)).assertValue();
    auto background_texture_img = allocator.createDeviceTexture(commandPool,
                                                                presentationQueue,
                                                                vul::TempArrayProxy(
                                                                        background_img.size(),
                                                                        background_img.data()),
                                                                vul::ImageCreateInfo::texture2D(
                                                                        background_img.getExtent2D(),
                                                              vul::Format::R8g8b8a8Srgb)).assertValue();
    auto block_texture_img = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      vul::TempArrayProxy(
                                                              block_img.size(),
                                                              block_img.data()),
                                                      vul::ImageCreateInfo::texture2D(
                                                              block_img.getExtent2D(),
                                                              vul::Format::R8g8b8a8Srgb)).assertValue();
    auto block_solid_texture_img = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      vul::TempArrayProxy(
                                                              block_solid_img.size(),
                                                              block_solid_img.data()),
                                                      vul::ImageCreateInfo::texture2D(
                                                              block_solid_img.getExtent2D(),
                                                              vul::Format::R8g8b8a8Srgb)).assertValue();
    auto paddle_texture_img = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      vul::TempArrayProxy(
                                                              paddle_img.size(),
                                                              paddle_img.data()),
                                                      vul::ImageCreateInfo::texture2D(
                                                              paddle_img.getExtent2D(),
                                                              vul::Format::R8g8b8a8Srgb)).assertValue();

    auto statue_of_david_texture_img_view = statue_of_david_texture_img.createImageView().assertValue();
    auto background_texture_img_view = background_texture_img.createImageView().assertValue();
    auto block_texture_img_view = block_texture_img.createImageView().assertValue();
    auto block_solid_texture_img_view = block_solid_texture_img.createImageView().assertValue();
    auto paddle_texture_img_view = paddle_texture_img.createImageView().assertValue();

    vul::SamplerBuilder samplerBuilder(device);
    samplerBuilder.setFilter(vul::Filter::Nearest);
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::Repeat);
    samplerBuilder.enableAnisotropy();
    samplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto sampler = samplerBuilder.create().assertValue();


    std::vector<vul::Buffer> uniformBuffers;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        uniformBuffers.push_back(
                allocator.createMappedCoherentBuffer(sizeof(UniformBufferObject),
                                                     vul::BufferUsageFlagBits::UniformBufferBit).assertValue());
    }

    auto descriptorPool = device.createDescriptorPool(
            {{descriptorSetLayoutBuilder, swapchainSize}
            },
            vul::DescriptorPoolCreateFlagBits::UpdateAfterBindBit).assertValue();

    auto descriptorSets = descriptorPool.createDescriptorSets(
            {{descriptorLayout, swapchainSize}}).assertValue();

    for (const auto &[i, descriptorSet]: descriptorSets |
                                         ranges::views::enumerate) {
        vul::WriteDescriptorSetBuilder writeBuilder(descriptorSet,
                                                    descriptorSetLayoutBuilder.createDescriptorTypeCounts());
        writeBuilder[0] = {&uniformBuffers[i]};
        writeBuilder[1].set({&sampler});
        writeBuilder[2].set({&statue_of_david_texture_img_view, &background_texture_img_view, &block_texture_img_view, &block_solid_texture_img_view, &paddle_texture_img_view});
        auto write_span = writeBuilder.getWrites();
        device.updateDescriptorSets(write_span);
    }

    auto commandBuffers = commandPool.createPrimaryCommandBuffers(
            swapchainSize).assertValue();
    //single command buffer, but... what if in use? use semaphore to determine that!
    auto transferCommandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();

    gul::ImguiRenderer imguiRenderer(window, instance, device, swapchain,
                                     presentationQueue,
                                     swapchain.getFormat());
    std::vector<std::function<void(std::size_t swapchainImageIndex)>> frameResourceResizeFunctions = {{},
                                                                                                      {},
                                                                                                      {}};

    auto resizeImGuiFramebuffer = imguiRenderer.createResizeFrameCallback();
    auto frameResourceResizeFunction = [&](std::size_t swapchainImageIndex) {
        resizeImGuiFramebuffer(swapchainImageIndex);
        resizeSwapchainFramebuffers(swapchainImageIndex);
    };
    auto resizeWindow = [&resizeSwapchain, & frameResourceResizeFunctions, &frameResourceResizeFunction, &swapchain]() {
        resizeSwapchain();
        frameResourceResizeFunctions = {frameResourceResizeFunction, frameResourceResizeFunction,
                                        frameResourceResizeFunction};
    };


    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::uint64_t> frameCounters(renderFinishedSemaphores.size(), 0);

    std::uint64_t last_upload_buffer_sent_idx = 0;
    std::uint64_t frames_using_uploaded_buffer_counter = 0;

    while (!window.should_close()) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - startTime).count();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - lastTime).count();
        lastTime = currentTime;
        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1000us);
//        std::this_thread::sleep_for(32ms) ;
//        presentationQueue.waitIdle();
        glfwPollEvents();
        imguiRenderer.newFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        auto cursor_pos = window.get_cursor_pos().value();
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

            ImGui::Text(fmt::format("cursor_pos x:{},y:{}", cursor_pos.xpos.count(),
                                    cursor_pos.ypos.count()).c_str());
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

        if (frameResourceResizeFunctions[swapchainImageIndex] != nullptr) {
            frameResourceResizeFunctions[swapchainImageIndex](swapchainImageIndex);
            frameResourceResizeFunctions[swapchainImageIndex] = {};
        }
        {



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
                if (window.key_pressed(glfw::Key::W)) {
                    forward = true;
                }
                if (window.key_pressed(glfw::Key::S)) {
                    back = true;
                }
                if (window.key_pressed(glfw::Key::A)) {
                    left = true;
                }
                if (window.key_pressed(glfw::Key::D)) {
                    right = true;
                }
                if (window.key_pressed(glfw::Key::Space)) {
                    up = true;
                }
                if (window.key_pressed(glfw::Key::LeftControl)) {
                    down = true;
                }
                if (window.key_pressed(glfw::Key::_1)) {
                    toggle_spread = !toggle_spread;
                }
                if (window.key_pressed(glfw::Key::_2)) {
                    toggle_density = !toggle_density;
                }
                camera.move(forward, back, left, right, up, down, deltaTime,
                            move_speed);

                bool rotate_up = false;
                bool rotate_down = false;
                bool rotate_left = false;
                bool rotate_right = false;
                if (window.key_pressed(glfw::Key::Up)) {
                    rotate_up = true;
                }
                if (window.key_pressed(glfw::Key::Down)) {
                    rotate_down = true;
                }
                if (window.key_pressed(glfw::Key::Left)) {
                    rotate_left = true;
                }
                if (window.key_pressed(glfw::Key::Right)) {
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
            //New orthographic 2D version.
            ubo.view = glm::identity<glm::mat4x4>();
            ubo.proj = glm::ortho(-400.0f, 400.0f, 300.0f, -300.0f, -1.0f, 10.0f);


            static float u_time = 0;
            ubo.u_time = time;
            ubo.camera_pos = camera.getPosition();
            u_time += 1.0f;
            uniformBuffers[swapchainImageIndex].mappedCopyFrom(ubo);
        }
        //currentNBufferInUse
        //We retrieve the counter value for the current frames using,
        auto current_frame_counter_value = framesUsingSemaphore.getCounterValue().assertValue();
        auto current_upload_counter_value =uploadFinishedSemaphore.getCounterValue().assertValue();
        std::uint64_t n_buffer_index = (last_upload_buffer_sent_idx + 1) % n_buffer_size;
        auto last_upload_buffer_idx = current_upload_counter_value % n_buffer_size;
        bool last_upload_finished = last_upload_buffer_idx == last_upload_buffer_sent_idx;
        auto last_upload_frames_used = (current_frame_counter_value % (2*swapchainSize)) / swapchainSize;
        bool last_upload_frames_used_was_last_upload = last_upload_frames_used = last_upload_buffer_sent_idx;
        bool all_frames_using_last_upload = current_frame_counter_value % swapchainSize == 0;
        bool all_frames_in_flight_using_last_upload = last_upload_frames_used_was_last_upload && all_frames_using_last_upload;

        {
            auto affine_matrices_memory = reinterpret_cast<glm::mat3x2 *>(host_staging_affine_matrices_arrays[n_buffer_index].mapMemory());
            auto material_id_memory = reinterpret_cast<std::uint32_t *>(host_staging_material_id_arrays[n_buffer_index].mapMemory());
            auto affine_matrix = glm::scale(glm::identity<glm::mat3x3>(), glm::vec2(100.0f));
            affine_matrix = glm::rotate(affine_matrix, time);
            affine_matrices_memory[0] = static_cast<glm::mat3x2>(affine_matrix);

            static_assert(sizeof(glm::mat3x2) == 24);
            material_id_memory[0] = 0u;
        }

        {
            auto affine_matrices_memory = reinterpret_cast<glm::mat3x2 *>(host_staging_affine_matrices_arrays[n_buffer_index].mapMemory());
            auto material_id_memory = reinterpret_cast<std::uint32_t *>(host_staging_material_id_arrays[n_buffer_index].mapMemory());
            auto affine_matrix = glm::translate(glm::identity<glm::mat3x3>(), glm::vec2(glm::sin(time) *200, 0.0f));
            affine_matrix = glm::scale(affine_matrix, glm::vec2(100.0f));
            affine_matrix = glm::rotate(affine_matrix, time);
            affine_matrices_memory[1] = static_cast<glm::mat3x2>(affine_matrix);

            static_assert(sizeof(glm::mat3x2) == 24);
            material_id_memory[1] = 1u;
        }
        if(all_frames_in_flight_using_last_upload){

            {
                transferCommandBuffer.begin(
                        vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
                {
                    transferCommandBuffer.copyBuffer(host_staging_affine_matrices_arrays[n_buffer_index],
                                             device_affine_matrices_arrays[n_buffer_index]);
                    transferCommandBuffer.copyBuffer(host_staging_material_id_arrays[n_buffer_index],
                                             device_material_id_arrays[n_buffer_index]);

                    //TODO don't know if I need to specify dst access mask here if doing queue transfer?
                    auto transferToGraphicsBarriers = {
                            vul::BufferMemoryBarrier::fromBuffer(
                            device_affine_matrices_arrays[n_buffer_index],
                            vul::PipelineStageFlagBits2::TransferBit,
                            vul::AccessFlagBits2::MemoryWriteBit,
                            vul::PipelineStageFlagBits2::VertexShaderBit,
                            vul::AccessFlagBits2::MemoryReadBit,
                            transferQueue.getQueueFamilyIndex(),
                            presentationQueue.getQueueFamilyIndex()
                            ),
                            vul::BufferMemoryBarrier::fromBuffer(
                            device_material_id_arrays[n_buffer_index],
                            vul::PipelineStageFlagBits2::TransferBit,
                            vul::AccessFlagBits2::MemoryWriteBit,
                            vul::PipelineStageFlagBits2::VertexShaderBit,
                            vul::AccessFlagBits2::MemoryReadBit,
                            transferQueue.getQueueFamilyIndex(),
                            presentationQueue.getQueueFamilyIndex()
                            )
                           };
                    auto transferGraphicsDepInfo = vul::DependencyInfo({}, transferToGraphicsBarriers, {});
                    transferCommandBuffer.pipelineBarrier(transferGraphicsDepInfo);
                }
                transferCommandBuffer.end();
                last_upload_buffer_sent_idx = (last_upload_buffer_sent_idx +  1) % n_buffer_size;

            }


            std::array<VkSemaphoreSubmitInfoKHR, 1> signalInfos;
            signalInfos[0] = uploadFinishedSemaphore.createSubmitInfo(
                    frameCounters[currentFrameIndex],
                    vul::PipelineStageFlagBits2::AllCommandsBit);

            transferQueue.submit(vul::SubmitInfoBuilder()
                                             //.waitSemaphoreInfos(presentationWaitInfo)
                                             .commandBufferInfos(transferCommandBuffer.createSubmitInfo())
                                             .signalSemaphoreInfos(signalInfos)
                                             .create());

        }


        imguiRenderer.render();

        using namespace czdr::std_literals;

        auto &commandBuffer = commandBuffers[swapchainImageIndex];
        {
            commandBuffer.begin(
                    vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
            {
                commandBuffer.copyBuffer(host_staging_affine_matrices_arrays[swapchainImageIndex],
                                         device_affine_matrices_arrays[swapchainImageIndex]);
                commandBuffer.copyBuffer(host_staging_material_id_arrays[swapchainImageIndex],
                                         device_material_id_arrays[swapchainImageIndex]);
                if((current_frame_counter_value % (2*swapchainSize)) / swapchainSize != last_upload_buffer_sent_idx){
                    current_frame_counter_value += 1;
//                    TODO Only record this section when we know we need to update?
//                     How do we know if we need an update? If upload index value is different than expectation?
//                     index += 1 (until % 6)/3 = value to upload?

                        auto transferToGraphicsBarriers = {
                                vul::BufferMemoryBarrier::fromBuffer(
                                device_affine_matrices_arrays[n_buffer_index],
                                vul::PipelineStageFlagBits2::TransferBit,
                                vul::AccessFlagBits2::MemoryWriteBit,
                                vul::PipelineStageFlagBits2::VertexShaderBit,
                                vul::AccessFlagBits2::MemoryReadBit,
                                transferQueue.getQueueFamilyIndex(),
                                presentationQueue.getQueueFamilyIndex()
                                ),
                                vul::BufferMemoryBarrier::fromBuffer(
                                device_material_id_arrays[n_buffer_index],
                                vul::PipelineStageFlagBits2::TransferBit,
                                vul::AccessFlagBits2::MemoryWriteBit,
                                vul::PipelineStageFlagBits2::VertexShaderBit,
                                vul::AccessFlagBits2::MemoryReadBit,
                                transferQueue.getQueueFamilyIndex(),
                                presentationQueue.getQueueFamilyIndex()
                                )
                               };
                        auto transferGraphicsDepInfo = vul::DependencyInfo({}, transferToGraphicsBarriers, {});
                    auto transferGraphicsDepInfo = vul::DependencyInfo({transferToGraphicsBarrier}, {}, {});
                }


//
//                    VkMemoryBarrier2KHR memoryBarrier = {
//                            ...
//                            .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR,
//                            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT_KHR,
//                            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR,
//                            .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR};
//
//                    VkDependencyInfoKHR dependencyInfo = {
//                            ...
//                            1,                 // memoryBarrierCount
//                            &memoryBarrier,    // pMemoryBarriers
//                            ...
//                    }
//
//                    vkCmdPipelineBarrier2KHR(commandBuffer, &dependencyInfo);

                auto transferToGraphicsBarrier = {vul::MemoryBarrier{
                        nullptr,
                        vul::PipelineStageFlagBits2::TransferBit,
                        vul::AccessFlagBits2::MemoryWriteBit,
                        vul::PipelineStageFlagBits2::VertexShaderBit,
                        vul::AccessFlagBits2::MemoryReadBit,
                }};
                auto transferGraphicsDepInfo = vul::DependencyInfo({transferToGraphicsBarrier}, {}, {});


                commandBuffer.pipelineBarrier(transferGraphicsDepInfo);
                auto extent = swapchain.getExtent();
                commandBuffer.setViewport(vul::Viewport(extent).get());
                commandBuffer.setScissor(vul::Rect2D(extent).get());

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {{1.0f, 0.5f, 1.0f, 1.0f}};
                clearValues[0].color = {
                        {clear_color.x, clear_color.y, clear_color.z,
                         clear_color.w}};
                clearValues[1].depthStencil = {1.0f, 0};
                {
                    auto renderPassBlock = commandBuffer.beginRenderPass(
                            firstColorPass,
                            swapchainFramebuffers[swapchainImageIndex],
                            VkRect2D{{0, 0}, extent},
                            clearValues);
                    commandBuffer.bindPipeline(renderFullscreenTexturePipeline);
                    commandBuffer.bindDescriptorSets(
                            vul::PipelineBindPoint::Graphics, renderFullscreenTexturePipelineLayout,
                            descriptorSets[swapchainImageIndex]);
                    auto current_swapchain_resource_index = static_cast<std::uint32_t>(
                            frame_counter %
                            swapchainSize);
                    RenderFullscreenPushConstantBlock pushConstant = {
                            1u
                    };

                    commandBuffer.pushConstants(renderFullscreenTexturePipelineLayout,
                                                vul::ShaderStageFlagBits::VertexBit |
                                                vul::ShaderStageFlagBits::FragmentBit,
                                                pushConstant);
                    auto vertex_count = 3;
                    renderPassBlock.draw(vertex_count);
                }
                auto renderPassToRenderPassBarriers = {
                        vul::ImageMemoryBarrier::depthStencilAttachmentReuse(depthImages[swapchainImageIndex]),
                        vul::ImageMemoryBarrier::colorAttachmentReuse(swapchain.getImages()[swapchainImageIndex],
                                                                      vul::ImageSubresourceRange(vul::getImageAspect(swapchain.getFormat())))
                };
                auto renderPassToRenderPassDepInfo = vul::DependencyInfo({}, {}, renderPassToRenderPassBarriers);
                commandBuffer.pipelineBarrier(transferGraphicsDepInfo);
                {
                    auto renderPassBlock = commandBuffer.beginRenderPass(
                            presentationRenderPass,
                            swapchainFramebuffers[swapchainImageIndex],
                            VkRect2D{{0, 0}, extent},
                            {});
                    commandBuffer.bindPipeline(renderSprites2dPipeline);
                    commandBuffer.bindDescriptorSets(
                            vul::PipelineBindPoint::Graphics, renderSprites2dPipelineLayout,
                            descriptorSets[swapchainImageIndex]);
                    std::uint32_t box_vertex_count = 36;
                    std::uint32_t vertex_split_pass_count = 5;

                    auto current_swapchain_resource_index = static_cast<std::uint32_t>(
                            frame_counter %
                            swapchainSize);
                    RenderSprites2dPushConstantBlock pushConstant = {
                            device_affine_matrices_arrays[current_swapchain_resource_index].getDeviceAddress(),
                            device_material_id_arrays[current_swapchain_resource_index].getDeviceAddress(),
                    };

                    commandBuffer.pushConstants(renderSprites2dPipelineLayout,
                                                vul::ShaderStageFlagBits::VertexBit |
                                                vul::ShaderStageFlagBits::FragmentBit,
                                                pushConstant);
                    auto vertex_count = 12;
                    renderPassBlock.draw(vertex_count);
                }
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


        presentationQueue.submit(vul::SubmitInfoBuilder()
                                         .waitSemaphoreInfos(presentationWaitInfo)
                                         .commandBufferInfos(commandBuffer.createSubmitInfo())
                                         .signalSemaphoreInfos(signalInfos)
                                         .create());
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