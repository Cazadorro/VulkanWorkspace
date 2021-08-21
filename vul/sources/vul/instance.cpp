//
// Created by Shae Bolt on 6/2/2021.
//

#include "vul/instance.h"
#include "vul/physicaldevice.h"
#include "vul/debugutils.h"
#include "vul/bitmasks.h"
#include "vul/enums.h"
#include <range/v3/view/any_view.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/none_of.hpp>

const std::vector<const char *> vul::Instance::validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> vul::Instance::debugUtilsExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

PFN_vkVoidFunction
vul::Instance::getProcAddr(const std::string &function_name) const {
    return vkGetInstanceProcAddr(m_handle, function_name.c_str());
}

VkInstance vul::Instance::get() const noexcept {
    return m_handle;
}

vul::Instance::Instance(const VkApplicationInfo &appInfo,
                        const gsl::span<const char *const> &enabledLayers,
                        const gsl::span<const char *const> &enabledExtensions,
                        const void *pNext,
                        const VkAllocationCallbacks *pAllocator) : m_pAllocator(
        pAllocator) {
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = enabledLayers.size();
    createInfo.ppEnabledLayerNames = enabledLayers.data();
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    auto result = static_cast<Result>(vkCreateInstance(&createInfo,
                                                       m_pAllocator,
                                                       &m_handle));
    if (result != Result::Success) {
        fmt::print(stderr, "Error: {}\n", vul::to_string(result));
        std::terminate();
    }
}

vul::ExpectedResult<vul::Instance> vul::Instance::withDebugCallback(const VkApplicationInfo &appInfo,
                                               const gsl::span<const char *const> &enabledLayers,
                                               const gsl::span<const char *const> &enabledExtensions,
                                               const void *pNext,
                                               const VkAllocationCallbacks *pAllocator) {
    auto messageSeverityFlags =
            DebugUtilsMessageSeverityFlagBitsEXT::VerboseBit |
            DebugUtilsMessageSeverityFlagBitsEXT::WarningBit |
            DebugUtilsMessageSeverityFlagBitsEXT::ErrorBit;
    auto messageTypeFlags =
            DebugUtilsMessageTypeFlagBitsEXT::GeneralBit |
            DebugUtilsMessageTypeFlagBitsEXT::ValidationBit |
            DebugUtilsMessageTypeFlagBitsEXT::PerformanceBit;
    auto debugCreateInfo = DebugCreateInfoBuilder::createInstanceCallbackInfo(
            messageSeverityFlags, messageTypeFlags, pNext);
    return create(appInfo, enabledLayers, enabledExtensions, &debugCreateInfo,
                    pAllocator);
}

vul::Instance::~Instance() {
    vkDestroyInstance(m_handle, m_pAllocator);
}

vul::Instance::Instance(vul::Instance &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
    swap(m_pAllocator, rhs.m_pAllocator);
}

vul::Instance &vul::Instance::operator=(vul::Instance &&rhs) noexcept {
    using std::swap;
    swap(m_handle, rhs.m_handle);
    swap(m_pAllocator, rhs.m_pAllocator);
    return *this;
}

std::vector<vul::PhysicalDevice> vul::Instance::enumeratePhysicalDevices() const {
    std::uint32_t deviceCount;
    vkEnumeratePhysicalDevices(m_handle, &deviceCount, nullptr);
    std::vector<vul::PhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_handle, &deviceCount, reinterpret_cast<VkPhysicalDevice*>(devices.data()));
    return devices;
}

vul::ExpectedResult<vul::Instance>
vul::Instance::create(const VkApplicationInfo &appInfo,
                      const gsl::span<const char *const> &enabledLayers,
                      const gsl::span<const char *const> &enabledExtensions,
                      const void *pNext,
                      const VkAllocationCallbacks *pAllocator) {
    Instance instance;
    instance.m_pAllocator = pAllocator;
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = pNext;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = enabledLayers.size();
    createInfo.ppEnabledLayerNames = enabledLayers.data();
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    //probably could check success before returning, saves space, but w/e.
    auto result = static_cast<Result>(vkCreateInstance(&createInfo,
                                                       instance.m_pAllocator,
                                                       &instance.m_handle));
    return vul::ExpectedResult<vul::Instance>(result, std::move(instance));
}

std::vector<VkLayerProperties> vul::enumerateLayerProperties() {
    std::uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    return layerProperties;
}

std::vector<VkExtensionProperties> vul::enumerateExtensionProperties() {
    std::uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensionProperties.data());
    return extensionProperties;
}

bool vul::extensionsSupported(const gsl::span<const char * const> &extensions) {
    std::vector<VkExtensionProperties> availableExtensions = vul::enumerateExtensionProperties();
    for (const char *extensionsName : extensions) {
        if(ranges::none_of(availableExtensions, [&extensionsName](const auto &supportedExtensionProperties){
            return std::string(extensionsName) == std::string(supportedExtensionProperties.extensionName);
        })){
            return false;
        }
    }
    return true;
}

bool vul::layersSupported(const gsl::span<const char * const> &layers) {
    std::vector<VkLayerProperties> availableLayers = vul::enumerateLayerProperties();
    for (const char *layerName : layers) {
        if(ranges::none_of(availableLayers, [&layerName](const auto &supportedLayerProperties){
            return std::string(layerName) == std::string(supportedLayerProperties.layerName);
        })){
            return false;
        }
    }
    return true;
}