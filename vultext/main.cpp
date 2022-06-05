//
// Created by Shae Bolt on 6/5/2021.
//

#include <gul/imguirenderer.h>
#include <gul/firstpersoncamera.h>
#include <gul/stbimage.h>
#include <gul/glfwwindow.h>
#include "bvh.h"
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
#include <range/v3/view/drop.hpp>
#include <range/v3/view/zip.hpp>
#include <gsl/span>
#include <vul/temparrayproxy.h>
#include <optional>
#include <chrono>
#include <thread>
#include <random>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <misc/cpp/imgui_stdlib.h>

#include <msdfgen.h>
#include <msdfgen-ext.h>


//std::vector<std::string> texture_file_paths = {
//        "../../resources/brick_goodvibes.png",
//        "../../resources/clay_goodvibes.png",
//        "../../resources/cobblestone_goodvibes.png",
//        "../../resources/dirt_goodvibes.png",
//        "../../resources/end_stone_goodvibes.png",
//        "../../resources/grass_block_top_goodvibes.png",
//        "../../resources/oak_planks_goodvibes.png",
//        "../../resources/sand_goodvibes.png"
//};
//std::vector<gul::StbImage> textureLayers;
//for (const auto &file_path: texture_file_paths) {
//gul::StbImage block_texture;
//gul::load(file_path, block_texture, gul::StbImage::Channels::rgb_alpha);
//textureLayers.push_back(block_texture);
//}
//
//std::vector<vul::TempConstVoidArrayProxy> textureLayerSpans;
//for (const auto &textureLayer: textureLayers) {
//textureLayerSpans.push_back(
//        vul::TempArrayProxy(textureLayer.size(), textureLayer.data()));
//}
//
//auto temp = vul::TempArrayProxy(textureLayerSpans);
//
//std::cout << vul::is_container<decltype(textureLayerSpans)>::value << std::endl;
//
//std::cout << vul::is_container<decltype(temp)>::value << std::endl;
//
//auto arrayTextureImage = allocator.createDeviceTexture(commandPool,
//                                                       presentationQueue,
//                                                       vul::TempArrayProxy(textureLayerSpans),
//                                                       vul::createSimple2DImageInfo(
//                                                               vul::Format::R8g8b8a8Srgb,
//                                                               VkExtent3D{16,16,1},
//                                                               vul::ImageUsageFlagBits::TransferDstBit |
//                                                               vul::ImageUsageFlagBits::SampledBit,
//                                                               1,textureLayers.size())).assertValue();


vul::Image generateSDFTextures(vul::VmaAllocator& allocator, vul::CommandPool& commandPool, vul::Queue& presentationQueue, std::int32_t glyph_dim = 32){

    std::vector<std::vector<std::uint8_t>> textureLayerRaw;
    std::vector<vul::TempConstVoidArrayProxy> textureLayerSpans;

//    for (const auto &textureLayer: textureLayers) { );
    msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
    if (ft) {
        msdfgen::FontHandle *font = msdfgen::loadFont(ft, "../../resources/fonts/open-sans/OpenSans-Regular.ttf");
        if (font) {
            msdfgen::Shape shape;

            constexpr auto channel_count = 4;
            msdfgen::Bitmap<float, channel_count> msdf(glyph_dim, glyph_dim);
            gsl::span<float> bitmap_span(static_cast<float*>(msdf), msdf.width()*msdf.height()*channel_count);

            for(auto unicode_value : ranges::views::ints(32, 126)){
                if (msdfgen::loadGlyph(shape, font, unicode_value)) {
                    std::vector<std::uint8_t> msdf8bit(glyph_dim*glyph_dim*channel_count);
                    shape.normalize();
                    //                      max. angle
                    msdfgen::edgeColoringSimple(shape, 3.0);
                    //           image width, height

                    //                     range, scale, translation
                    msdfgen::generateMTSDF(msdf, shape, msdfgen::Projection(1.0, msdfgen::Vector2(0.0,0.0)), 1.0);
                    //const, but not really... very frustrating...
                    msdfgen::simulate8bit(msdf);
                    for(auto [msdf8bit_value, msdf_value] : ranges::views::zip(msdf8bit, bitmap_span)){
                        msdf8bit_value = static_cast<std::uint8_t>(msdf_value);
                    }
                    textureLayerRaw.push_back(msdf8bit);
                    textureLayerSpans.push_back(vul::TempArrayProxy(textureLayerRaw.back()));
                }
                else {
                    std::vector<std::uint8_t> msdf8bit(glyph_dim*glyph_dim*channel_count, 0);
                    textureLayerRaw.push_back(msdf8bit);
                    textureLayerSpans.push_back(vul::TempArrayProxy(textureLayerRaw.back()));
                }
            }

            msdfgen::destroyFont(font);
        }
        msdfgen::deinitializeFreetype(ft);
    }
    auto arrayTextureImage = allocator.createDeviceTexture(commandPool,
                                                       presentationQueue,
                                                       vul::TempArrayProxy(textureLayerSpans),
                                                       vul::createSimple2DImageInfo(
                                                               vul::Format::R8g8b8a8Srgb,
                                                               VkExtent2D{static_cast<std::uint32_t>(glyph_dim),
                                                                          static_cast<std::uint32_t>(glyph_dim)},
                                                               vul::ImageUsageFlagBits::TransferDstBit |
                                                               vul::ImageUsageFlagBits::SampledBit,
                                                               1,textureLayerRaw.size())).assertValue();
    return arrayTextureImage;
}


//    layout(push_constant) uniform PushConstantBlock {
//            f32vec2_ref u_offsets; //8 bytes
//            f32vec2_ref u_scale;  //16 bytes
//            uint32_ref u_material_ids; // 24;
//            uint64_t u_color_state;
//            mat4x4 u_mvp; // 64 + 24 bytes;
//    };
//




struct SDFPushConstantBlock{
    std::uint64_t u_offsets;
    std::uint64_t u_scale;
    std::uint64_t u_material_ids;
    std::uint64_t u_color_state;
    glm::mat4x4 u_mvp;
};
static_assert(sizeof(SDFPushConstantBlock) == (8+8+8+8 + 64));

struct SDFGPUBuffers{
    vul::Buffer u_offsets;
    vul::Buffer u_scale;
    vul::Buffer u_material_ids;
};

SDFGPUBuffers string_to_sdf_gpu_buffers(vul::VmaAllocator& allocator,
                                        vul::CommandPool& commandPool,
                                        vul::Queue& presentationQueue,
                                        float line_offset, glm::vec2 scale, const std::string_view& text){
    constexpr std::uint32_t first_char_value = 32;
    std::vector<glm::vec2> host_offsets;
    std::vector<glm::vec2> host_scale;
    std::vector<std::uint32_t> host_material_ids;
    host_offsets.reserve(text.size());
    host_scale.reserve(text.size());
    host_material_ids.reserve(text.size());

    auto total_offset = 0.0f;
    for(std::size_t i = 0; i < text.size(); ++i){
        host_offsets.emplace_back(total_offset + scale*0.1f, line_offset);
        host_scale.emplace_back(scale, scale);
        host_material_ids.emplace_back(static_cast<std::uint32_t>(text[i]) - first_char_value);
    }
    SDFGPUBuffers sdf_gpu_buffers;
    sdf_gpu_buffers.u_offsets = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_offsets,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    sdf_gpu_buffers.u_scale = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_scale,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    sdf_gpu_buffers.u_material_ids = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_material_ids,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    return std::move(sdf_gpu_buffers);
}

//see https://github.com/KhronosGroup/Vulkan-Samples/tree/master/samples/extensions
//see https://www.khronos.org/blog/vulkan-timeline-semaphores
//see https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/extensions/VK_KHR_synchronization2.md
//see https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples


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


int main() {
    gul::GlfwWindow window(800, 600, "ExampleWindow");
    gul::FirstPersonCamera camera;
//    camera.setPosition(glm::vec3(0.0, 0.0, 0.0));
    camera.setPosition(glm::vec3(13.0,2.0,-3.0));
    //camera.lookAt(glm::vec3(0.0,0.0,0.0));
//    camera.setRotation(glm::vec3(0, 0, 0.0));
    camera.lookAt(glm::vec3(0.0,0.0,0.0));

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
    features.physicalDeviceVulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingPartiallyBound = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingVariableDescriptorCount = VK_TRUE;

//    features.physicalDeviceVulkan12Features.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
    features.physicalDeviceVulkan12Features.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;

//    features.physicalDeviceVulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
//    features.physicalDeviceVulkan12Features.descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;

    features.physicalDeviceVulkan12Features.runtimeDescriptorArray = VK_TRUE;

    features.physicalDeviceVulkan12Features.scalarBlockLayout = VK_TRUE;
    features.physicalDeviceVulkan12Features.timelineSemaphore = VK_TRUE;
    features.physicalDeviceVulkan12Features.bufferDeviceAddress = VK_TRUE;
    features.physicalDeviceVulkan12Features.bufferDeviceAddressCaptureReplay = VK_TRUE;

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
//    const vul::SurfaceFormat defaultSurfaceFormat = {vul::Format::B8g8r8a8Srgb,
//                                                     vul::ColorSpaceKHR::SrgbNonlinear};
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
    renderPass.setObjectName("MyActualRenderPass");

    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setFlags(
            vul::DescriptorSetLayoutCreateFlagBits::UpdateAfterBindPoolBit);
    descriptorSetLayoutBuilder.setBindings(
            {vul::CombinedSamplerBinding(0,
                                         vul::ShaderStageFlagBits::FragmentBit).get()
            },
            {vul::DescriptorBindingFlagBits::UpdateAfterBindBit});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    vul::GraphicsPipeline graphicsPipeline;


    auto pipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            VkPushConstantRange{
                    (vul::ShaderStageFlagBits::FragmentBit | vul::ShaderStageFlagBits::VertexBit).to_underlying(),
                    0,
                    sizeof(SDFPushConstantBlock)
            }).assertValue();
    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);


//    pipelineBuilder.setVertexBinding<Vertex>(0);

    pipelineBuilder.setPrimitiveStateInfo(
            vul::PrimitiveTopology::TriangleList);
    pipelineBuilder.setViewportStateFromExtent(
            surface.getSwapchain()->getExtent());
    pipelineBuilder.setDynamicState(
            {vul::DynamicState::Viewport, vul::DynamicState::Scissor});
    pipelineBuilder.setDefaultRasterizationState(
            vul::CullModeFlagBits::FrontBit); //TODO turn triangle around...? why did they make it front culling...
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
                vul::readSPIRV("spirv/gen_quad_2d.vert.spv")).assertValue();
        auto fragmentShader = device.createShaderModule(
                vul::readSPIRV("spirv/sdf_quad_render.frag.spv")).assertValue();
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
                    surface.getSwapchain()->getExtent(),
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
                        surface.getSwapchain()->getExtent(),
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




    auto arrayTextureImage = generateSDFTextures(allocator, commandPool, presentationQueue, 32);

    auto arrayTextureView = arrayTextureImage.createImageView(vul::ImageSubresourceRange(
            vul::ImageAspectFlagBits::ColorBit, 0, 1, 0, arrayTextureImage.getArrayLayerCount())).assertValue();


    vul::SamplerBuilder samplerBuilder(device);
    samplerBuilder.setFilter(vul::Filter::Linear);
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::Repeat);
    samplerBuilder.enableAnisotropy();
    samplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto sampler = samplerBuilder.create().assertValue();


    struct alignas(32) SDFColorState {
        glm::vec3 bg_color;
        glm::vec3 fg_color;
        glm::vec2 unit_range;
    };
    static_assert(sizeof(SDFColorState) == 32);
    SDFColorState host_color_state = {};
    host_color_state.bg_color = glm::vec3(0.0f);
    host_color_state.fg_color = glm::vec3(1.0f);
    host_color_state.unit_range = 255.0f / glm::vec2(32.0f);
    auto gpu_color_state = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_color_state,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

//    layout(push_constant) uniform PushConstantBlock {
//            f32vec2_ref u_offsets; //8 bytes
//            f32vec2_ref u_scale;  //16 bytes
//            uint32_ref u_material_ids; // 24;
//            uint64_t u_color_state;
//            mat4x4 u_mvp; // 64 + 24 bytes;
//    };
//



    auto [u_offsets, u_scale, u_material_ids] = string_to_sdf_gpu_buffers(allocator, commandPool, presentationQueue, 0.1f, glm::vec2(0.1f), "Hello!");





    auto commandBuffers = commandPool.createPrimaryCommandBuffers(
            swapchainSize).assertValue();

    gul::ImguiRenderer imguiRenderer(window, instance, device, surface,
                                     presentQueueIndex, presentationQueue,
                                     surface.getSwapchain()->getFormat());




    auto descriptorPool = device.createDescriptorPool(
            {{descriptorSetLayoutBuilder,      swapchainSize},
            vul::DescriptorPoolCreateFlagBits::UpdateAfterBindBit).assertValue();

    auto descriptorSets = descriptorPool.createDescriptorSets(
            {{descriptorLayout, swapchainSize}}).assertValue();


    auto resizeWindow = [&resizeSwapchain, resizeImGuiFramebuffers = imguiRenderer.createResizeCallback(
            presentationQueue)]() {
        resizeSwapchain();
        resizeImGuiFramebuffers();
    };

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::uint64_t> frameCounters(renderFinishedSemaphores.size(),
                                             0);

//    computeBuilder.set
    auto start = std::chrono::high_resolution_clock::now();
    while (!window.shouldClose()) {

        glfwPollEvents();
        imguiRenderer.newFrame();
        auto windowExtent = surface.getSwapchain()->getExtent();
        bool view_state_updated = false;
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

            ImGui::Text(fmt::format("camera pos : x:{},y:{},z:{}",
                                    camera.getPosition().x,
                                    camera.getPosition().y,
                                    camera.getPosition().z).c_str());
            ImGui::Text(fmt::format("camera rot : r:{},p:{},y:{}",
                                    glm::degrees(camera.getRotation().x),
                                    glm::degrees(camera.getRotation().y),
                                    glm::degrees(
                                            camera.getRotation().z)).c_str());


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
                float move_speed = 3;

                bool forward = false;
                bool back = false;
                bool left = false;
                bool right = false;
                bool up = false;
                bool down = false;
                bool shift = false;
                if (window.keyPressed(GLFW_KEY_LEFT_SHIFT)) {
                    shift = true;
                }
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
                if(shift){
                    move_speed *= 10.0;
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
            ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
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
                                        10.0f);
            ubo.proj[1][1] *= -1;
            uniformBuffers[swapchainImageIndex].mappedCopyFrom(
                    ubo);

            if(inflight_view_state_update_versions[swapchainImageIndex] != view_state_update_version){
                inflight_view_state_update_versions[swapchainImageIndex] = view_state_update_version;
                device_view_states[swapchainImageIndex].mappedCopyFrom(view_state);
            }

        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> ds = finish - start;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(ds);
        double time = elapsed.count() /(1000.0);
        RayTracingPushConstant rayTracingPushConstant = {};
        rayTracingPushConstant.u_view_state = device_view_states[swapchainImageIndex].getDeviceAddress();
        rayTracingPushConstant.u_camera_origin = camera.getPosition();
        rayTracingPushConstant.u_camera_rotation = camera.getRotation();
        rayTracingPushConstant.u_frame_idx = static_cast<std::uint32_t>(swapchainImageIndex); //TODO swapchain_size;
        rayTracingPushConstant.u_time = static_cast<float>(time);
        rayTracingPushConstant.u_distance_ids = device_distance_field_ids.getDeviceAddress();
        rayTracingPushConstant.u_point_array = device_points.getDeviceAddress();
        imguiRenderer.render();
        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1000us);
//        std::this_thread::sleep_for(16ms);

        auto &commandBuffer = commandBuffers[swapchainImageIndex];
        {
            commandBuffer.begin(
                    vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
            {
                commandBuffer.bindPipeline(raytracePipeline);
                commandBuffer.bindDescriptorSets(
                        vul::PipelineBindPoint::Compute, raytracePipelineLayout,
                        raytraceDescriptorSet);
                auto computeComputeBarrier = vul::createMemoryBarrier(
                        vul::PipelineStageFlagBits2KHR::ComputeShaderBit |
                        vul::PipelineStageFlagBits2KHR::FragmentShaderBit,
                        vul::AccessFlagBits2KHR::ShaderWriteBit |
                        vul::AccessFlagBits2KHR::ShaderReadBit,
                        vul::PipelineStageFlagBits2KHR::ComputeShaderBit,
                        vul::AccessFlagBits2KHR::ShaderWriteBit |
                        vul::AccessFlagBits2KHR::ShaderReadBit);
                auto computeComputeDepInfo = vul::createDependencyInfo(
                        computeComputeBarrier, {}, {});
                {
                    commandBuffer.pipelineBarrier(computeComputeDepInfo);
                    commandBuffer.pushConstants(raytracePipelineLayout,
                                                vul::ShaderStageFlagBits::ComputeBit,
                                                rayTracingPushConstant);
                    commandBuffer.dispatch(
                            static_cast<std::uint32_t>(std::ceil(
                                    (windowExtent.width * windowExtent.height) /
                                    1024.0)));

                }
                auto computeGraphicsBarrier = vul::createMemoryBarrier(
                        vul::PipelineStageFlagBits2KHR::ComputeShaderBit,
                        vul::AccessFlagBits2KHR::ShaderWriteBit,
                        vul::PipelineStageFlagBits2KHR::FragmentShaderBit,
                        vul::AccessFlagBits2KHR::ShaderReadBit);
                auto dependencyInfo = vul::createDependencyInfo(
                        computeGraphicsBarrier, {}, {});
                commandBuffer.pipelineBarrier(dependencyInfo);
            }
            {
                auto extent = surface.getSwapchain()->getExtent();
                commandBuffer.setViewport(vul::Viewport(extent));
                commandBuffer.setScissor(vul::Rect2D(extent));


                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {{1.0f, 0.5f, 1.0f, 1.0f}};
                clearValues[0].color = {
                        {clear_color.x, clear_color.y, clear_color.z,
                         clear_color.w}};
                clearValues[1].depthStencil = {1.0f, 0};
                auto renderPassBlock = commandBuffer.beginRenderPass(
                        renderPass,
                        swapchainFramebuffers[swapchainImageIndex],
                        vul::Rect2D(extent),
                        clearValues);
                commandBuffer.bindPipeline(graphicsPipeline);

                commandBuffer.bindDescriptorSets(
                        vul::PipelineBindPoint::Graphics, pipelineLayout,
                        descriptorSets[swapchainImageIndex]);
                commandBuffer.pushConstants(pipelineLayout,
                                            vul::ShaderStageFlagBits::FragmentBit,
                                            static_cast<std::uint32_t>(
                                                    frame_counter %
                                                    swapchainSize));
                renderPassBlock.draw(3);
            }
            imguiRenderer.recordCommands(commandBuffer, swapchainImageIndex);
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

        imguiRenderer.postSubmit();
        auto presentResult = surface.getSwapchain()->present(presentationQueue,
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