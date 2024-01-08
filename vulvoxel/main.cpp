//
// Created by Shae Bolt on 6/5/2021.
//

#include <vul/writedescriptorset.h>
#include <vul/dependencyinfo.h>
#include <vul/memorybarrier.h>
#include <vul/imagecreateinfo.h>
#include <vul/imagememorybarrier.h>
#include <gul/imguirenderer.h>
#include <gul/noise/fbm.h>
#include <czdr/bitutil/bit.h>
#include <czdr/stdutil/math.h>
#include <czdr/stdutil/math_literals.h>
#include <gul/noise/opensimplex.h>
//#include "chunkmanagement.h"
#include "cpu_bitmask_intersect.h"
#include "chunks.h"
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

//see https://github.com/KhronosGroup/Vulkan-Samples/tree/master/samples/extensions
//see https://www.khronos.org/blog/vulkan-timeline-semaphores
//see https://github.com/KhronosGroup/Vulkan-Guide/blob/master/chapters/extensions/VK_KHR_synchronization2.md
//see https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples

#include <string_view>

#include "raytrace_bitfield_renderer.h"

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    glm::vec3 camera_pos;
    float u_time;
};
static_assert(sizeof(UniformBufferObject) == 16 * 4 * 3 + 16);

struct alignas(8) RunLengthEncodingPushConstant {
    vul::DeviceAddress u_material_data_block_ptr;
    vul::DeviceAddress u_cumulative_block_offsets;
    vul::DeviceAddress u_bitmasks_ref;
    vul::DeviceAddress u_voxel_sdf_out;
    std::uint32_t u_cumulative_block_offsets_size;
};
static_assert(sizeof(RunLengthEncodingPushConstant) == 40);

struct alignas(8) JFAInitPushConstant {
    std::uint32_t u_block_idx;
    std::uint32_t u_padding;
    vul::DeviceAddress u_bitmask_ref;
    vul::DeviceAddress u_voxel_jfa_out;
};
static_assert(sizeof(JFAInitPushConstant) == 24);

struct alignas(8) JFAIterationPushConstant {
    std::uint32_t u_jump_step_size;
    std::uint32_t u_jump_step_type;
    vul::DeviceAddress u_voxel_jfa_in;
    vul::DeviceAddress u_voxel_jfa_out;
    vul::DeviceAddress u_voxel_sdf_out;
};
static_assert(sizeof(JFAIterationPushConstant) == 32);

#include <czdr/glfw/core.h>
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

    auto device = physicalDevice.createDevice(surface,
                                              {graphicsComputeFamily},
                                              {computeTransferFamily},
                                              deviceExtensions,
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

    std::array validSwapchainResults = {vul::Result::SuboptimalKhr,
                                        vul::Result::ErrorOutOfDateKhr};
    std::uint64_t frame_counter = 0;

    //TODO make single subpass renderpass vulkan automatically?
    //TODO could just do binary search through texels
    //TODO need to merge texel coordinates with subsequent frames.
    //TODO LRU cache design for texels?

    auto renderPassBuilder = vul::RenderPassBuilder();
    auto presentAttachment = renderPassBuilder.addAttachment(vul::AttachmentDescription::PresentTemp(swapchain.getFormat()));
    auto depthAttachment = renderPassBuilder.addAttachment(vul::AttachmentDescription::DepthTemp(vul::Format::D24UnormS8Uint));
    auto& subpassBuilder = renderPassBuilder.createSubpass();
    subpassBuilder.addColor(presentAttachment)
                  .setDepthStencil(depthAttachment);
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
    auto renderPass = renderPassBuilder.createRenderPass(device).assertValue();
    renderPass.setObjectName("MyActualRenderPass");
    auto commandPool = device.createCommandPool(
            presentationQueue.getQueueFamilyIndex(),
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();

    auto descriptorSetLayoutBuilder = vul::DescriptorSetLayoutBuilder(device);
    descriptorSetLayoutBuilder.setBindings({vul::UniformBufferBinding(0,
                                                                      vul::ShaderStageFlagBits::VertexBit |
                                                                      vul::ShaderStageFlagBits::FragmentBit),
                                            vul::CombinedSamplerBinding(1,

                                                                        vul::ShaderStageFlagBits::FragmentBit)});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    std::vector<std::uint32_t> chunk_data(32 * 32 * 32, 1);
    std::vector<std::uint32_t> zorder_chunk_data(32 * 32 * 32, 1);


    std::vector<std::uint32_t> set_offsets = {1, 5, 2, 24, 32 * 32 - 32, 32,
                                              32 * 32 - 32, 32 * 32, 32 * 3 + 4,
                                              28 + 17, 32 * 8, 32 * 32};
    std::vector<std::uint32_t> set_values = {4, 5, 7, 6, 2, 3, 2, 0, 2, 4, 1,
                                             0};

    std::size_t total_offset = 0;
    for (auto [offset, value]: ranges::views::zip(set_offsets, set_values)) {
        for (std::size_t i = total_offset; i < total_offset + offset; ++i) {
            chunk_data[i] = value;
        }
        total_offset += offset;
    }
    bool clear = false;
    vul::ChunkSpan chunk_span(vul::make_chunk_span(chunk_data));
    vul::ChunkSpan zorder_chunk_span(vul::make_chunk_span(zorder_chunk_data));
    for (std::size_t y = 0; y < 32; ++y) {
        for (std::size_t z = 0; z < 32; ++z) {
            for (std::size_t x = 0; x < 32; ++x) {
                if (clear) {
                    chunk_span(x, y, z) = 0;
                    continue;
                }
                auto d_x = static_cast<double>(x);
                auto d_y = static_cast<double>(y);
                auto d_z = static_cast<double>(z);
                auto height = gul::fbm2d(d_x, d_z, &gul::Noise2, 2, 0.5f, 1.0 / 16.0, 2.0, 32.0);
                std::uint32_t chunk_material = 0;
                if ((31 - d_y) < height) {

                    auto solid_value = gul::fbm3d(d_x, d_y, d_z, &gul::Noise3_Fallback, 1, 1.0f, 8.0, 2.0, 1.0);
                    if (solid_value > 0.0) {
                        chunk_material = 8;
                    } else {
                        chunk_material = 8;
                    }
                }

                chunk_span(x, y, z) = chunk_material;
                auto zx = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(x);
                auto zy = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(y);
                auto zz = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(z);
//                fmt::print(stdout,"{},{},{} : {},{},{}\n", std::bitset<16>(x).to_string(), std::bitset<16>(y).to_string(), std::bitset<16>(z).to_string(),
//                           std::bitset<16>(zx).to_string(), std::bitset<16>(zy).to_string(), std::bitset<16>(zz).to_string());
                std::uint16_t combined = zx | (zy << 1u) | (zz << 2u);
//                fmt::print(stdout, "{}\n", combined);
//                std::cout << std::flush;

                zorder_chunk_span[combined] = chunk_material;
            }
        }
    }

    chunk_span(0, 0, 0) = 8;

//    chunk_span(2,0,0) = 8;
//    chunk_span(0,2,0) = 8;
    chunk_span(0, 0, 2) = 8;

    chunk_span(15, 15, 15) = 5;
    {
        auto x = 0;
        auto y = 0;
        auto z = 0;
        auto zx = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(x);
        auto zy = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(y);
        auto zz = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(z);
        std::uint16_t combined = zx | (zy << 1u) | (zz << 2u);
        zorder_chunk_span[combined] = 8;
    }
    {
        auto x = 0;
        auto y = 0;
        auto z = 2;
        auto zx = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(x);
        auto zy = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(y);
        auto zz = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(z);
        std::uint16_t combined = zx | (zy << 1u) | (zz << 2u);
        zorder_chunk_span[combined] = 8;
    }
    {
        auto x = 15;
        auto y = 15;
        auto z = 15;
        auto zx = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(x);
        auto zy = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(y);
        auto zz = czdr::bit_interlace<std::uint16_t, std::uint16_t, 5, 3>(z);
        std::uint16_t combined = zx | (zy << 1u) | (zz << 2u);
        zorder_chunk_span[combined] = 5;
    }

//    chunk_span(2,2,0) = 8;
//    chunk_span(0,2,2) = 8;
//    chunk_span(2,2,2) = 8;

    auto bitmask = vul::ChunkBitmask::from_filled(chunk_span);

    auto bitmask2 = vul::ChunkBitmaskByteRLE::from_filled(chunk_span);
    auto bitmask3 = vul::ChunkBitmaskBitRLE::from_filled(chunk_span);
    auto bitmask4 = vul::ChunkBitmaskLayerTableBitRLE::from_filled(chunk_span);

    auto bitmask5 = vul::ChunkBitmaskGridLayer2::from_filled(chunk_span);
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



    auto chunk_rle = vul::ChunkRLE::from_linear(std::span<std::uint32_t, vul::chunk_consts::chunk_size>(chunk_data));
//    //TODO removing hidden actually makes
//    chunk_rle = vul::ChunkRLE::from_removing_id(vul::ChunkRLE::from_removing_hidden(std::span<std::uint32_t,vul::chunk_consts::chunk_size>(chunk_data)));


    auto zchunk_rle = vul::ChunkRLE::from_linear(
            std::span<std::uint32_t, vul::chunk_consts::chunk_size>(zorder_chunk_data));
    std::vector<std::uint32_t> cumulative_rle_sizes = {chunk_rle.size()};
    auto device_cumulative_rle_sizes = allocator.createDeviceBuffer(commandPool,
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


    auto test = vul::TempConstVoidArrayProxy(chunk_rle.get_material_ids());
    //TODO figure out what the deal is with this? maybe remove tempconstvoidarrayproxy?
    auto rle_staging_materials = allocator.createStagingBuffer(chunk_rle.get_material_ids()).assertValue();
    auto rle_staging_offsets = allocator.createStagingBuffer(chunk_rle.get_offsets()).assertValue();
    auto data_chunk_block_size_bytes = 1024 * 1024 * 12;
    auto data_chunk_block_rle_offset_begin = 1024 * 1024 * 8;


    auto rle_data_buffer = allocator.createDeviceBuffer(data_chunk_block_size_bytes,
                                                        vul::BufferUsageFlagBits::ShaderDeviceAddressBit
                                                        | vul::BufferUsageFlagBits::TransferDstBit).assertValue();


    vul::copy(rle_staging_materials, rle_data_buffer, commandPool, presentationQueue, 0);
    vul::copy(rle_staging_offsets, rle_data_buffer, commandPool, presentationQueue, data_chunk_block_rle_offset_begin);

    auto rle_materials_address = rle_data_buffer.getDeviceAddress();
    auto rle_offsets_address = rle_materials_address + data_chunk_block_rle_offset_begin;


    std::array voxel_jfa_buffers = {
            allocator.createDeviceBuffer(sizeof(std::uint16_t) * vul::chunk_consts::chunk_size,
                                         vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue(),
            allocator.createDeviceBuffer(sizeof(std::uint16_t) * vul::chunk_consts::chunk_size,
                                         vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue()
    };

    auto voxel_df_buffer = allocator.createDeviceBuffer(sizeof(std::uint8_t) * vul::chunk_consts::chunk_size,
                                                        vul::BufferUsageFlagBits::ShaderDeviceAddressBit |
                                                        vul::BufferUsageFlagBits::TransferSrcBit).assertValue();

    auto voxel_df_host_buffer = allocator.createMappedCoherentBuffer(
            sizeof(std::uint8_t) * vul::chunk_consts::chunk_size,
            vul::BufferUsageFlagBits::TransferDstBit).assertValue();

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
            vul::PushConstantRange::create<JFAInitPushConstant>(
                    vul::ShaderStageFlagBits::ComputeBit)).assertValue();
    auto jfaInitComputeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline jfaInitPipleline;
    {
        auto computeShader = device.createShaderModule("spirv/jfa_init.comp.spv").assertValue();
        jfaInitComputeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        jfaInitComputeBuilder.setPipelineLayout(jfaInitPipelineLayout);
        jfaInitPipleline = jfaInitComputeBuilder.create().assertValue();
    }


    auto jfaIterationPipelineLayout = device.createPipelineLayout(
            vul::PushConstantRange::create<JFAIterationPushConstant>(
                    vul::ShaderStageFlagBits::ComputeBit)).assertValue();
    auto jfaIterationComputeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline jfaIterationPipleline;
    {
        auto computeShader = device.createShaderModule("spirv/jfa_iteration.comp.spv").assertValue();
        jfaIterationComputeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        jfaIterationComputeBuilder.setPipelineLayout(jfaIterationPipelineLayout);
        jfaIterationPipleline = jfaIterationComputeBuilder.create().assertValue();
    }

//    layout(set = 0, binding = 0) uniform UniformBufferObject {
//            uvec2 u_resolution;
//            float u_fov;
//            float u_focus_distance;
//    };
//
//    layout(set = 0, binding = 1, r32f) uniform writeonly image2D u_output_image[];
//
//    layout(push_constant, scalar) uniform PushConstantBlock{
//            vec3 u_camera_origin;
//            uint u_frame_idx;
//            vec3 u_camera_rotation;
//            float u_time;
////32 bytes, max 128 bytes.
//            uint32_array u_bitmask;
//            uint64_t padding;
//    };


    vul::RaytraceBitfieldRenderer raytraceBitfieldRenderer(device, swapchainSize);
    vul::RaytraceBitfieldDisplayRenderer bitfieldDisplayRenderer(device, swapchain, renderPass);
    std::vector<vul::Image> raytrace_bitfield_images;
    std::vector<vul::ImageView> raytrace_bitfield_image_views;

    auto raytrace_bitifield_image_info = vul::ImageCreateInfo::sampledStorage2D(
            swapchain.getExtent(), vul::Format::R32Sfloat);
    auto raytrace_bitfield_image_transition_barrier = vul::ImageMemoryBarrier::unfilledLayoutTransition(
            vul::PipelineStageFlagBits2::TopOfPipeBit,
            vul::PipelineStageFlagBits2::AllCommandsBit,
            vul::AccessFlagBits2::ShaderWriteBit,
            vul::ImageLayout::Undefined,
            vul::ImageLayout::General);
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        raytrace_bitfield_images.push_back(allocator.createDeviceImage(raytrace_bitifield_image_info).assertValue());
        raytrace_bitfield_image_transition_barrier.setImage(raytrace_bitfield_images.back());
        vul::transition(raytrace_bitfield_image_transition_barrier.get(), commandPool, presentationQueue);
        raytrace_bitfield_image_views.push_back(raytrace_bitfield_images.back().createImageView().assertValue());
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
            vul::PushConstantRange::create<RunLengthEncodingPushConstant>((vul::ShaderStageFlagBits::VertexBit |
                                                                           vul::ShaderStageFlagBits::FragmentBit))
    ).assertValue();
//    auto pipelineLayout = device.createPipelineLayout(
//            descriptorLayout).assertValue();
    auto pipelineBuilder = vul::GraphicsPipelineBuilder(device);


    pipelineBuilder.setPrimitiveStateInfo();
    pipelineBuilder.setViewportStateFromExtent(swapchain.getExtent());
    pipelineBuilder.setDynamicState({vul::DynamicState::Viewport, vul::DynamicState::Scissor});
    pipelineBuilder.setRasterizationState();
    pipelineBuilder.setMultisampleState();
    pipelineBuilder.setDepthStencilState();
    pipelineBuilder.setBlendState({vul::PipelineColorBlendAttachmentState::disableBlendRGBA()});
    pipelineBuilder.setRenderPass(renderPass, 0);
    pipelineBuilder.setPipelineLayout(pipelineLayout);
    {
        auto vertexShader = device.createShaderModule("spirv/rle_render.vert.spv").assertValue();
        auto fragmentShader = device.createShaderModule("spirv/rle_render.frag.spv").assertValue();
        pipelineBuilder.setShaderCreateInfo(
                vertexShader.createVertexStageInfo(),
                fragmentShader.createFragmentStageInfo());
        graphicsPipeline = pipelineBuilder.create().assertValue();
    }



    std::vector<vul::Image> depthImages(swapchainSize);

    for(auto& depthImage : depthImages){
        depthImage = allocator.createDeviceImage(vul::ImageCreateInfo::depthStencilAttachment2D(swapchain.getExtent())).assertValue();
    }
    std::vector<vul::ImageView> depthImageViews(swapchainSize);
    for(auto [i,depthImageView] : depthImageViews | ranges::views::enumerate){
        depthImageView = depthImages[i].createImageView().assertValue();
    }



    std::vector<vul::Framebuffer> swapchainFramebuffers;
    for (const auto &[imageView, depthImageView]: ranges::views::zip(swapchainImageViews, depthImageViews)) {
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments({&imageView,
                                           &depthImageView});
        framebufferBuilder.setDimensions(swapchain.getExtent());
        framebufferBuilder.setRenderPass(renderPass);
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
        swapchain = swapchainBuilder.resize(swapchain, surface.getCapabilitiesFrom(physicalDevice).currentExtent).assertValue();
    };

    auto resizeSwapchainFramebuffers = [&](std::size_t swapchainImageIndex){
        depthImages[swapchainImageIndex] = allocator.createDeviceImage(vul::ImageCreateInfo::depthStencilAttachment2D(swapchain.getExtent())).assertValue();
        depthImageViews[swapchainImageIndex] = depthImages[swapchainImageIndex].createImageView().assertValue();
        const auto &imageView = swapchain.getImageViews()[swapchainImageIndex];
        vul::FramebufferBuilder framebufferBuilder(device);
        framebufferBuilder.setAttachments({&imageView,&depthImageViews[swapchainImageIndex]});
        framebufferBuilder.setDimensions(swapchain.getExtent());
        framebufferBuilder.setRenderPass(renderPass);
        swapchainFramebuffers[swapchainImageIndex] = framebufferBuilder.create().assertValue();
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

    auto arrayTextureImage = allocator.createDeviceTextureArray(commandPool,
                                                                presentationQueue,
                                                                vul::make_proxy(textureLayers),
                                                                vul::ImageCreateInfo::texture2D({16, 16}, vul::Format::R8g8b8a8Unorm, 1, textureLayers.size())
                                                               ).assertValue();





    auto arrayTextureView = arrayTextureImage.createImageView().assertValue();

    auto textureImage = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      pixels,
                                                      vul::ImageCreateInfo::texture2D(pixels.getExtent2D(),vul::Format::R8g8b8a8Unorm)
                                                     ).assertValue();


    auto textureImageView = textureImage.createImageView().assertValue();

    vul::SamplerBuilder samplerBuilder(device);
    samplerBuilder.setFilter(vul::Filter::Nearest);
    samplerBuilder.setAddressMode(vul::SamplerAddressMode::Repeat);
    samplerBuilder.enableAnisotropy();
    samplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto sampler = samplerBuilder.create().assertValue();


    std::vector<vul::Buffer> uniformBuffers;
    std::vector<vul::Buffer> raytraceBitfieldUniformBuffers;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        uniformBuffers.push_back(
                allocator.createMappedCoherentBuffer(sizeof(UniformBufferObject),
                                                     vul::BufferUsageFlagBits::UniformBufferBit).assertValue());
        raytraceBitfieldUniformBuffers.push_back(
                allocator.createMappedCoherentBuffer(sizeof(vul::RaytraceBitfieldRenderer::UniformBufferObject),
                                                     vul::BufferUsageFlagBits::UniformBufferBit).assertValue());
    }



    auto descriptorPool = device.createDescriptorPool(
            {{descriptorSetLayoutBuilder, swapchainSize},
             {raytraceBitfieldRenderer.getDescriptorSetLayoutBuilder(), swapchainSize},
             {bitfieldDisplayRenderer.getDescriptorSetLayoutBuilder(), swapchainSize}
             },
             vul::DescriptorPoolCreateFlagBits::UpdateAfterBindBit).assertValue();

    auto descriptorSets = descriptorPool.createDescriptorSets(
            {{descriptorLayout, swapchainSize}}).assertValue();

    for (const auto &[i, descriptorSet]: descriptorSets |
                                         ranges::views::enumerate) {
        vul::WriteDescriptorSetBuilder writeBuilder(descriptorSet, descriptorSetLayoutBuilder.createDescriptorTypeCounts());
        writeBuilder[0] = {&uniformBuffers[i]};
        writeBuilder[1].set({&arrayTextureView}, sampler);
        device.updateDescriptorSets(writeBuilder.getWrites());
    }

    auto raytraceBitfieldDescriptorSets = descriptorPool.createDescriptorSets(
            {{raytraceBitfieldRenderer.getDescriptorSetLayout(), swapchainSize}}).assertValue();

    auto raytraceBitfieldDisplayRendererDescriptorSets = descriptorPool.createDescriptorSets(
            {{bitfieldDisplayRenderer.getDescriptorSetLayout(), swapchainSize}}).assertValue();

    //TODO type safe storage image, uniform buffer, accept normal buffer, and have "createDescriptorInfo" happen inside?
    for (const auto &[i, zippedDescriptorSet]: ranges::views::zip(raytraceBitfieldDescriptorSets, raytraceBitfieldDisplayRendererDescriptorSets) |
                                         ranges::views::enumerate){
        auto extent = glm::uvec2(swapchain.getExtent().width, swapchain.getExtent().height);
        vul::RaytraceBitfieldRenderer::UniformBufferObject ubo(extent, glm::radians(90.0), 1.0);
        raytraceBitfieldUniformBuffers[i].mappedCopyFrom(ubo);
        {
            vul::WriteDescriptorSetBuilder writeBuilder(zippedDescriptorSet.first,
                                                        raytraceBitfieldRenderer.getDescriptorTypeCounts());
            writeBuilder[0] = {&raytraceBitfieldUniformBuffers[i]};
            writeBuilder[1] = {&raytrace_bitfield_image_views[i]};
            device.updateDescriptorSets(writeBuilder.getWrites());
        }
        {
            vul::WriteDescriptorSetBuilder writeBuilder(zippedDescriptorSet.second,
                                                        bitfieldDisplayRenderer.getDescriptorTypeCounts());
            writeBuilder[0].set(&raytrace_bitfield_image_views[i], sampler);
            device.updateDescriptorSets(writeBuilder.getWrites());
        }
    }


    auto resize_bitfield_assets = [&](std::size_t swapchainImageIndex){
        auto extent = glm::uvec2(swapchain.getExtent().width, swapchain.getExtent().height);
        vul::RaytraceBitfieldRenderer::UniformBufferObject ubo(extent, glm::radians(90.0), 1.0);
        raytraceBitfieldUniformBuffers[swapchainImageIndex].mappedCopyFrom(ubo);
        raytrace_bitfield_images[swapchainImageIndex] = (allocator.createDeviceImage(
        raytrace_bitifield_image_info).assertValue());
        raytrace_bitfield_image_transition_barrier.setImage(raytrace_bitfield_images[swapchainImageIndex]);
        vul::transition(raytrace_bitfield_image_transition_barrier, commandPool,presentationQueue);
        raytrace_bitfield_image_views[swapchainImageIndex] = (raytrace_bitfield_images[swapchainImageIndex].createImageView().assertValue());
        {
            const auto &descriptorSet = raytraceBitfieldDescriptorSets[swapchainImageIndex];
            vul::WriteDescriptorSetBuilder writeBuilder(descriptorSet,
                                                        raytraceBitfieldRenderer.getDescriptorTypeCounts());
            writeBuilder[1] = {&raytrace_bitfield_image_views[swapchainImageIndex]};
            device.updateDescriptorSets(writeBuilder.getWrites());
        }
        {
            const auto &descriptorSet = raytraceBitfieldDisplayRendererDescriptorSets[swapchainImageIndex];
            vul::WriteDescriptorSetBuilder writeBuilder(descriptorSet,
                                                        bitfieldDisplayRenderer.getDescriptorTypeCounts());
            writeBuilder[0].set(&raytrace_bitfield_image_views[swapchainImageIndex], sampler);
            device.updateDescriptorSets(writeBuilder.getWrites());
        }
    };

    auto commandBuffers = commandPool.createPrimaryCommandBuffers(
            swapchainSize).assertValue();

    gul::ImguiRenderer imguiRenderer(window, instance, device, swapchain,
                                     presentationQueue,
                                     swapchain.getFormat());
    std::vector<std::function<void(std::size_t swapchainImageIndex)>> frameResourceResizeFunctions = {{},{},{}};

    auto resizeImGuiFramebuffer = imguiRenderer.createResizeFrameCallback();
    auto frameResourceResizeFunction = [&](std::size_t swapchainImageIndex){
        resizeImGuiFramebuffer(swapchainImageIndex);
        resizeSwapchainFramebuffers(swapchainImageIndex);
        resize_bitfield_assets(swapchainImageIndex);
    };
    auto resizeWindow = [&resizeSwapchain, & frameResourceResizeFunctions, &frameResourceResizeFunction, &raytrace_bitifield_image_info, &swapchain]() {
        resizeSwapchain();
        raytrace_bitifield_image_info.setExtent(swapchain.getExtent());
        frameResourceResizeFunctions = {frameResourceResizeFunction,frameResourceResizeFunction,frameResourceResizeFunction};
    };


    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::uint64_t> frameCounters(renderFinishedSemaphores.size(),
                                             0);


    while (!window.should_close()) {
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

            ImGui::Text(fmt::format("cursor_pos x:{},y:{}", cursor_pos.xpos,
                                    cursor_pos.ypos).c_str());
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

        if(frameResourceResizeFunctions[swapchainImageIndex] != nullptr){
            frameResourceResizeFunctions[swapchainImageIndex](swapchainImageIndex);
            frameResourceResizeFunctions[swapchainImageIndex] = {};
        }
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
            static float u_time = 0;
            ubo.u_time = time;
            ubo.camera_pos = camera.getPosition();
            u_time += 1.0f;
            uniformBuffers[swapchainImageIndex].mappedCopyFrom(ubo);
        }


        imguiRenderer.render();

        using namespace czdr::literals;

        auto &commandBuffer = commandBuffers[swapchainImageIndex];
        {
            commandBuffer.begin(
                    vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
            if(false) {
                {
                    commandBuffer.bindPipeline(jfaInitPipleline);
                    auto computeComputeBarrier = {vul::MemoryBarrier::computeToComputeRWARW()};
                    auto computeComputeDepInfo = vul::DependencyInfo({computeComputeBarrier}, {}, {});
                    commandBuffer.pushConstants(jfaInitPipelineLayout,
                                                vul::ShaderStageFlagBits::ComputeBit,
                                                jfa_init_push_constant);
                    auto workgroup_dim = czdr::ceil(vul::chunk_consts::chunk_width, 8_u8);
                    commandBuffer.pipelineBarrier(computeComputeDepInfo);
                    commandBuffer.dispatch(workgroup_dim, workgroup_dim, workgroup_dim);
                    commandBuffer.bindPipeline(jfaIterationPipleline);
                    jfa_iteration_push_constant.u_voxel_jfa_in = voxel_jfa_buffers[0].getDeviceAddress();
                    jfa_iteration_push_constant.u_voxel_jfa_out = voxel_jfa_buffers[1].getDeviceAddress();
                    for (std::uint32_t i = 0; i < 3; ++i) {
                        for (std::int32_t j = std::bit_width(31u) - 1; j >= 0; --j) {
                            jfa_iteration_push_constant.u_jump_step_size = 1u << static_cast<std::uint32_t>(j);
                            jfa_iteration_push_constant.u_jump_step_type = i;
                            commandBuffer.pipelineBarrier(computeComputeDepInfo);
                            commandBuffer.pushConstants(jfaIterationPipelineLayout,
                                                        vul::ShaderStageFlagBits::ComputeBit,
                                                        jfa_iteration_push_constant);
                            commandBuffer.dispatch(workgroup_dim, workgroup_dim, workgroup_dim);
                            std::swap(jfa_iteration_push_constant.u_voxel_jfa_in,
                                      jfa_iteration_push_constant.u_voxel_jfa_out);
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
            }
            {
                {
                    auto storageToSampleImageBarrier = {
                            vul::ImageMemoryBarrier::WriteAfterRead(raytrace_bitfield_images[swapchainImageIndex],
                                                                    vul::PipelineStageFlagBits2::FragmentShaderBit,
                                                                    vul::PipelineStageFlagBits2::ComputeShaderBit,
                                                                    vul::ImageLayout::Undefined,
                                                                    vul::ImageLayout::General)};
                    auto dependencyInfo = vul::DependencyInfo({} , {}, {storageToSampleImageBarrier});
                    commandBuffer.pipelineBarrier(dependencyInfo);
                    auto pushConstant = vul::RaytraceBitfieldRenderer::PushConstant{
                            camera.getPosition(),
                            0, // no longer needed, still want for padding though?
                            camera.getRotation(),
                            0, //time not needed right now.
                            rle_bitmask_buffer.getDeviceAddress(),
                            0,
                    };

                    commandBuffer.bindPipeline(raytraceBitfieldRenderer.getPipeline());
                    commandBuffer.bindDescriptorSets(
                            vul::PipelineBindPoint::Compute, raytraceBitfieldRenderer.getPipelineLayout(),
                            raytraceBitfieldDescriptorSets[swapchainImageIndex]);
                    commandBuffer.pushConstants(raytraceBitfieldRenderer.getPipelineLayout(),
                                                vul::ShaderStageFlagBits::ComputeBit,
                                                pushConstant);
                    auto workgroup_dim_x = czdr::ceil(swapchain.getExtent().width, 32u);
                    auto workgroup_dim_y = czdr::ceil(swapchain.getExtent().height, 32u);
                    commandBuffer.dispatch(workgroup_dim_x, workgroup_dim_y, 1);
                }

                {
                    auto storageToSampleImageBarrier = {
                            vul::ImageMemoryBarrier::computeToFragmentRAW(raytrace_bitfield_images[swapchainImageIndex])};
                    auto dependencyInfo = vul::DependencyInfo({} , {}, {storageToSampleImageBarrier});
                    commandBuffer.pipelineBarrier(dependencyInfo);
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
                    commandBuffer.bindPipeline(bitfieldDisplayRenderer.getPipeline());
                    commandBuffer.bindDescriptorSets(
                            vul::PipelineBindPoint::Graphics, bitfieldDisplayRenderer.getPipelineLayout(),
                            raytraceBitfieldDisplayRendererDescriptorSets[swapchainImageIndex]);
                    renderPassBlock.draw(3);
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


        if (false) {
            vul::copy(voxel_df_buffer, voxel_df_host_buffer, commandPool, presentationQueue);

            auto mapped_memory = reinterpret_cast<const std::uint8_t *>(voxel_df_host_buffer.mapMemory());
            std::cout << "\nSTART\n";
            for (std::size_t z = 0; z < 32; z++) {
                std::cout << "\n";
                for (std::size_t y = 0; y < 32; y++) {
                    std::cout << "\n";
                    for (std::size_t x = 0; x < 32; x++) {
                        std::cout << static_cast<std::uint32_t>(mapped_memory[z * 32 * 32 + y * 32 + x]) << ", ";
                    }
                }
            }
            std::cout << "\nEND\n";
        }

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