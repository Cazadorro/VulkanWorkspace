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

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}
inline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::mt19937 generator;
    return distribution(generator);
}
static std::size_t initial_seed = 23423459235346u;
static std::uniform_real_distribution<float> out_distribution(0.0f, 1.0f);
static std::mt19937 out_generator(initial_seed);
inline float random_float_out(){
    return out_distribution(out_generator);
}
inline float random_float_out(float min, float max){
    return min + (max-min)*random_float_out();
}
inline float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_float();
}
inline glm::vec3 random_color(){
    return {random_float(),random_float(),random_float()};
}
inline glm::vec3 random_color(float min, float max){
    return {random_float(min,max),random_float(min,max),random_float(min,max)};
}


float line_segment_distance(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b){
    glm::vec3 ba = b-a;
    glm::vec3 pa = point-a;
    float h = glm::clamp( dot(pa,ba)/glm::dot(ba,ba), 0.0f, 1.0f );
    glm::vec3  q = pa-h*ba;
    float d = glm::length(q);
    return d;
}

std::vector<std::uint8_t> generate_distance_field(std::size_t segment_count){
    std::size_t distance_field_dim = 128;
    auto distance_field_dimf = static_cast<float>(distance_field_dim);
    std::size_t distance_field_size = distance_field_dim*distance_field_dim*distance_field_dim;
    std::vector<std::uint8_t> distance_field(distance_field_size);

    glm::vec3 prev_pt(random_float(0.0f, distance_field_dimf),random_float(0.0f, distance_field_dimf), distance_field_dimf);

    prev_pt = glm::vec3(distance_field_dimf/2.0,distance_field_dimf/2.0,distance_field_dimf/2.0);

    std::vector<glm::vec3> points;
    points.push_back(prev_pt);
    for(std::size_t i = 0; i < segment_count; ++i){
        auto point = points.back();
        glm::vec3 random_offset( random_float(-8.0f, 8.0f),random_float(-8.0f, 8.0f), random_float(-8.0f, 8.0f));
        auto new_point = point + random_offset;
        points.push_back(new_point);
    }
    glm::vec3 end_pt(random_float(0.0f, distance_field_dimf),random_float(0.0f, distance_field_dimf), 0.0f);
//    points.push_back(end_pt);
//    points.clear();
//    points.push_back(glm::vec3(0.0f,64.0f,distance_field_dimf));
//    points.push_back(glm::vec3(distance_field_dimf,64.0f,0.0f));
    for(std::size_t y = 0; y < distance_field_dim; ++y){
        for(std::size_t z = 0; z < distance_field_dim; ++z){
            for(std::size_t x = 0; x < distance_field_dim; ++x){
                glm::vec3 point(static_cast<float>(x),static_cast<float>(y),static_cast<float>(z));
                glm::vec3 prev_point = points[0];
                float dist = std::numeric_limits<float>::infinity();
                for(auto& next_point : points | ranges::views::drop(1)){
                    dist = glm::min(dist, line_segment_distance(point, prev_point, next_point));
                    prev_point = next_point;
                }
                auto uint_dist = static_cast<std::uint8_t>(glm::clamp(dist, 0.0f, 255.0f));
                auto linear_idx = y * distance_field_dim * distance_field_dim + z * distance_field_dim + x;
                distance_field.at(linear_idx) = uint_dist;
            }
        }
    }
    return distance_field;
}

struct DistanceIDPoints{
    std::vector<std::uint8_t> distance_field_ids;
    std::vector<glm::vec3> points;
    std::vector<std::uint8_t> rle_idxs;
    std::vector<std::uint32_t> rle_distances;
};
DistanceIDPoints generate_closest_distance_id(std::size_t segment_count, std::size_t distance_field_dim){
    auto distance_field_dimf = static_cast<float>(distance_field_dim);
    std::size_t distance_field_size = distance_field_dim*distance_field_dim*distance_field_dim;
    std::vector<std::uint8_t> distance_field_ids(distance_field_size);

    glm::vec3 prev_pt(random_float(0.0f, distance_field_dimf), distance_field_dimf, random_float(0.0f, distance_field_dimf));

    prev_pt = glm::vec3(distance_field_dimf/2.0,distance_field_dimf/2.0,distance_field_dimf/2.0);

    std::vector<glm::vec3> points;
    points.push_back(prev_pt);
    for(std::size_t i = 0; i < segment_count; ++i){
        auto point = points.back();
        glm::vec3 random_offset( random_float(-8.0f, 8.0f),random_float(-8.0f, 8.0f), random_float(-8.0f, 8.0f));
        auto new_point = point + random_offset;
        points.push_back(new_point);
    }
    glm::vec3 end_pt(random_float(0.0f, distance_field_dimf),0.0f, random_float(0.0f, distance_field_dimf));
    prev_pt = glm::vec3(distance_field_dimf/2.0,distance_field_dimf,distance_field_dimf/2.0);
    end_pt = glm::vec3(distance_field_dimf/2.0,0.0f,distance_field_dimf/2.0);
//        points.clear();
//    points.push_back(prev_pt);
//    points.push_back(end_pt);

//    points.push_back(glm::vec3(0.0f,64.0f,distance_field_dimf));
//    points.push_back(glm::vec3(distance_field_dimf,64.0f,0.0f));
    std::uint8_t value;
    std::uint32_t distance;
    std::vector<std::uint8_t> rle_idxs;
    std::vector<std::uint32_t> rle_distances;
    for(std::size_t y = 0; y < distance_field_dim; ++y){
        for(std::size_t z = 0; z < distance_field_dim; ++z){
            for(std::size_t x = 0; x < distance_field_dim; ++x){
                glm::vec3 point(static_cast<float>(x) + 0.5,static_cast<float>(y) + 0.5,static_cast<float>(z) + 0.5);
                glm::vec3 prev_point = points[0];
                float dist = std::numeric_limits<float>::infinity();
                std::uint8_t best_idx = 0;
                for(auto [idx, next_point] : ranges::views::enumerate(points)  | ranges::views::drop(1)){
                    auto curr_dist =line_segment_distance(point, prev_point, next_point);
                    if(curr_dist < dist){
                        //idx of the points, but the actual line segment index is different.
                        best_idx = idx - 1;
                        dist = curr_dist;
                    }
                    prev_point = next_point;
                }
                auto linear_idx = ( distance_field_dim - y - 1) * distance_field_dim * distance_field_dim + z * distance_field_dim + x;
                distance_field_ids.at(linear_idx) = best_idx;
                if(rle_idxs.empty()){
                    rle_idxs.push_back(best_idx);
                    rle_distances.push_back(1);
                }else if(rle_idxs.back() != best_idx){
                    rle_idxs.push_back(best_idx);
                    rle_distances.push_back(1);
                }else{
                    rle_distances.back() += 1;
                }
            }
        }
    }
    return DistanceIDPoints{distance_field_ids, points, rle_idxs, rle_distances};
}
struct RLEPoints{
    std::vector<glm::vec3> points;
    std::vector<std::uint8_t> rle_idxs;
    std::vector<std::uint32_t> rle_distances;
};
RLEPoints generate_closest_distance_id2(std::size_t segment_count, std::size_t distance_field_dim, float denominator){
    out_generator.seed(initial_seed);
    auto distance_field_dimf = static_cast<float>(distance_field_dim);
    std::size_t distance_field_size = distance_field_dim*distance_field_dim*distance_field_dim;
    glm::vec3 prev_pt(random_float_out(0.0f, distance_field_dimf), distance_field_dimf, random_float_out(0.0f, distance_field_dimf));

    prev_pt = glm::vec3(distance_field_dimf/(2.0 * denominator),distance_field_dimf/(2.0 * denominator),distance_field_dimf/(2.0 * denominator));

    std::vector<glm::vec3> points;
    points.push_back(prev_pt);
    for(std::size_t i = 0; i < segment_count; ++i){
        auto point = points.back();
        glm::vec3 random_offset( random_float_out(-8.0f, 8.0f),random_float_out(-8.0f, 8.0f), random_float_out(-8.0f, 8.0f));
        auto new_point = point + random_offset;
        points.push_back(new_point);
    }
    glm::vec3 end_pt(random_float_out(0.0f, distance_field_dimf),0.0f, random_float_out(0.0f, distance_field_dimf));
    prev_pt = glm::vec3(distance_field_dimf/2.0,distance_field_dimf,distance_field_dimf/2.0);
    end_pt = glm::vec3(distance_field_dimf/2.0,0.0f,distance_field_dimf/2.0);
//        points.clear();
//    points.push_back(prev_pt);
//    points.push_back(end_pt);

//    points.push_back(glm::vec3(0.0f,64.0f,distance_field_dimf));
//    points.push_back(glm::vec3(distance_field_dimf,64.0f,0.0f));
    std::uint8_t value;
    std::uint32_t distance;
    std::vector<std::uint8_t> rle_idxs;
    std::vector<std::uint32_t> rle_distances;
    for(std::size_t y = 0; y < distance_field_dim; ++y){
        for(std::size_t z = 0; z < distance_field_dim; ++z){
            for(std::size_t x = 0; x < distance_field_dim; ++x){
                glm::vec3 point(static_cast<float>(x) + 0.5,static_cast<float>(y) + 0.5,static_cast<float>(z) + 0.5);
                point /= denominator;
                glm::vec3 prev_point = points[0];
                float dist = std::numeric_limits<float>::infinity();
                std::uint8_t best_idx = 0;
                for(auto [idx, next_point] : ranges::views::enumerate(points)  | ranges::views::drop(1)){
                    auto curr_dist =line_segment_distance(point, prev_point, next_point);
                    if(curr_dist < dist){
                        //idx of the points, but the actual line segment index is different.
                        best_idx = idx - 1;
                        dist = curr_dist;
                    }
                    prev_point = next_point;
                }
                auto linear_idx = ( distance_field_dim - y - 1) * distance_field_dim * distance_field_dim + z * distance_field_dim + x;
                if(rle_idxs.empty()){
                    rle_idxs.push_back(best_idx);
                    rle_distances.push_back(1);
                }else if(rle_idxs.back() != best_idx){
                    rle_idxs.push_back(best_idx);
                    rle_distances.push_back(1);
                }else{
                    rle_distances.back() += 1;
                }
            }
        }
    }
    return RLEPoints{points, rle_idxs, rle_distances};
}



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
                                         vul::ShaderStageFlagBits::FragmentBit,
                                         3).get()
            },
            {vul::DescriptorBindingFlagBits::UpdateAfterBindBit});
    auto descriptorLayout = descriptorSetLayoutBuilder.create().assertValue();

    vul::GraphicsPipeline graphicsPipeline;
//    auto pipelineLayout = device.createPipelineLayout(
//            descriptorLayout).assertValue();



    auto pipelineLayout = device.createPipelineLayout(
            descriptorLayout,
            VkPushConstantRange{
                    static_cast<VkShaderStageFlags>(vul::ShaderStageFlagBits::FragmentBit),
                    0,
                    sizeof(std::uint32_t)
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
                vul::readSPIRV("spirv/fullscreen.vert.spv")).assertValue();
        auto fragmentShader = device.createShaderModule(
                vul::readSPIRV("spirv/fullscreen.frag.spv")).assertValue();
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
                                                              pixels.getExtent2D(),
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


    std::vector<vul::Buffer> uniformBuffers;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        uniformBuffers.push_back(
                allocator.createMappedCoherentBuffer(bufferSize,
                                                     vul::BufferUsageFlagBits::UniformBufferBit).assertValue());
    }


    auto commandBuffers = commandPool.createPrimaryCommandBuffers(
            swapchainSize).assertValue();

    gul::ImguiRenderer imguiRenderer(window, instance, device, surface,
                                     presentQueueIndex, presentationQueue,
                                     surface.getSwapchain()->getFormat());


//    struct alignas(4) RayTracingPushConstant {
//        glm::vec3 u_camera_position;
//        std::uint32_t u_image_width;
////    16bytes
//        glm::vec3 u_camera_orientation;
//        std::uint32_t u_image_height;
////    32bytes
//        float u_fov;
//        std::uint32_t u_frame_idx;
//// 40 bytes;
//    };
//    static_assert(sizeof(RayTracingPushConstant) == 40);
    struct alignas(8) RayTracingPushConstant {
        std::uint64_t u_view_state;
        glm::vec3 u_camera_origin;
        std::uint32_t u_frame_idx;
        glm::vec3 u_camera_rotation;
        float u_time;
        std::uint64_t u_distance_ids;
        std::uint64_t u_point_array;
// 40 bytes;
    };
    static_assert(sizeof(RayTracingPushConstant) == 40 + 16);

    vul::SamplerBuilder raytracedImageSamplerBuilder(device);
    raytracedImageSamplerBuilder.setFilter(vul::Filter::Linear);
    raytracedImageSamplerBuilder.setAddressMode(
            vul::SamplerAddressMode::MirroredRepeat);
    raytracedImageSamplerBuilder.enableAnisotropy();
    raytracedImageSamplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto raytracedImageSampler = raytracedImageSamplerBuilder.create().assertValue();
    //TODO use spec consts for storage image size.
    auto raytraceDescriptorLayoutBuilder = vul::DescriptorSetLayoutBuilder(
            device);
    raytraceDescriptorLayoutBuilder.setFlags(
            vul::DescriptorSetLayoutCreateFlagBits::UpdateAfterBindPoolBit);
    raytraceDescriptorLayoutBuilder.setBindings(
            {vul::StorageImageBinding(0,
                                      vul::ShaderStageFlagBits::ComputeBit,
                                      3).get(),
             vul::CombinedSamplerBinding(1,
                                      vul::ShaderStageFlagBits::ComputeBit).get()},
            {vul::DescriptorBindingFlagBits::UpdateAfterBindBit,static_cast<vul::DescriptorBindingFlagBits>(0)});


    auto raytraceDescriptorLayout = raytraceDescriptorLayoutBuilder.create().assertValue();
    auto raytracePipelineLayout = device.createPipelineLayout(
            raytraceDescriptorLayout,
            VkPushConstantRange{
                    static_cast<VkShaderStageFlags>(vul::ShaderStageFlagBits::ComputeBit),
                    0,
                    sizeof(RayTracingPushConstant)
            }).assertValue();
    auto computeBuilder = vul::ComputePipelineBuilder(device);
    vul::ComputePipeline raytracePipeline;
    {
        auto computeShader = device.createShaderModule(
                vul::readSPIRV("spirv/cloudmarch.comp.spv")).assertValue();
        computeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo());
        computeBuilder.setPipelineLayout(raytracePipelineLayout);
        raytracePipeline = computeBuilder.create().assertValue();
    }


    auto descriptorPool = device.createDescriptorPool(
            {{descriptorSetLayoutBuilder,      swapchainSize},
             {raytraceDescriptorLayoutBuilder, 1}},
            vul::DescriptorPoolCreateFlagBits::UpdateAfterBindBit).assertValue();

    auto descriptorSets = descriptorPool.createDescriptorSets(
            {{descriptorLayout, swapchainSize}}).assertValue();
    auto raytraceDescriptorSet = descriptorPool.createDescriptorSet(
            raytraceDescriptorLayout).assertValue();
    device.setObjectName(raytraceDescriptorSet, "raytraceDescriptorSet");

    std::vector<vul::Image> raytracedImages;
    std::vector<vul::ImageView> raytracedImagesViews;

    auto distance_field =generate_distance_field(1);
    auto [distance_field_ids, points,rle_idxs, rle_distances] = generate_closest_distance_id(16, 128);
//
//    {
//        std::array test_sizes = {128, 256, 512, 1024};
//        std::array test_denom = {1.0, 2.0, 4.0, 8.0};
//        for(auto [size, denom] : ranges::views::zip(test_sizes, test_denom)){
//            auto[points1, rle_idxs1, rle_distances1] = generate_closest_distance_id2(
//                    16, size, denom);
//            fmt::print("size {} rle_idxs1 size {}\n", size, rle_idxs1.size());
//        }
//    }
    fmt::print("rle_idxs size {}\n", rle_idxs.size());

    auto device_distance_field_ids = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            distance_field_ids,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_points = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            points,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    vul::SamplerBuilder lightningSamplerBuilder(device);
    lightningSamplerBuilder.setFilter(vul::Filter::Linear);
    lightningSamplerBuilder.setAddressMode(
            vul::SamplerAddressMode::ClampToBorder);
    lightningSamplerBuilder.setBorderColor(vul::BorderColor::IntOpaqueBlack);
    lightningSamplerBuilder.enableAnisotropy();
    lightningSamplerBuilder.setMipmapMode(vul::SamplerMipmapMode::Linear);

    auto lightningSampler = lightningSamplerBuilder.create().assertValue();
    auto lightningImage = allocator.createDeviceTexture(commandPool,
                                                      presentationQueue,
                                                      distance_field,
                                                      vul::createSimple3DImageInfo(
                                                              vul::Format::R8Unorm,
                                                              VkExtent3D{128,128,128},
                                                              vul::ImageUsageFlagBits::TransferDstBit |
                                                              vul::ImageUsageFlagBits::SampledBit)).assertValue();

    auto lightningImageView = lightningImage.createImageView(
            vul::ImageSubresourceRange(
                    vul::ImageAspectFlagBits::ColorBit)).assertValue();
    auto resizeDescriptorSets = [&]() {
        raytracedImages.clear();
        raytracedImagesViews.clear();
        for (std::size_t i = 0; i < swapchainSize; ++i) {
            raytracedImages.push_back(allocator.createDeviceImage(
                    vul::createSimple2DImageInfo(
                            vul::Format::R8g8b8a8Unorm,
                            surface.getSwapchain()->getExtent(),
                            vul::ImageUsageFlagBits::SampledBit |
                            vul::ImageUsageFlagBits::StorageBit)).assertValue());
            vul::transition(raytracedImages.back(), commandPool,
                            presentationQueue,
                            vul::ImageAspectFlagBits::ColorBit,
                            vul::PipelineStageFlagBits2KHR::AllCommandsBit,
                            vul::AccessFlagBits2KHR::ShaderWriteBit,
                            vul::ImageLayout::General);
            raytracedImagesViews.push_back(
                    raytracedImages.back().createImageView(
                            vul::ImageSubresourceRange(
                                    vul::ImageAspectFlagBits::ColorBit)).assertValue());
        }


        for (const auto&[i, descriptorSet]: descriptorSets |
                                            ranges::views::enumerate) {
            auto updateBuilder = descriptorSetLayoutBuilder.createUpdateBuilder();
            updateBuilder.getDescriptorElementAt(0).setCombinedImageSampler(
                    raytracedImagesViews | ranges::views::transform(
                            [&raytracedImageSampler](auto &value) {
                                return value.createDescriptorInfo(
                                        raytracedImageSampler,
                                        vul::ImageLayout::General);
                            }) |
                    ranges::to<std::vector>());
            auto updates = updateBuilder.create(descriptorSet);
            device.updateDescriptorSets(updates);
        }

        {

            using namespace ranges;
            auto updateBuilder = raytraceDescriptorLayoutBuilder.createUpdateBuilder();
            updateBuilder.getDescriptorElementAt(0).setStorageImage(
                    raytracedImagesViews | views::transform(
                            [](auto &value) { return value.createStorageWriteInfo(); }) |
                    ranges::to<std::vector>());
            updateBuilder.getDescriptorElementAt(1).setCombinedImageSampler({lightningImageView.createDescriptorInfo(lightningSampler, vul::ImageLayout::ShaderReadOnlyOptimal)});
            auto updates = updateBuilder.create(raytraceDescriptorSet);
            device.updateDescriptorSets(updates);
        }

    };

    resizeDescriptorSets();

    enum class MaterialType : std::uint8_t {
        None = 0u,
        Lambertian = 1u,
        Metal = 2u,
        Dielectric = 3u
    };



//    std::vector host_material_ids = {
//            MaterialType::Lambertian,
//            MaterialType::Lambertian,
//            MaterialType::Dielectric,
//            MaterialType::Dielectric,
//            MaterialType::Metal
//    };
//    std::vector host_material_data = {
//            glm::vec4(0.8, 0.8, 0.0, 0.0),
//            glm::vec4(0.1, 0.2, 0.5, 1.5),
//            glm::vec4(1.0, 1.0, 1.0, 1.5),
//            glm::vec4(1.0, 1.0, 1.0, 1.5),
//            glm::vec4(0.8, 0.6, 0.2, 0.0)
//    };
//    std::vector host_sphere_data = {
//            Sphere{glm::vec3(0.0, -100.5, 1.0), 100},
//            Sphere{glm::vec3(0.0, 0.0, 1.0), 0.5},
//            Sphere{glm::vec3(-1.0, 0.0, 1.0), 0.5},
//            Sphere{glm::vec3(-1.0, 0.0, 1.0), -0.4},
//            Sphere{glm::vec3(1.0, 0.0, 1.0), 0.5}
//    };

    std::vector<MaterialType> host_material_ids;
    std::vector<glm::vec4> host_material_data;
    std::vector<vul::Sphere> host_sphere_data;
    std::vector<vul::PathEnd> host_path_data;


    auto ground_material = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);
    auto ground_type = MaterialType::Lambertian;
    host_material_ids.push_back(ground_type);
    host_material_data.push_back(ground_material);
    host_sphere_data.push_back({glm::vec3(0.0f,-1000.0f,0.0f), 1000.0f});
    host_path_data.push_back({glm::vec3(0.0), 0.0});
    int max_rad = 0;
    for (int a = -max_rad; a < max_rad; a++) {
        for (int b = -max_rad; b < max_rad; b++) {
            auto choose_mat = random_double();
            glm::vec3 center(a + (0.9f*random_float()), 0.2f, b + (0.9f*random_float()));

            if ((center - glm::vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {


                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_color() * random_color();
                    host_material_ids.push_back(MaterialType::Lambertian);
                    host_material_data.push_back(glm::vec4(albedo,0.0f));
                    host_sphere_data.push_back({center,  0.2f});
                    auto center2 = center + glm::vec3(0.0f, random_float(0.0f, 0.5f), 0.0f);
                    float speed = glm::distance(center, center2) / 1.0;
//                    speed = 0.0;
//                    center2 = center;
                    host_path_data.push_back({center2, speed});
                } else if (choose_mat < 0.95) {
                    //material
                    auto albedo = random_color(0.5f, 1.0f);
                    auto fuzz = random_float(0.0f, 0.5f);
                    host_material_ids.push_back(MaterialType::Metal);
                    host_material_data.push_back(glm::vec4(albedo,fuzz));
                    host_sphere_data.push_back({center,  0.2f});
                    host_path_data.push_back({glm::vec3(0.0), 0.0});
                } else {
                    // glass
                    auto albedo = glm::vec3(1.0f);
                    auto ir = 1.5f;
                    host_material_ids.push_back(MaterialType::Dielectric);
                    host_material_data.push_back(glm::vec4(albedo,ir));
                    host_sphere_data.push_back({center,  0.2f});
                    host_path_data.push_back({glm::vec3(0.0), 0.0});
                }
            }
        }
    }
    host_material_ids.push_back(MaterialType::Dielectric);
    host_material_data.push_back(glm::vec4(glm::vec3(1.0f),1.5f));
    host_sphere_data.push_back({glm::vec3(0.0f,1.0f,0.0f),  1.0f});
    host_path_data.push_back({glm::vec3(0.0), 0.0});

    host_material_ids.push_back(MaterialType::Lambertian);
    host_material_data.push_back(glm::vec4(glm::vec3(0.4f, 0.2f, 0.1f),0.0f));
    host_sphere_data.push_back({glm::vec3(-4.0f, 1.0f, 0.0f),  1.0f});
    host_path_data.push_back({glm::vec3(0.0), 0.0});

    host_material_ids.push_back(MaterialType::Metal);
    host_material_data.push_back(glm::vec4(glm::vec3(0.7f, 0.6f, 0.5f),0.0f));
    host_sphere_data.push_back({glm::vec3(4.0f, 1.0f, 0.0f),  1.0f});
    host_path_data.push_back({glm::vec3(0.0), 0.0});

    auto bvh = vul::create_bvh(host_sphere_data, host_path_data);



    auto device_material_ids = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_material_ids,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_material_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_material_data,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_sphere_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_sphere_data,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    auto device_path_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            host_path_data,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

//    auto device_bvh_data = allocator.createDeviceBuffer(
//            commandPool,
//            presentationQueue,
//            bvh.nodes,
//            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_bbox_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            bvh.bboxes,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_children_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            bvh.children,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();
    auto device_leaf_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            bvh.leaves,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    auto device_parent_data = allocator.createDeviceBuffer(
            commandPool,
            presentationQueue,
            bvh.parents,
            vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue();

    struct alignas(8) ViewState {
        std::uint64_t material_ids;
        std::uint64_t material_data;
        std::uint64_t sphere_data;
        std::uint64_t path_data;
        std::uint64_t bbox_data;
        std::uint64_t children_data;
        std::uint64_t leaf_data;
        std::uint64_t parent_data;
        std::uint32_t element_count;
        std::uint32_t image_width;

        std::uint32_t image_height;
        float focus_dist;

        float aperture;
        float fov;

        float exposure_time;
    };
    static_assert(sizeof(ViewState) == 64 + 16 + 16);
    ViewState view_state = {};
    view_state.material_ids = device_material_ids.getDeviceAddress();
    view_state.material_data = device_material_data.getDeviceAddress();
    view_state.sphere_data = device_sphere_data.getDeviceAddress();
    view_state.path_data = device_path_data.getDeviceAddress();
    view_state.bbox_data = device_bbox_data.getDeviceAddress();
    view_state.children_data = device_children_data.getDeviceAddress();
    view_state.leaf_data = device_leaf_data.getDeviceAddress();
    view_state.parent_data = device_parent_data.getDeviceAddress();
    view_state.element_count = static_cast<std::uint32_t>(host_material_ids.size());
    view_state.image_width = surface.getSwapchain()->getExtent().width;
    view_state.image_height = surface.getSwapchain()->getExtent().height;
    view_state.focus_dist = 10.0f;
    view_state.aperture = 0.1f;
    view_state.fov = glm::radians(90.0f);
    view_state.exposure_time = 1.0f;

    std::vector<vul::Buffer> device_view_states;
    for (std::size_t i = 0; i < swapchainSize; ++i) {
        device_view_states.push_back(
                allocator.createMappedCoherentBuffer(
                        view_state,
                        vul::BufferUsageFlagBits::ShaderDeviceAddressBit).assertValue());
    }

    std::uint64_t view_state_update_version = 0;
    std::vector<std::uint64_t> inflight_view_state_update_versions(swapchainSize,view_state_update_version);

    auto resizeDataBuffers= [&](){
        auto extent =surface.getSwapchain()->getExtent();
        view_state.image_width = extent.width;
        view_state.image_height = extent.height;
        for(auto& device_view_state : device_view_states){
            device_view_state.mappedCopyFrom(view_state);
        }
    };


    auto resizeWindow = [&resizeSwapchain, resizeImGuiFramebuffers = imguiRenderer.createResizeCallback(
            presentationQueue), &resizeDescriptorSets, &resizeDataBuffers]() {
        resizeSwapchain();
        resizeImGuiFramebuffers();
        resizeDescriptorSets();
        resizeDataBuffers();
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


            if(ImGui::InputFloat("focus_dist", &view_state.focus_dist, 0.01f, 1.0f)){
                view_state_updated = true;
            }
            if(ImGui::InputFloat("aperture", &view_state.aperture, 0.01f, 1.0f)){
                view_state_updated = true;
            }
            float fov = glm::degrees(view_state.fov);
            if(ImGui::SliderFloat("fov", &fov,0.0f,180.0f)){
                view_state.fov = glm::radians(fov);
                view_state_updated = true;
            }
            if(ImGui::InputFloat("exposure_time", &view_state.exposure_time, 0.01f, 1.0f)){
                view_state_updated = true;
            }


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

        if(view_state_updated){
            view_state_update_version +=1;
//            view_state_update_version %= swapchainSize;
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