//
// Created by Shae Bolt on 6/5/2021.
//

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
    glm::vec3 camera_pos;
    float u_time;
};
static_assert(sizeof(UniformBufferObject) == 16*4*3+16);

const std::vector<Vertex> vertices = {
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

    auto commandPool = device.createCommandPool(
            presentQueueIndex,
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();

    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setBindings({vul::UniformBufferBinding(0,
                                                                      vul::ShaderStageFlagBits::VertexBit | vul::ShaderStageFlagBits::FragmentBit).get(),
                                            vul::CombinedSamplerBinding(1,

                                                                        vul::ShaderStageFlagBits::FragmentBit).get()});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    std::vector<std::uint32_t> chunk_data(32*32*32, 1);


    std::vector<std::uint32_t> set_offsets = {1,5,2,24,32*32-32, 32, 32*32 - 32, 32*32, 32*3 + 4, 28+17, 32*8,32*32};
    std::vector<std::uint32_t> set_values = {4,5,7,6,2,3,2,0,2,4,1,0};

    std::size_t total_offset =0;
    for(auto [offset, value] : ranges::views::zip(set_offsets, set_values)){
        for(std::size_t i =total_offset; i < total_offset + offset; ++i){
            chunk_data[i] = value;
        }
        total_offset += offset;
    }

    gul::bitmask bitmask(32*32*32);
    for(auto [idx, value] : ranges::views::enumerate(chunk_data)){
        if(value != 0){
            bitmask.set(idx);
        }
    }

    auto bitmask_intersect = [&u_bitmask = bitmask](glm::vec3 orig, glm::vec3 dir, glm::vec3 block_offset, std::uint32_t& voxel_index){
        orig = orig.xzy();
        dir = dir.xzy();
        orig.z *= -1.0;
        dir.z *= -1.0;
        glm::vec3 invDir = 1.0f / dir;
//    bvec3 sign = bvec3(dir.x < 0, dir.y < 0, dir.z < 0);
        glm::vec3 cellDimension = glm::vec3(1,1,1);
        glm::uvec3 resolution = glm::uvec3(32u,32u,32u);
        float tHitBox = 0.0f;
        // initialization step
        glm::ivec3 exit, step, cell;
        glm::vec3 deltaT, nextCrossingT;
        for (uint8_t i_8 = uint8_t(0); i_8 < uint8_t(3); ++i_8) {
            std::uint32_t i = std::uint32_t(i_8);
            // convert ray starting point to cell coordinates
            //bbox origin should be 0,0,0 now?

            float rayOrigCell = ((orig[i] + dir[i] * tHitBox) - block_offset[i]);
            cell[i] = int(glm::clamp(int(floor(rayOrigCell / cellDimension[i])), 0, int(resolution[i] - 1))); //should I even clamp?
            cell[i] = int(floor(rayOrigCell / cellDimension[i]));
            if (dir[i] < 0) {
                deltaT[i] = -cellDimension[i] * invDir[i];
                nextCrossingT[i] = tHitBox + (float(cell[i]) * cellDimension[i] - rayOrigCell) * invDir[i];
                if(i == 234){
                    exit[i] = int(resolution[i]);
                    step[i] = 1;
                }else{
                    exit[i] = -1;
                    step[i] = -1;
                }

            }
            else {
                deltaT[i] = cellDimension[i] * invDir[i];
                nextCrossingT[i] = tHitBox + ((float(cell[i]) + 1)  * cellDimension[i] - rayOrigCell) * invDir[i];
//            exit[i] = int(resolution[i]);
//            step[i] = 1;
                if(i == 234){
                    exit[i] = -1;
                    step[i] = -1;
                }else{
                    exit[i] = int(resolution[i]);
                    step[i] = 1;
                }

            }
        }

        // Walk through each cell of the grid and test for an intersection if
        // current cell contains geometry
        while (true) {
            if(cell.x >= 32 || cell.y >= 32 || cell.z >= 32 || cell.x < 0 || cell.y < 0 || cell.z < 0){
                return false;
            }

            uint32_t o = cell.z * resolution.x * resolution.y + cell.y * resolution.x + cell.x;
            if (u_bitmask.get(o)) {
                voxel_index = o;
                return true;
            }
            uint8_t k =
                    (uint8_t(nextCrossingT[0] < nextCrossingT[1]) << uint8_t(2)) + //y > x
                    (uint8_t(nextCrossingT[0] < nextCrossingT[2]) << uint8_t(1)) + //z > x
                    (uint8_t(nextCrossingT[1] < nextCrossingT[2])); //z > y
            const uint8_t map[8] = {uint8_t(2),
                                    uint8_t(1),
                                    uint8_t(2),
                                    uint8_t(1),
                                    uint8_t(2),
                                    uint8_t(2),
                                    uint8_t(0),
                                    uint8_t(0)};
//        const uint8_t map[8] = {uint8_t(0),
//                                uint8_t(0),
//                                uint8_t(2),
//                                uint8_t(2),
//                                uint8_t(1),
//                                uint8_t(2),
//                                uint8_t(1),
//                                uint8_t(2)};
            uint8_t axis = map[std::uint32_t(k)];

            //not needed because if we "get" a value at the position, we gaurantee a hit.
//        if (tHit < nextCrossingT[axis]) break;
            cell[std::uint32_t(axis)] += step[std::uint32_t(axis)];
            if (cell[std::uint32_t(axis)] == exit[std::uint32_t(axis)]){
                break;
            }
            nextCrossingT[std::uint32_t(axis)] += deltaT[std::uint32_t(axis)];
        }
        return false;
    };
    auto org = glm::vec3(3.25f,0.0f,3.25f);
    auto dir = normalize(glm::vec3(0.0f,2,-1));
    auto org_offset = glm::vec3(0.0f,1.0f,0.0f) * 0.001f;
    auto block_offset = glm::vec3(0.0f);
    std::uint32_t voxel_index;
    auto intersected = bitmask_intersect(org + org_offset, dir, block_offset,voxel_index);
    fmt::print("intersectred {}\n", intersected);
    auto calc_rle = [](const std::vector<std::uint32_t> & chunk){
        std::vector<std::uint16_t> offsets;
        std::vector<std::uint32_t> materials;
        std::uint16_t running_size = 0;
        std::uint32_t running_material = chunk.empty() ? 0 : chunk[0];
        for(const auto& voxel : chunk){
            if(running_material != voxel){
                offsets.push_back(running_size);
                materials.push_back(running_material);
                running_material = voxel;
            }
            running_size += 1;
        }
        VUL_ASSERT(materials.empty() || materials.back() != running_material , "Last value shouldn't have actually been set...");
        if(!chunk.empty()){
            VUL_ASSERT(running_size == chunk.size());
            offsets.push_back(running_size);
            materials.push_back(running_material);
        }
        return std::tuple{materials, offsets};
    };

    auto[chunk_rle_materials, chunk_rle_offsets] = calc_rle(chunk_data);

    auto rle_material_buffer = allocator.createDeviceBuffer(commandPool,
                                                     presentationQueue,
                                                     vul::TempArrayProxy(
                                                             chunk_rle_materials.size(),
                                                             chunk_rle_materials.data()),
                                                     vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto rle_offset_buffer = allocator.createDeviceBuffer(commandPool,
                                                           presentationQueue,
                                                           vul::TempArrayProxy(
                                                                   chunk_rle_offsets.size(),
                                                                   chunk_rle_offsets.data()),
                                                           vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto rle_bitmask_buffer = allocator.createDeviceBuffer(commandPool,
                                                           presentationQueue,
                                                           vul::TempArrayProxy(
                                                                   bitmask.size(),
                                                                   bitmask.data()),
                                                           vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();



    vul::GraphicsPipeline graphicsPipeline;
    struct alignas(8) RunLengthEncodingPushConstant{
        std::uint32_t size;
        std::uint32_t padding;
        std::uint64_t materials;
        std::uint64_t offsets;
        std::uint64_t bitmask;
    };
    static_assert(sizeof(RunLengthEncodingPushConstant) == 32);
    RunLengthEncodingPushConstant rlePushConstant = {
            static_cast<std::uint32_t>(chunk_rle_offsets.size()),
            0u,
            rle_material_buffer.getDeviceAddress(),
            rle_offset_buffer.getDeviceAddress(),
            rle_bitmask_buffer.getDeviceAddress()
    };


    auto pipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            VkPushConstantRange{
                (vul::ShaderStageFlagBits::VertexBit | vul::ShaderStageFlagBits::FragmentBit).to_underlying(),
                0,
                sizeof(RunLengthEncodingPushConstant)
        }).assertValue();
//    auto pipelineLayout = device.createPipelineLayout(
//            descriptorLayout).assertValue();
    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);


//    pipelineBuilder.setVertexBinding<Vertex>(0);
    pipelineBuilder.setPrimitiveStateInfo(
            vul::PrimitiveTopology::TriangleList);
    pipelineBuilder.setViewportStateFromExtent(
            surface.getSwapchain()->getExtent());
    pipelineBuilder.setDynamicState(
            {vul::DynamicState::Viewport, vul::DynamicState::Scissor});
    pipelineBuilder.setDefaultRasterizationState();
//    pipelineBuilder.setDefaultRasterizationState(vul::CullModeFlagBits::None);

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
                vul::readSPIRV("spirv/rle_render.vert.spv")).assertValue();
        auto fragmentShader = device.createShaderModule(
                vul::readSPIRV("spirv/rle_render.frag.spv")).assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());
        graphicsPipeline = pipelineBuilder.create().assertValue();
    }



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
    gul::load("../../textures/texture.jpg", pixels,
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
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::MirroredRepeat);
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



    while (!window.shouldClose()) {
        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1000us);
//        std::this_thread::sleep_for(32ms) ;
//        presentationQueue.waitIdle();
        glfwPollEvents();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        auto cursor_pos =  window.getCursorPosition();
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

            ImGui::Text(fmt::format("cursor_pos x:{},y:{}", cursor_pos.x, cursor_pos.y).c_str());
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


        {
            fmt::print("Value {},{},{}\n", camera.getPosition().x,camera.getPosition().y,camera.getPosition().z);
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
            ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),
                        glm::vec3(1.0f, 0.0f, 0.0f));
//            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
//                                    glm::vec3(0.0f, 1.0f, 0.0f));
//            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
//                                   glm::vec3(0.0f, 0.0f, 0.0f),
//                                   glm::vec3(0.0f, 1.0f, 0.0f));
            ubo.view = camera.calcViewMatrix();
            ubo.proj = glm::perspective(glm::radians(45.0f),
                                        surface.getSwapchain()->getExtent().width /
                                        (float) surface.getSwapchain()->getExtent().height,
                                        0.1f,
                                        1000.0f);

            ubo.proj[1][1] *= -1;
            static float u_time = 0;
            ubo.u_time = time;
            ubo.camera_pos = camera.getPosition();
            u_time += 1.0f;
            uniformBuffers[swapchainImageIndex].copyToMapped<UniformBufferObject>(
                    ubo);

        }



        ImGui::Render();




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
//                commandBuffer.bindVertexBuffers(vertexBuffer, 0ull);
//                commandBuffer.bindIndexBuffer(indexBuffer,
//                                              vul::IndexType::Uint16);
                commandBuffer.bindDescriptorSets(
                        vul::PipelineBindPoint::Graphics, pipelineLayout,
                        descriptorSets[swapchainImageIndex]);
                std::uint32_t box_vertex_count = 36;
                std::uint32_t vertex_split_pass_count = 5;
                commandBuffer.pushConstants(pipelineLayout,
                                            vul::ShaderStageFlagBits::VertexBit | vul::ShaderStageFlagBits::FragmentBit,
                                            rlePushConstant);
                auto vertex_count = chunk_rle_materials.size() * box_vertex_count * vertex_split_pass_count;
                renderPassBlock.draw(vertex_count);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                                commandBuffer.get());
            }
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