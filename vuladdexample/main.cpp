//
// Created by Shae Bolt on 6/5/2021.
//

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
#include <iostream>
#include <optional>
#include <chrono>
#include <thread>
#include <random>


int main() {

    auto instanceExtensions = ranges::views::concat(vul::Instance::debugUtilsExtensions) |
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
            VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
    };
    vul::Device device;
    auto physicalDeviceOpt = vul::pickPhysicalDevice(instance, features, deviceExtensions);
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

    auto computeFamilyIndexResult = queueFamilyProperties.getMinimumQueueFamilyIndex(
            computeQueueFamilyIndexes);
    if (!computeFamilyIndexResult.has_value()) {
        throw std::runtime_error("failed to find a suitable compute queue!");
    }
    std::uint32_t computeQueueIndex = computeFamilyIndexResult.value();
    std::vector<vul::SingleQueueCreateInfo> queueCreateInfos = {vul::SingleQueueCreateInfo{computeQueueIndex}};
    device = physicalDevice.createDevice(queueCreateInfos, deviceExtensions,
                                         features).assertValue();

    vul::Queue computeQueue = device.getQueueAt(0).value();


    auto allocator = vul::VmaAllocator::create(instance, physicalDevice,
                                               device,
                                               VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT).assertValue();

    auto computeFinishedSemaphore = device.createTimelineSemaphore(0).assertValue();

    std::uint64_t frame_counter = 0;


    auto commandPool = device.createCommandPool(
            computeQueueIndex,
            vul::CommandPoolCreateFlagBits::ResetCommandBufferBit).assertValue();



    auto commandBuffer = commandPool.createPrimaryCommandBuffer().assertValue();

    struct alignas(8) PushConstantBlock {
        std::uint32_t u_width;
        std::uint32_t u_height;
        std::uint64_t u_matrix;
        float u_addend;
    };
    static_assert(sizeof(PushConstantBlock) == 24);


    std::uint32_t width = 64;
    std::uint32_t height = 64;
    std::vector<float> host_matrix(width*height, 1.0);
    std::size_t host_matrix_size_bytes = host_matrix.size() * sizeof(float);
    vul::Buffer host_staging_matrix = allocator.createStagingBuffer(host_matrix_size_bytes,vul::BufferUsageFlagBits::TransferDstBit).assertValue();
    vul::Buffer device_matrix = allocator.createDeviceBuffer(commandPool, computeQueue, host_matrix,
                                                             vul::BufferUsageFlagBits::ShaderDeviceAddressBit
                                                             | vul::BufferUsageFlagBits::TransferSrcBit).assertValue();


    auto computePipelineLayout = device.createPipelineLayout(
            vul::TempArrayProxy<const vul::DescriptorSetLayout *>{},
            VkPushConstantRange{
                    static_cast<VkShaderStageFlags>(vul::ShaderStageFlagBits::ComputeBit),
                    0,
                    sizeof(PushConstantBlock)
            }).assertValue();
    auto computeBuilder = vul::ComputePipelineBuilder(device);
    std::vector<std::uint32_t> block_size = {32,32,1};
    std::vector<std::uint32_t> grid_size{static_cast<std::uint32_t>(std::ceil(
            width / float(block_size[0]))),
                   static_cast<std::uint32_t>(std::ceil(
                           height / float(block_size[1]))),1};
    fmt::print("Block Size : {},{},{}\n", block_size[0], block_size[1], block_size[2]);
    fmt::print("Grid Size : {},{},{}\n", grid_size[0], grid_size[1], grid_size[2]);
    vul::ComputePipeline computePipeline;
    {
        auto computeShader = device.createShaderModule(
                vul::readSPIRV("spirv/add.comp.spv")).assertValue();
        VkSpecializationMapEntry local_size_x_id = {0u, 0u, 4u};
        VkSpecializationMapEntry local_size_y_id = {1u, 4u, 4u};
        VkSpecializationMapEntry local_size_z_id = {2u, 8u, 4u};
        std::vector<VkSpecializationMapEntry> blockSizeMapEntries = {
                local_size_x_id,
                local_size_y_id,
                local_size_z_id
        };
        VkSpecializationInfo blockSizeSpecializationInfo = {};
        blockSizeSpecializationInfo.mapEntryCount = static_cast<std::uint32_t>(blockSizeMapEntries.size());
        blockSizeSpecializationInfo.pMapEntries = blockSizeMapEntries.data();
        blockSizeSpecializationInfo.dataSize = block_size.size() * sizeof(std::uint32_t);
        blockSizeSpecializationInfo.pData = block_size.data();
        computeBuilder.setShaderCreateInfo(
                computeShader.createComputeStageInfo(&blockSizeSpecializationInfo));
        computeBuilder.setPipelineLayout(computePipelineLayout);
        computePipeline = computeBuilder.create().assertValue();
    }

    PushConstantBlock pushConstantBlock = {};
    pushConstantBlock.u_width = width;
    pushConstantBlock.u_height = height;
    pushConstantBlock.u_matrix = device_matrix.getDeviceAddress();
    pushConstantBlock.u_addend = 10.f;
    {
        commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
        commandBuffer.bindPipeline(computePipeline);
        {
            commandBuffer.pushConstants(computePipelineLayout,
                                        vul::ShaderStageFlagBits::ComputeBit,
                                        pushConstantBlock);
            commandBuffer.dispatch(grid_size[0], grid_size[1], grid_size[2]);

        }
        auto computeTransferBarrier = vul::createMemoryBarrier(
                vul::PipelineStageFlagBits2KHR::ComputeShaderBit,
                vul::AccessFlagBits2KHR::ShaderWriteBit,
                vul::PipelineStageFlagBits2KHR::TransferBit,
                vul::AccessFlagBits2KHR::TransferReadBit);
        auto dependencyInfo = vul::createDependencyInfo(
                computeTransferBarrier, {}, {});
        commandBuffer.pipelineBarrier(dependencyInfo);
        commandBuffer.copyBuffer(device_matrix, host_staging_matrix, 0);
        commandBuffer.end();
    }
    std::uint64_t semaphoreDoneValue = 1;
    VkSemaphoreSubmitInfoKHR computeExecutionDoneInfo = computeFinishedSemaphore.createSubmitInfo(semaphoreDoneValue, vul::PipelineStageFlagBits2KHR::AllCommandsBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    VkSubmitInfo2KHR submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
    submitInfo.pNext = nullptr;
    submitInfo.flags = 0;
    submitInfo.waitSemaphoreInfoCount = 0;
    submitInfo.pWaitSemaphoreInfos = nullptr;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferInfo;
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos =&computeExecutionDoneInfo;

    computeQueue.submit(submitInfo);
    computeFinishedSemaphore.wait(semaphoreDoneValue);
    std::cout << "matrix size = " << host_matrix.size() << std::endl;
    void * memory = host_staging_matrix.mapMemory();
    for (const auto& value : gsl::span(reinterpret_cast<float*>(memory), host_matrix.size())) {
        std::cout << value << std::endl;
    }

    return 0;
}