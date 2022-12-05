//
// Created by Shae Bolt on 6/5/2021.
//

#include <gul/imguirenderer.h>
#include <gul/noise/fbm.h>
#include <uul/bit.h>
#include <uul/math.h>
#include <gul/noise/opensimplex.h>
//#include "chunkmanagement.h"
#include "cpu_bitmask_intersect.h"
#include "chunks.h"
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

struct alignas(8) RunLengthEncodingPushConstant {
    std::uint64_t u_material_data_block_ptr;
    std::uint64_t u_cumulaive_block_offsets;
    std::uint64_t u_bitmasks_ref;
    std::uint64_t u_voxel_sdf_out;
    std::uint32_t u_cumulative_block_offsets_size;
};
static_assert(sizeof(RunLengthEncodingPushConstant) == 40);

struct alignas(8) JFAInitPushConstant{
    std::uint32_t u_block_idx;
    std::uint32_t u_padding;
    std::uint64_t u_bitmask_ref;
    std::uint64_t u_voxel_jfa_out;
};
static_assert(sizeof(JFAInitPushConstant) == 24);

struct alignas(8) JFAIterationPushConstant{
    std::uint32_t u_jump_step_size;
    std::uint32_t u_jump_step_type;
    std::uint64_t u_voxel_jfa_in;
    std::uint64_t u_voxel_jfa_out;
    std::uint64_t u_voxel_sdf_out;
};
static_assert(sizeof(JFAIterationPushConstant) == 32);

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

    std::uint32_t presentQueueIndex =queueFamilyProperties.calcMinimumPresentationQueueFamilyIndex(surface, graphicsComputeFamily).value();
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
    descriptorSetLayoutBuilder.setBindings({vul::UniformBufferBinding(0,
                                                                      vul::ShaderStageFlagBits::VertexBit |
                                                                      vul::ShaderStageFlagBits::FragmentBit).get(),
                                            vul::CombinedSamplerBinding(1,

                                                                        vul::ShaderStageFlagBits::FragmentBit).get()});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    std::vector<std::uint32_t> chunk_data(32 * 32 * 32, 1);


    std::vector<std::uint32_t> set_offsets = {1, 5, 2, 24, 32 * 32 - 32, 32,
                                              32 * 32 - 32, 32 * 32, 32 * 3 + 4,
                                              28 + 17, 32 * 8, 32 * 32};
    std::vector<std::uint32_t> set_values = {4, 5, 7, 6, 2, 3, 2, 0, 2, 4, 1,
                                             0};

    std::size_t total_offset = 0;
    for (auto[offset, value]: ranges::views::zip(set_offsets, set_values)) {
        for (std::size_t i = total_offset; i < total_offset + offset; ++i) {
            chunk_data[i] = value;
        }
        total_offset += offset;
    }
    bool clear = false;
    vul::ChunkSpan chunk_span(vul::make_chunk_span(chunk_data));
    for(std::size_t y = 0; y < 32; ++y){
        for(std::size_t z = 0; z < 32; ++z){
            for(std::size_t x = 0; x < 32; ++x){
                if(clear){
                    chunk_span(x,y,z) = 0;
                    continue;
                }
                auto d_x = static_cast<double>(x);
                auto d_y = static_cast<double>(y);
                auto d_z = static_cast<double>(z);
                auto height = gul::fbm2d(d_x, d_z, &gul::Noise2, 2, 0.5f, 1.0/16.0, 2.0, 32.0);
                std::uint32_t chunk_material = 0;
                if((31 - d_y) < height){

                    auto solid_value = gul::fbm3d(d_x, d_y, d_z, &gul::Noise3_Fallback, 1, 1.0f, 8.0, 2.0, 1.0);
                    if(solid_value > 0.0){
                        chunk_material = 8;
                    }else{
                        chunk_material = 8;
                    }
                }
                chunk_span(x,y,z) = chunk_material;
            }
        }
    }

    chunk_span(0,0,0) = 8;

//    chunk_span(2,0,0) = 8;
//    chunk_span(0,2,0) = 8;
    chunk_span(0,0,2) = 8;
//    chunk_span(2,2,0) = 8;
//    chunk_span(0,2,2) = 8;
//    chunk_span(2,2,2) = 8;

    auto bitmask = vul::ChunkBitmask::from_filled(chunk_span);

    auto bitmask2 = vul::ChunkBitmaskByteRLE::from_filled(chunk_span);
    auto bitmask3 = vul::ChunkBitmaskBitRLE::from_filled(chunk_span);
    auto bitmask4 = vul::ChunkBitmaskLayerTableBitRLE::from_filled(chunk_span);
    //counter per chunk,
    //index to which chunk you're refering to.
    //chunks rendering (what about large memory chunks?, maybe just allocate large memory chunks, put other stuff at the end).
    //check count of all referenced from array of reference counts and values.
    //u32 sets of cumsum
    //[0,4,5,10],[refptr, refptr, refptr...]
    //copy refptr to refptr when updated
    //data for bitfield contained inside of RLE, so possibly remove need for bitmask, and generate bitfield on fly? would stop bit fields from being edited in other ways though...


    auto org = glm::vec3(6.5f, -3.0f, 6.5f);
    auto dir = normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    auto org_offset = glm::vec3(0.0f, 1.0f, 0.0f) * 0.001f;
    auto block_offset = glm::vec3(0.0f);
    std::uint32_t voxel_index;
    //TODO get to work with interesected.
//    auto intersected = bitmask_intersect(bitmask, org + org_offset, dir,
//                                         block_offset, voxel_index);
//
//    fmt::print("intersectred {}\n", intersected);
//    std::cout << std::endl;



    auto chunk_rle = vul::ChunkRLE::from_linear(std::span<std::uint32_t,vul::chunk_consts::chunk_size>(chunk_data));
    std::vector<std::uint32_t> cumulative_rle_sizes = {chunk_rle.size()};
    auto device_cumulative_rle_sizes =  allocator.createDeviceBuffer(commandPool,
                                                                     presentationQueue,
                                                                     cumulative_rle_sizes,
                                                                     vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
//    allocator.createDeviceBuffer(
//            commandPool,
//            presentationQueue,
//            kdtree.get_aabb(),
//            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    auto rle_bitmask_buffer = allocator.createDeviceBuffer(commandPool,
                                                           presentationQueue,
                                                           bitmask,
                                                           vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();





    auto rle_staging_materials = allocator.createStagingBuffer(vul::TempArrayProxy(chunk_rle.get_material_ids())).assertValue();
    auto rle_staging_offsets = allocator.createStagingBuffer(vul::TempArrayProxy(chunk_rle.get_offsets())).assertValue();
    auto data_chunk_block_size_bytes = 1024*1024*12;
    auto data_chunk_block_rle_offset_begin = 1024*1024*8;



    auto rle_data_buffer = allocator.createDeviceBuffer(data_chunk_block_size_bytes,
                                                        vul::BufferUsageFlagBits::ShaderDeviceAddressBit
                                                        | vul::BufferUsageFlagBits::TransferDstBit).assertValue();


    vul::copy(rle_staging_materials, rle_data_buffer, commandPool, presentationQueue, 0);
    vul::copy(rle_staging_offsets, rle_data_buffer, commandPool, presentationQueue, data_chunk_block_rle_offset_begin);

    auto rle_materials_address = rle_data_buffer.getDeviceAddress();
    auto rle_offsets_address = rle_materials_address + data_chunk_block_rle_offset_begin;





    std::array voxel_jfa_buffers = {
            allocator.createDeviceBuffer(sizeof(std::uint16_t) * vul::chunk_consts::chunk_size, vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue(),
            allocator.createDeviceBuffer(sizeof(std::uint16_t) * vul::chunk_consts::chunk_size, vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue()
    };

    auto voxel_df_buffer = allocator.createDeviceBuffer(sizeof(std::uint8_t) * vul::chunk_consts::chunk_size, vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    JFAInitPushConstant jfa_init_push_constant = {
            0,
            0,
            rle_bitmask_buffer.getDeviceAddress(),
            voxel_jfa_buffers[0].getDeviceAddress(),

    };

    JFAIterationPushConstant jfa_iteration_push_constant = {
            0,
            0,
            voxel_jfa_buffers[0].getDeviceAddress(),
            voxel_jfa_buffers[1].getDeviceAddress(),
            voxel_df_buffer.getDeviceAddress()
    };

    auto jfaInitPipelineLayout = device.createPipelineLayout(
            vul::TempArrayProxy<const vul::DescriptorSetLayout *>{},
            VkPushConstantRange{
                    static_cast<VkShaderStageFlags>(vul::ShaderStageFlagBits::ComputeBit),
                    0,
                    sizeof(JFAInitPushConstant)
            }).assertValue();
    auto jfaInitComputeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline jfaInitPipleline;
    {
        auto computeShader = device.createShaderModule(
                vul::readSPIRV("spirv/jfa_init.comp.spv")).assertValue();
        jfaInitComputeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        jfaInitComputeBuilder.setPipelineLayout(jfaInitPipelineLayout);
        jfaInitPipleline = jfaInitComputeBuilder.create().assertValue();
    }

    auto jfaIterationPipelineLayout = device.createPipelineLayout(
            vul::TempArrayProxy<const vul::DescriptorSetLayout *>{},
            VkPushConstantRange{
                    static_cast<VkShaderStageFlags>(vul::ShaderStageFlagBits::ComputeBit),
                    0,
                    sizeof(JFAIterationPushConstant)
            }).assertValue();
    auto jfaIterationComputeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline jfaIterationPipleline;
    {
        auto computeShader = device.createShaderModule(
                vul::readSPIRV("spirv/jfa_iteration.comp.spv")).assertValue();
        jfaIterationComputeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        jfaIterationComputeBuilder.setPipelineLayout(jfaIterationPipelineLayout);
        jfaIterationPipleline = jfaIterationComputeBuilder.create().assertValue();
    }


    vul::GraphicsPipeline graphicsPipeline;


    RunLengthEncodingPushConstant rlePushConstant = {
            rle_data_buffer.getDeviceAddress(),
            device_cumulative_rle_sizes.getDeviceAddress(),
            rle_bitmask_buffer.getDeviceAddress(),
            voxel_df_buffer.getDeviceAddress(),
            static_cast<std::uint32_t>(cumulative_rle_sizes.size())
    };

    auto pipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            VkPushConstantRange{
                    (vul::ShaderStageFlagBits::VertexBit |
                     vul::ShaderStageFlagBits::FragmentBit).to_underlying(),
                    0,
                    sizeof(RunLengthEncodingPushConstant)
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

    auto resizeSwapchain = [&window, &presentationQueue,  &swapchainBuilder, &swapchain,
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


    gul::StbImage pixels;
    gul::load("../../textures/texture.jpg", pixels,
              gul::StbImage::Channels::rgb_alpha);

    std::vector<std::string> texture_file_paths = {
            "../../resources/brick_goodvibes.png",
            "../../resources/clay_goodvibes.png",
            "../../resources/cobblestone_goodvibes.png",
            "../../resources/dirt_goodvibes.png",
            "../../resources/end_stone_goodvibes.png",
            "../../resources/grass_block_top_goodvibes.png",
            "../../resources/oak_planks_goodvibes.png",
            "../../resources/sand_goodvibes.png"
    };
    std::vector<gul::StbImage> textureLayers;
    for (const auto &file_path: texture_file_paths) {
        gul::StbImage block_texture;
        gul::load(file_path, block_texture, gul::StbImage::Channels::rgb_alpha);
        textureLayers.push_back(block_texture);
    }

    std::vector<vul::TempConstVoidArrayProxy> textureLayerSpans;
    for (const auto &textureLayer: textureLayers) {
        textureLayerSpans.push_back(
                vul::TempArrayProxy(textureLayer.size(), textureLayer.data()));
    }

    auto temp = vul::TempArrayProxy(textureLayerSpans);

    std::cout << vul::is_container<decltype(textureLayerSpans)>::value << std::endl;

    std::cout << vul::is_container<decltype(temp)>::value << std::endl;

    auto arrayTextureImage = allocator.createDeviceTexture(commandPool,
                                                           presentationQueue,
                                                           vul::TempArrayProxy(textureLayerSpans),
                                                           vul::createSimple2DImageInfo(
                                                                   vul::Format::R8g8b8a8Unorm,
                                                                   VkExtent2D{16,16},
                                                                   vul::ImageUsageFlagBits::TransferDstBit |
                                                                   vul::ImageUsageFlagBits::SampledBit,
                                                                   1,textureLayers.size())).assertValue();





    auto arrayTextureView = arrayTextureImage.createImageView(vul::ImageSubresourceRange(
            vul::ImageAspectFlagBits::ColorBit, 0, 1, 0, 8)).assertValue();

    auto textureImage = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      vul::TempArrayProxy(
                                                              pixels.size(),
                                                              pixels.data()),
                                                      vul::createSimple2DImageInfo(
                                                              vul::Format::R8g8b8a8Unorm,
                                                              pixels.getExtent2D(),
                                                              vul::ImageUsageFlagBits::TransferDstBit |
                                                              vul::ImageUsageFlagBits::SampledBit)).assertValue();


    auto textureImageView = textureImage.createImageView(
            vul::ImageSubresourceRange(
                    vul::ImageAspectFlagBits::ColorBit)).assertValue();

    vul::SamplerBuilder samplerBuilder(device);
    samplerBuilder.setFilter(vul::Filter::Nearest);
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::Repeat);
    samplerBuilder.enableAnisotropy();
    samplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto sampler = samplerBuilder.create().assertValue();


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
                {arrayTextureView.createDescriptorInfo(sampler,
                                                       vul::ImageLayout::ShaderReadOnlyOptimal)});
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






    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::uint64_t> frameCounters(renderFinishedSemaphores.size(),
                                             0);


    std::uint32_t bits_2 = 0x3;
    std::uint32_t bits_3 = 0x7;
    std::uint32_t bits_4 = 0xF;
    std::uint32_t bits_5 = 0x1F;
    std::uint32_t bits_6 = 0x3F;
    std::uint32_t bits_7 = 0x7F;
    std::uint32_t bits_8 = 0xFF;
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 2>(bits_2)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 3>(bits_2)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 4>(bits_2)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 5>(bits_2)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 6>(bits_2)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 2>(bits_3)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 3>(bits_3)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 4>(bits_3)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 5>(bits_3)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 6>(bits_3)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 2>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 3>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 4>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 5>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 4, 6>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 8, 2>(bits_7)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 8, 3>(bits_7)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 8, 4>(bits_7)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 5, 2>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 5, 3>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 5, 4>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 5, 5>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<32>(
            uul::bit_interlace<std::uint32_t, std::uint32_t, 5, 6>(bits_4)) << '\n';
    std::cout << "test: " << std::bitset<64>(
            uul::bit_interlace<std::uint64_t, std::uint32_t, 21, 3>(0xFFFFu)) << '\n';
    auto temp_interleave =(uul::bit_interlace<std::uint64_t, std::uint32_t, 21, 3>(
            0xFF7Fu));
    std::cout << "test: " << std::bitset<32>(
            uul::bit_de_interlace<std::uint64_t, std::uint32_t, 21, 3>(
                    temp_interleave)) << '\n';
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
            ImGui::Text(fmt::format("CameraPos {},{},{}\n", camera.getPosition().x,camera.getPosition().y, camera.getPosition().z).c_str());
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
            uniformBuffers[swapchainImageIndex].mappedCopyFrom(ubo);
        }


        imguiRenderer.render();


        auto &commandBuffer = commandBuffers[swapchainImageIndex];
        {
            commandBuffer.begin(
                    vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
            {
                commandBuffer.bindPipeline(jfaInitPipleline);
                auto computeComputeBarrier = vul::createComputeBarrierRWARW();
                auto computeComputeDepInfo = vul::createDependencyInfo(
                        computeComputeBarrier, {}, {});
                commandBuffer.pushConstants(jfaInitPipelineLayout,
                                            vul::ShaderStageFlagBits::ComputeBit,
                                            jfa_init_push_constant);
                auto workgroup_dim = uul::integer_ceil(vul::chunk_consts::chunk_width,8);
                commandBuffer.pipelineBarrier(computeComputeDepInfo);
                commandBuffer.dispatch(workgroup_dim, workgroup_dim, workgroup_dim);
                commandBuffer.bindPipeline(jfaIterationPipleline);
                jfa_iteration_push_constant.u_voxel_jfa_in = voxel_jfa_buffers[0].getDeviceAddress();
                jfa_iteration_push_constant.u_voxel_jfa_out = voxel_jfa_buffers[1].getDeviceAddress();
                for(std::uint32_t i = 0; i < 3; ++i){
                    for(std::int32_t j = std::bit_width(31u) - 1; j >= 0; --j){
                        jfa_iteration_push_constant.u_jump_step_size = 1u << static_cast<std::uint32_t>(j);
                        jfa_iteration_push_constant.u_jump_step_type = i;
                        commandBuffer.pipelineBarrier(computeComputeDepInfo);
                        commandBuffer.pushConstants(jfaIterationPipelineLayout,
                                                    vul::ShaderStageFlagBits::ComputeBit,
                                                    jfa_iteration_push_constant);
                        commandBuffer.dispatch(workgroup_dim, workgroup_dim, workgroup_dim);
                        std::swap(jfa_iteration_push_constant.u_voxel_jfa_in, jfa_iteration_push_constant.u_voxel_jfa_out);
                    }
                    //break;
                }
                auto computeGraphicsBarrier = vul::createComputeFragmentBarrierRAW();
                auto computeGraphicsDepInfo = vul::createDependencyInfo(
                        computeGraphicsBarrier, {}, {});
                commandBuffer.pipelineBarrier(computeGraphicsDepInfo);
            }
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
                std::uint32_t box_vertex_count = 36;
                std::uint32_t vertex_split_pass_count = 5;
                commandBuffer.pushConstants(pipelineLayout,
                                            vul::ShaderStageFlagBits::VertexBit |
                                            vul::ShaderStageFlagBits::FragmentBit,
                                            rlePushConstant);
                auto vertex_count =
                        cumulative_rle_sizes.back() * box_vertex_count *
                        vertex_split_pass_count;
                renderPassBlock.draw(vertex_count);
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