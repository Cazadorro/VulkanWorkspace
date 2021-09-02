//
// Created by Shae Bolt on 6/5/2021.
//

#include <gul/stbimage.h>
#include <gul/glfwwindow.h>
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
#include <range/v3/view/iota.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/range/conversion.hpp>
#include <gsl/span>
#include <vul/temparrayproxy.h>
#include <optional>


//see https://github.com/KhronosGroup/Vulkan-Samples/tree/master/samples/extensions
//see https://www.khronos.org/blog/vulkan-timeline-semaphores
//see https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/extensions/VK_KHR_synchronization2.md
//see https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples

//X createInstance();
//X setupDebugMessenger();
//X createSurface();
//X pickPhysicalDevice();
//X createLogicalDevice();
//X createSwapChain();

//createImageViews();

//X createRenderPass();
//X createDescriptorSetLayout();
//X createGraphicsPipeline();
//X createCommandPool();

//createDepthResources();

//X createFramebuffers();

//createTextureImage();
//createTextureImageView();

//X createTextureSampler();
//X createVertexBuffer();
//X createIndexBuffer();
//X createUniformBuffers();
//X createDescriptorPool();
//X createDescriptorSets();
//X createCommandBuffers();
//X createSyncObjects();
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
}

        std::optional<vul::PhysicalDevice>

pickPhysicalDevice(const vul::Instance &instance, const vul::Surface &surface,
                   const vul::Features &features,
                   const gsl::span<const char *const> &deviceExtensions,
                   const vul::SurfaceFormat &surfaceFormat,
                   vul::PresentModeKHR presentationMode) {
    for (const auto &physicalDevice : instance.enumeratePhysicalDevices()) {
        auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        auto graphicsComputeFamily = vul::QueueFlagBits::GraphicsBit |
                                     vul::QueueFlagBits::ComputeBit;
        auto computeTransferFamily = vul::QueueFlagBits::ComputeBit |
                                     vul::QueueFlagBits::TransferBit;
        auto supportsQueueFamilies = queueFamilyProperties.contains(
                {graphicsComputeFamily, computeTransferFamily})
                                     && queueFamilyProperties.contains(surface,
                                                                       vul::QueueFlagBits::GraphicsBit);

        auto surfacePresentModes = surface.getPresentModesFrom(physicalDevice);
        auto surfaceFormats = surface.getFormatsFrom(physicalDevice);
        auto surfaceSupported =
                surfacePresentModes.isSupported(presentationMode)
                && surfaceFormats.isSupported(surfaceFormat);
        auto featuresSupported = vul::matches(physicalDevice.getFeatures(),
                                              features);
        auto extensionsSupported = physicalDevice.getExtensionProperties().supports(
                deviceExtensions);
        if (supportsQueueFamilies && surfaceSupported && featuresSupported &&
            extensionsSupported) {
            return physicalDevice;
        }
    }
    return std::nullopt;
}

int main() {
    gul::GlfwWindow window(800, 600, "ExampleWindow");
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
    appInfo.apiVersion = VK_API_VERSION_1_2;
    vul::Instance instance = vul::Instance::withDebugCallback(
            appInfo, vul::Instance::validationLayers,
            instanceExtensions).assertValue();

    vul::DebugUtilsMessengerEXT debugMessenger =
            vul::DebugCreateInfoBuilder(instance)
                    .setDefaults()
                    .create()
                    .assertValue();
    vul::Features features;
    features.getPhysicalDeviceFeatures().samplerAnisotropy = VK_TRUE;
    features.getPhysicalDeviceFeatures().robustBufferAccess = VK_TRUE;
    features.getPhysicalDeviceFeatures().shaderFloat64 = VK_TRUE;
    features.getPhysicalDeviceFeatures().shaderInt64 = VK_TRUE;
    features.getPhysicalDeviceFeatures().shaderInt16 = VK_TRUE;

    features.physicalDeviceVulkan11Features.storageBuffer16BitAccess = VK_TRUE;
    features.physicalDeviceVulkan11Features.uniformAndStorageBuffer16BitAccess = VK_TRUE;
    features.physicalDeviceVulkan11Features.storagePushConstant16 = VK_TRUE;
    //features.physicalDeviceVulkan11Features.storageInputOutput16 = VK_TRUE; in/out for shaders, not very important.

    features.physicalDeviceVulkan12Features.storageBuffer8BitAccess = VK_TRUE;
    features.physicalDeviceVulkan12Features.uniformAndStorageBuffer8BitAccess = VK_TRUE;
    features.physicalDeviceVulkan12Features.storagePushConstant8 = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderBufferInt64Atomics = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderSharedInt64Atomics = VK_TRUE;
    //features.physicalDeviceVulkan12Features.shaderFloat16 = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderInt8 = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.scalarBlockLayout = VK_TRUE;
    features.physicalDeviceVulkan12Features.timelineSemaphore = VK_TRUE;
    features.physicalDeviceVulkan12Features.bufferDeviceAddress = VK_TRUE;

    features.physicalDeviceShaderAtomicFloatFeaturesEXT.shaderBufferFloat32AtomicAdd = VK_TRUE;
    features.physicalDeviceShaderAtomicFloatFeaturesEXT.shaderBufferFloat32Atomics = VK_TRUE;
    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
    };
    vul::Device device; //TODO fix issue where swapchain is both a member of surface, but must be deleted before device, need to move swapchain back out?
    vul::Surface surface = window.createSurface(instance);
    //TODO will need to capitalize letters?
    const vul::SurfaceFormat defaultSurfaceFormat = {vul::Format::B8g8r8a8Srgb,
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
                                 vul::QueueFlagBits::ComputeBit;
    auto computeTransferFamily = vul::QueueFlagBits::ComputeBit |
                                 vul::QueueFlagBits::TransferBit;

    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto graphicsQueueFamilyIndexes = queueFamilyProperties.getQueueFamilyIndexes(
            graphicsComputeFamily);
    auto computeQueueFamilyIndexes = queueFamilyProperties.getQueueFamilyIndexes(
            computeTransferFamily);
    auto presentationQueueFamilyIndexes = queueFamilyProperties.getPresentationQueueFamilyIndexes(
            surface, graphicsComputeFamily);
    auto presentFamilyIndexResult = queueFamilyProperties.getMinimumQueueFamilyIndex(
            presentationQueueFamilyIndexes);
    if (!presentFamilyIndexResult.has_value()) {
        throw std::runtime_error(
                "failed to find a suitable presentation queue!");
    }
    auto graphicsFamilyIndexResult = queueFamilyProperties.getMinimumQueueFamilyIndex(
            graphicsQueueFamilyIndexes);
    if (!graphicsFamilyIndexResult.has_value()) {
        throw std::runtime_error("failed to find a suitable graphics queue!");
    }
    auto computeFamilyIndexResult = queueFamilyProperties.getMinimumQueueFamilyIndex(
            computeQueueFamilyIndexes);
    if (!computeFamilyIndexResult.has_value()) {
        throw std::runtime_error("failed to find a suitable compute queue!");
    }
    std::uint32_t presentQueueIndex = presentFamilyIndexResult.value();
    std::uint32_t graphicsQueueIndex = graphicsFamilyIndexResult.value();
    std::uint32_t computeQueueIndex = computeFamilyIndexResult.value();
    std::vector<vul::SingleQueueCreateInfo> queueCreateInfos = {
            vul::SingleQueueCreateInfo{presentQueueIndex},
            vul::SingleQueueCreateInfo{computeQueueIndex}};
    device = physicalDevice.createDevice(queueCreateInfos, deviceExtensions,
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

    surface.createSwapchain(swapchainBuilder);
    auto allocator = vul::VmaAllocator::create(instance, physicalDevice,
                                                  device).assertValue();

    const std::int32_t maxFramesInFlight = 2;
    std::uint64_t currentFrameIndex = 0;
    std::vector<vul::BinarySemaphore> binaryRenderFinishedSemaphores;
    std::vector<vul::BinarySemaphore> presentationFinishedSemaphores;
    std::vector<vul::TimelineSemaphore> renderFinishedSemaphores;
    for (auto i : ranges::views::ints(0, maxFramesInFlight)) {
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
    auto resizeSwapchain = [&window, &presentationQueue, &surface, &swapchainBuilder]() {
        auto size = window.getFramebufferSize();
        while (size == glm::ivec2(0)) {
            size = window.getFramebufferSize();
            glfwWaitEvents();
        }
        //TODO will need to recreate associated assets (like framebuffers or images that match the size of the window).
        //TODO need to add framebuffers to assets possibly managed by swapchain?
        presentationQueue.waitIdle();
        surface.resizeSwapchain(swapchainBuilder,
                                window.getFramebufferExtent());
    };

    auto subpassBuilder = vul::SubpassGraph(
            {vul::AttachmentDescription::PresentTemp(
                    surface.getSwapchain()->getFormat()),
             vul::AttachmentDescription::DepthTemp(
                     vul::Format::D24UnormS8Uint)}, 1);
    subpassBuilder.subpassAt(0).setWrites({0, 1})
            .setPreDependExternal(
                    vul::PipelineStageFlagBits::ColorAttachmentOutputBit,
                    vul::PipelineStageFlagBits::ColorAttachmentOutputBit,
                    {},
                    vul::AccessFlagBits::ColorAttachmentWriteBit |
                    vul::AccessFlagBits::DepthStencilAttachmentWriteBit);
    auto renderPass = subpassBuilder.createRenderPass(device).assertValue();

    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setBindings({vul::UniformBufferBinding(0,
                                                                      vul::ShaderStageFlagBits::VertexBit).get(),
                                            vul::CombinedSamplerBinding(1,
                                                                        vul::ShaderStageFlagBits::FragmentBit).get()});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    vul::GraphicsPipeline graphicsPipeline;
    {
        auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);
        auto vertexShader = device.createShaderModule(
                vul::readSPIRV("spirv/shader_depth.vert.spv")).assertValue();
        auto fragmentShader = device.createShaderModule(
                vul::readSPIRV("spirv/shader_depth.frag.spv")).assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());
        pipelineBuilder.setVertexBinding<Vertex>(0);
        pipelineBuilder.setPrimitiveStateInfo(
                vul::PrimitiveTopology::TriangleList);
        pipelineBuilder.setViewportStateFromExtent(
                surface.getSwapchain()->getExtent());
        pipelineBuilder.setDefaultRasterizationState();
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

        auto pipelineLayout = device.createPipelineLayout(descriptorLayout).assertValue();
        pipelineBuilder.setRenderPass(renderPass, 0);
        graphicsPipeline = pipelineBuilder.create().assertValue();
    }

    auto commandPool = device.createCommandPool(graphicsQueueIndex).assertValue();

    vul::Image depthImage = allocator.createDeviceImage(
            vul::createSimple2DImageInfo(
                    vul::Format::D24UnormS8Uint,
                    surface.getSwapchain()->getExtent3D(),
                    vul::ImageUsageFlagBits::DepthStencilAttachmentBit)
                    ).assertValue();

    auto depthImageView = depthImage.createImageView().assertValue();


    std::vector<vul::Framebuffer> swapchainFramebuffers;
    const auto& swapchainImageViews = surface.getSwapchain()->getImageViews();

    for (const auto& imageView : swapchainImageViews) {
        std::array<const vul::ImageView*,2> imageViews = {&imageView, &depthImageView};
        swapchainFramebuffers.push_back(
                device.createFramebuffer(renderPass,
                                         imageViews,
                                         surface.getSwapchain()->getExtent()).assertValue());
    }

    gul::StbImage pixels;
    gul::load("../../textures/texture.jpg",pixels,
              gul::StbImage::Channels::rgb_alpha);
    auto textureImage = allocator.createDeviceTexture(commandPool, presentationQueue,
            vul::TempArrayProxy(pixels.getDeviceSize(), pixels.getData()),
            vul::createSimple2DImageInfo(
                    vul::Format::R8g8b8a8Srgb, pixels.getExtent3D(),
                    vul::ImageUsageFlagBits::TransferDstBit | vul::ImageUsageFlagBits::SampledBit)).assertValue();

    auto textureImageView = textureImage.createImageView();


    while (!window.shouldClose()) {

        glfwPollEvents();
        //TODO do actual timeline semaphore update here. use framecounter, not sure how, will need to make sure value is <= actual signal value.
        renderFinishedSemaphores[currentFrameIndex].wait(frame_counter);
        auto &presentationFinishedSemaphore = presentationFinishedSemaphores[currentFrameIndex];
        auto &binaryRenderFinishedSemaphore = binaryRenderFinishedSemaphores[currentFrameIndex];
        auto swapchainImageIndexResult = surface.getSwapchain()->acquireNextImage(
                presentationFinishedSemaphore);

        if (swapchainImageIndexResult.result ==
            vul::Result::ErrorOutOfDateKhr) {
            resizeSwapchain();
            continue;
        }
        auto swapchainImageIndex = swapchainImageIndexResult.assertValue(
                validSwapchainResults);
        //TODO do actual render here

        auto presentResult = surface.getSwapchain()->present(presentationQueue,
                                                             binaryRenderFinishedSemaphore,
                                                             swapchainImageIndex);

        if (presentResult == vul::Result::ErrorOutOfDateKhr ||
            presentResult == vul::Result::SuboptimalKhr || framebufferResized) {
            framebufferResized = false;
            resizeSwapchain();
        } else if (presentResult != vul::Result::Success) {
            throw std::runtime_error("failed to present swapchain image");
        }

        currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight;
        frame_counter += 1;
    }
    surface.getSwapchain();
    //TODO need to remove spec invalidation by removing overlapping feature stuff that exists in both VkPhysicalDeviceVulkan11/2Features and other features
    return 0;
}