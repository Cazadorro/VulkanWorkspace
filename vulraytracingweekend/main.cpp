//
// Created by Shae Bolt on 6/5/2021.
//

#include <gul/firstpersoncamera.h>
#include <gul/stbimage.h>
#include <gul/glfwwindow.h>
#include "bitmask.h"
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
#include <gsl/span>
#include <vul/temparrayproxy.h>
#include <optional>
#include <chrono>
#include <thread>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>


//see https://github.com/KhronosGroup/Vulkan-Samples/tree/master/samples/extensions
//see https://www.khronos.org/blog/vulkan-timeline-semaphores
//see https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/extensions/VK_KHR_synchronization2.md
//see https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples


vul::ExpectedResult<vul::RenderPass>
createImGuiRenderPass(const vul::Device &device, vul::Format surfaceFormat,
                      bool clearEnable) {
    VkAttachmentDescription attachment = {};
    attachment.format = vul::get(surfaceFormat);
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = clearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                    : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
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

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct alignas(16) RayTracingParameters{
    glm::vec3 offset;
    std::uint32_t width;
    glm::vec3 roll_pitch_yaw;
    std::uint32_t height;
    float fov;
    std::uint32_t image_count;
    glm::vec2 m_padding;
};

const std::vector<Vertex> vertices = {
        {{-0.5f, 0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  0.0f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  0.0f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        //not needed below
        ,

        {{-0.5f, 0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  0.0f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  0.0f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        ,

        {{-0.5f, 0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  0.0f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  0.0f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        ,
        {{-0.5f, 0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  0.0f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  0.0f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        ,
        {{-0.5f, 0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  0.0f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  0.0f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};


const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
};

std::optional<vul::PhysicalDevice>
pickPhysicalDevice(const vul::Instance &instance, const vul::Surface &surface,
                   const vul::Features &features,
                   const gsl::span<const char *const> &deviceExtensions,
                   const vul::SurfaceFormat &surfaceFormat,
                   vul::PresentModeKHR presentationMode) {
    for (const auto &physicalDevice: instance.enumeratePhysicalDevices()) {
        //TODO handle more than just discrete
        if (physicalDevice.getType() != vul::PhysicalDeviceType::DiscreteGpu) {
            continue;
        }
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
    gul::FirstPersonCamera camera;
    camera.setPosition(glm::vec3(0.0, 0.0, -1.0));
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
    //all the descriptor indexing stuff we need
    features.physicalDeviceVulkan12Features.descriptorIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderStorageImageArrayNonUniformIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.shaderSampledImageArrayNonUniformIndexing  = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingPartiallyBound = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingVariableDescriptorCount = VK_TRUE;
    features.physicalDeviceVulkan12Features.runtimeDescriptorArray = VK_TRUE;

    features.physicalDeviceVulkan12Features.scalarBlockLayout = VK_TRUE;
    features.physicalDeviceVulkan12Features.timelineSemaphore = VK_TRUE;
    features.physicalDeviceVulkan12Features.bufferDeviceAddress = VK_TRUE;

    features.physicalDeviceShaderAtomicFloatFeaturesEXT.shaderBufferFloat32AtomicAdd = VK_TRUE;
    features.physicalDeviceShaderAtomicFloatFeaturesEXT.shaderBufferFloat32Atomics = VK_TRUE;

    features.physicalDeviceSynchronization2FeaturesKHR.synchronization2 = VK_TRUE;
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
                                 vul::QueueFlagBits::ComputeBit |
                                 vul::QueueFlagBits::TransferBit;
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
            .imageUsage(vul::ImageUsageFlagBits::ColorAttachmentBit | vul::ImageUsageFlagBits::StorageBit)
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
                                               device/*,
                                               VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT*/).assertValue();

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
                    surface.getSwapchain()->getFormat()),
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

    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setBindings({vul::UniformBufferBinding(0,
                                                                      vul::ShaderStageFlagBits::VertexBit).get(),
                                            vul::CombinedSamplerBinding(1,

                                                                        vul::ShaderStageFlagBits::FragmentBit).get()});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    vul::GraphicsPipeline graphicsPipeline;
    auto pipelineLayout = device.createPipelineLayout(
            descriptorLayout).assertValue();
    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);


    pipelineBuilder.setVertexBinding<Vertex>(0);
    pipelineBuilder.setPrimitiveStateInfo(
            vul::PrimitiveTopology::TriangleList);
    pipelineBuilder.setViewportStateFromExtent(
            surface.getSwapchain()->getExtent());
    pipelineBuilder.setDynamicState(
            {vul::DynamicState::Viewport, vul::DynamicState::Scissor});
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
                vul::readSPIRV("spirv/shader_depth.vert.spv")).assertValue();
        auto fragmentShader = device.createShaderModule(
                vul::readSPIRV("spirv/shader_depth.frag.spv")).assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());
        graphicsPipeline = pipelineBuilder.create().assertValue();
    }

    auto commandPool = device.createCommandPool(
            presentQueueIndex,
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();

    vul::Image depthImage = allocator.createDeviceImage(
            vul::createSimple2DImageInfo(
                    vul::Format::D24UnormS8Uint,
                    surface.getSwapchain()->getExtent3D(),
                    vul::ImageUsageFlagBits::DepthStencilAttachmentBit)
    ).assertValue();

    auto depthImageView = depthImage.createImageView(vul::ImageSubresourceRange(
            vul::ImageAspectFlagBits::DepthBit)).assertValue();


    std::vector<vul::Framebuffer> swapchainFramebuffers;
    const auto &swapchainImageViews = surface.getSwapchain()->getImageViews();
    auto swapchainSize = static_cast<std::uint32_t>(swapchainImageViews.size());

    for (const auto &imageView: swapchainImageViews) {
        std::array<const vul::ImageView *, 2> imageViews = {&imageView,
                                                            &depthImageView};
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments(imageViews);
        framebufferBuilder.setDimensions(surface.getSwapchain()->getExtent());
        framebufferBuilder.setRenderPass(renderPass);
        swapchainFramebuffers.push_back(
                framebufferBuilder.create().assertValue());
    }

    auto resizeSwapchain = [&window, &presentationQueue, &surface, &swapchainBuilder,
            &pipelineBuilder, &graphicsPipeline, &depthImage, &depthImageView,
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

//        surface.resizeSwapchain(swapchainBuilder,
//                                window.getFramebufferExtent());
        swapchainBuilder.imageExtent(window.getFramebufferExtent());
        surface.createSwapchain(swapchainBuilder);

        depthImage = allocator.createDeviceImage(
                vul::createSimple2DImageInfo(
                        vul::Format::D24UnormS8Uint,
                        surface.getSwapchain()->getExtent3D(),
                        vul::ImageUsageFlagBits::DepthStencilAttachmentBit)
        ).assertValue();

        depthImageView = depthImage.createImageView(vul::ImageSubresourceRange(
                vul::ImageAspectFlagBits::DepthBit)).assertValue();
        const auto &swapchainImageViews = surface.getSwapchain()->getImageViews();
        for (const auto &imageView: swapchainImageViews) {
            std::array<const vul::ImageView *, 2> imageViews = {&imageView,
                                                                &depthImageView};
            vul::FramebufferBuilder framebufferBuilder(device);
            framebufferBuilder.setAttachments(imageViews);
            framebufferBuilder.setDimensions(
                    surface.getSwapchain()->getExtent());
            framebufferBuilder.setRenderPass(renderPass);
            swapchainFramebuffers.push_back(
                    framebufferBuilder.create().assertValue());
        }
    };


    gul::StbImage pixels;
//    gul::load("../../textures/texture.jpg", pixels,
//              gul::StbImage::Channels::rgb_alpha);
    gul::load("../../textures/texture2.png", pixels,
              gul::StbImage::Channels::rgb_alpha);
    auto textureImage = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      vul::TempArrayProxy(
                                                              pixels.size(),
                                                              pixels.data()),
                                                      vul::createSimple2DImageInfo(
                                                              vul::Format::R8g8b8a8Srgb,
                                                              pixels.getExtent3D(),
                                                              vul::ImageUsageFlagBits::TransferDstBit |
                                                              vul::ImageUsageFlagBits::SampledBit)).assertValue();

    auto textureImageView = textureImage.createImageView(
            vul::ImageSubresourceRange(
                    vul::ImageAspectFlagBits::ColorBit)).assertValue();

    vul::SamplerBuilder samplerBuilder(device);
    samplerBuilder.setFilter(vul::Filter::Linear);
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::Repeat);
    samplerBuilder.enableAnisotropy();
    samplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto sampler = samplerBuilder.create().assertValue();

    //TODO enable non const vector/array to convert to TempArrayProxy automatically.
    auto vertexBuffer = allocator.createDeviceBuffer(
            commandPool, presentationQueue,
            vul::TempArrayProxy(vertices.size(), vertices.data()),
            vul::BufferUsageFlagBits::TransferDstBit |
            vul::BufferUsageFlagBits::VertexBufferBit).assertValue();

    auto indexBuffer = allocator.createDeviceBuffer(
            commandPool, presentationQueue,
            vul::TempArrayProxy(indices.size(), indices.data()),
            vul::BufferUsageFlagBits::TransferDstBit |
            vul::BufferUsageFlagBits::IndexBufferBit).assertValue();

    std::vector<vul::Buffer> uniformBuffers;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        uniformBuffers.push_back(
                allocator.createMappedCoherentBuffer(bufferSize,
                                                     vul::BufferUsageFlagBits::UniformBufferBit).assertValue());
    }

    auto descriptorPool = device.createDescriptorPool(
            {{descriptorSetLayoutBuilder,
              swapchainSize}}).assertValue();

    auto descriptorSets = descriptorPool.createDescriptorSets(
            {{descriptorLayout, swapchainSize}}).assertValue();

    for (const auto&[i, descriptorSet]: descriptorSets |
                                        ranges::views::enumerate) {
        auto updateBuilder = descriptorSetLayoutBuilder.createUpdateBuilder();
        updateBuilder.getDescriptorElementAt(0).setUniformBuffer(
                {uniformBuffers[i].createDescriptorInfo()});
        //TODO potentially could keep as layout general?
        updateBuilder.getDescriptorElementAt(1).setCombinedImageSampler(
                {textureImageView.createDescriptorInfo(sampler,
                                                       vul::ImageLayout::ShaderReadOnlyOptimal)});
        auto updates = updateBuilder.create(descriptorSet);
        device.updateDescriptorSets(updates);
    }

    auto commandBuffers = commandPool.createPrimaryCommandBuffers(
            swapchainSize).assertValue();


    std::vector<VkDescriptorPoolSize> pool_sizes =
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

    auto imguiPool = device.createDescriptorPool(
            pool_sizes, 1000,
            vul::DescriptorPoolCreateFlagBits::FreeDescriptorSetBit).assertValue();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
//    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
//    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.getWindowPtr(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance.get();
    init_info.PhysicalDevice = physicalDevice.get();
    init_info.Device = device.get();
    init_info.QueueFamily = presentQueueIndex;
    init_info.Queue = presentationQueue.get();
//    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = imguiPool.get();
//    init_info.Allocator = g_Allocator;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
//    init_info.CheckVkResultFn = check_vk_result;

    ImGui_ImplVulkan_Init(&init_info, renderPass.get());
#if 0
    auto imguiRenderPass = createImGuiRenderPass(device, surface.getSwapchain()->getFormat(), false).assertValue();
    ImGui_ImplVulkan_Init(&init_info, imguiRenderPass.get());

    std::vector<vul::Framebuffer> imguiFramebuffers;
//    const auto &swapchainImageViews = surface.getSwapchain()->getImageViews();
//    auto swapchainSize = static_cast<std::uint32_t>(swapchainImageViews.size());

    for (const auto &imageView: swapchainImageViews) {
        std::array<const vul::ImageView *, 1> imageViews = {&imageView};
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments(imageViews);
        framebufferBuilder.setDimensions(surface.getSwapchain()->getExtent());
        framebufferBuilder.setRenderPass(renderPass);
        swapchainFramebuffers.push_back(
                framebufferBuilder.create().assertValue());
    }
#endif

    renderPass.setObjectName("MyActualRenderPass");

    commandPool.singleTimeSubmit(presentationQueue,
                                 [](vul::CommandBuffer &commandBuffer) {
                                     ImGui_ImplVulkan_CreateFontsTexture(
                                             commandBuffer.get());
                                 });
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::uint64_t> frameCounters(renderFinishedSemaphores.size(),
                                             0);

    auto raytracingLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    raytracingLayoutBuilder.setBindings(
            {vul::UniformBufferBinding(0,
                                       vul::ShaderStageFlagBits::ComputeBit).get(),
             vul::StorageImageBinding(1,
                                      vul::ShaderStageFlagBits::ComputeBit,
                                      swapchainSize).get()});

    auto raytracingLayout = raytracingLayoutBuilder.create().assertValue();
    auto raytracingPipelineLayout = device.createPipelineLayout(raytracingLayout).assertValue();
    auto computeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline raytracingPipeline;
    {
        auto computeShader = device.createShaderModule(
                vul::readSPIRV("spirv/raytrace.comp.spv")).assertValue();
        computeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        computeBuilder.setPipelineLayout(raytracingPipelineLayout);
        raytracingPipeline = computeBuilder.create().assertValue();
    }




//    RayTracingParameters raytracingParameters = {};
//    {
//        auto extent = surface.getSwapchain()->getExtent();
//        raytracingParameters.fov = glm::radians(90.0f);
//        raytracingParameters.offset = camera.getPosition();
//        raytracingParameters.roll_pitch_yaw = camera.getRotation();
//        raytracingParameters.width = extent.width;
//        raytracingParameters.height = extent.height;
//    }

    std::vector<vul::Buffer> raytracingParameterBuffers;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        VkDeviceSize bufferSize = sizeof(RayTracingParameters);
        uniformBuffers.push_back(
                allocator.createMappedCoherentBuffer(bufferSize,
                                                     vul::BufferUsageFlagBits::UniformBufferBit).assertValue());
    }
//
//    auto raytracingParameterBuffer = allocator.createDeviceBuffer(
//            commandPool, presentationQueue, raytracingParameters,
//            vul::BufferUsageFlagBits::UniformBufferBit).assertValue();


    auto lbmDescriptorPool = device.createDescriptorPool(
            {{raytracingLayoutBuilder, 1}}).assertValue();

    auto lbmDescriptorSet = lbmDescriptorPool.createDescriptorSet(
            raytracingLayout).assertValue();

    {
        device.setObjectName(lbmDescriptorSet, "raytracingDescriptorSet");
        using namespace ranges;
        auto updateBuilder = raytracingLayoutBuilder.createUpdateBuilder();

        updateBuilder.getDescriptorElementAt(0).setUniformBuffer(
                {raytracingParameterBuffers[0].createDescriptorInfo()});
        updateBuilder.getDescriptorElementAt(1).setStorageImage(
                swapchainImageViews | views::transform(
                        [](auto &value) { return value.createStorageWriteInfo(); }) |
                ranges::to<std::vector>());
        auto updates = updateBuilder.create(lbmDescriptorSet);
        device.updateDescriptorSets(updates);
    }

    while (!window.shouldClose()) {

        glfwPollEvents();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

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
        auto swapchainImageIndexResult = surface.getSwapchain()->acquireNextImage(
                presentationFinishedSemaphore);

        if (swapchainImageIndexResult.result ==
            vul::Result::ErrorOutOfDateKhr) {
            resizeSwapchain();
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
                float move_speed = 3;

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


            auto extent = surface.getSwapchain()->getExtent();
            UniformBufferObject ubo = {};
            ubo.model = glm::identity<glm::mat4x4>();
            ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                        glm::vec3(1.0f, 0.0f, 0.0f));
//            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
//                                    glm::vec3(0.0f, 1.0f, 0.0f));
//            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
//                                   glm::vec3(0.0f, 0.0f, 0.0f),
//                                   glm::vec3(0.0f, 1.0f, 0.0f));
            ubo.view = camera.calcViewMatrix();
            ubo.proj = glm::perspective(glm::radians(45.0f),
                                        extent.width /
                                        (float) extent.height,
                                        0.1f,
                                        10.0f);
            ubo.proj[1][1] *= -1;
            uniformBuffers[swapchainImageIndex].mappedCopyFrom(
                    ubo);
            RayTracingParameters raytracingParameters = {};
            raytracingParameters.image_count = swapchainSize;
            raytracingParameters.fov = glm::radians(90.0f);
            raytracingParameters.offset = camera.getPosition();
            raytracingParameters.roll_pitch_yaw = camera.getRotation();
            raytracingParameters.width = extent.width;
            raytracingParameters.height = extent.height;

        }


        ImGui::Render();
        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1000us);
//        std::this_thread::sleep_for(16ms);

        auto &commandBuffer = commandBuffers[swapchainImageIndex];
        {
            commandBuffer.begin(
                    vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
            {
                auto extent = surface.getSwapchain()->getExtent();
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
                commandBuffer.bindVertexBuffers(vertexBuffer, 0ull);
                commandBuffer.bindIndexBuffer(indexBuffer,
                                              vul::IndexType::Uint16);
                commandBuffer.bindDescriptorSets(
                        vul::PipelineBindPoint::Graphics, pipelineLayout,
                        descriptorSets[swapchainImageIndex]);
                renderPassBlock.drawIndexed(indices.size());

//                renderPassBlock.draw(36);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                                commandBuffer.get());
            }
#if 0
            {
                std::array<VkClearValue, 1> clearValues{};
//                clearValues[0].color = {{1.0f, 0.5f, 1.0f, 1.0f}};
                clearValues[0].color = {
                        {clear_color.x, clear_color.y, clear_color.z,
                         clear_color.w}};
                auto extent = surface.getSwapchain()->getExtent();
                auto renderPassBlock = commandBuffer.beginRenderPass(
                        imguiRenderPass,
                        imguiFramebuffers[swapchainImageIndex],
                        VkRect2D{{0, 0}, extent},
                        clearValues);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                                commandBuffer.get());
            }
#endif
            commandBuffer.end();
        }

        frameCounters[currentFrameIndex] += 1;
        auto presentationWaitInfo = presentationFinishedSemaphore.createSubmitInfo(
                vul::PipelineStageFlagBits2KHR::ColorAttachmentOutputBit);
        std::array<VkSemaphoreSubmitInfoKHR, 2> signalInfos;
        signalInfos[0] = renderFinishedSemaphores[currentFrameIndex].createSubmitInfo(
                frameCounters[currentFrameIndex],
                vul::PipelineStageFlagBits2KHR::AllCommandsBit);
        signalInfos[1] = binaryRenderFinishedSemaphore.createSubmitInfo(
                vul::PipelineStageFlagBits2KHR::AllCommandsBit);
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

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

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
    device.waitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}