//
// Created by Shae Bolt on 6/5/2021.
//

#include "vul/surface.h"
#include "vul/device.h"
#include "vul/physicaldevice.h"
#include "vul/expectedresult.h"
#include "vul/bitmasks.h"
#include "vul/enums.h"
#include "vul/features.h"

#include <fmt/core.h>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <bitset>
#include <unordered_map>


std::optional<std::uint32_t> vul::MemoryProperties::findMemoryType(
        const gsl::span<const VkMemoryRequirements> &memoryRequirementsList,
        vul::MemoryPropertyBitMask properties) const {
    for (uint32_t memoryTypeIndex = 0; memoryTypeIndex <
                                       m_memoryProperties.memoryTypeCount; ++memoryTypeIndex) {
        bool typeSupported = ranges::all_of(memoryRequirementsList,
                                            [memoryTypeIndex](
                                                    const auto &memoryRequirements) {
                                                return std::bitset<32>(
                                                        memoryRequirements.memoryTypeBits)[memoryTypeIndex];
                                            });
        if (typeSupported) {
            bool propertiesSupported = vul::MemoryPropertyBitMask(
                    m_memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags).contains(
                    properties);
            if (propertiesSupported) {
                return memoryTypeIndex;
            }
        }
    }
    return std::nullopt;
}

std::optional<std::uint32_t> vul::MemoryProperties::findMemoryType(
        const VkMemoryRequirements &memoryRequirements,
        vul::MemoryPropertyBitMask properties) const {
    return findMemoryType(gsl::span(&memoryRequirements, 1), properties);
}

vul::MemoryProperties::MemoryProperties(
        VkPhysicalDeviceMemoryProperties memoryProperties) : m_memoryProperties(
        memoryProperties) {

}

const VkPhysicalDeviceMemoryProperties &vul::MemoryProperties::get() const {
    return m_memoryProperties;
}


vul::QueueFamilyProperties::QueueFamilyProperties(
        const PhysicalDevice &physicalDevice,
        std::vector<VkQueueFamilyProperties> queueFamilyProperties)
        : m_physicalDevice(&physicalDevice),
          m_queueFamilyProperties(std::move(queueFamilyProperties)) {

}

std::vector<std::uint32_t> vul::QueueFamilyProperties::getQueueFamilyIndexes(
        vul::QueueBitMask queueFamilyBits) const {
    auto notHasQueueBits = [queueFamilyBits](const auto &props) {
        return !(props.queueCount > 0 &&
               vul::QueueBitMask(props.queueFlags).contains(
                       queueFamilyBits));
    };
    auto getEnumeratedU32 = [](const auto &pair) {
        return static_cast<std::uint32_t>(pair.first);
    };
    using namespace ranges;
    return m_queueFamilyProperties
           | views::remove_if(notHasQueueBits)
           | views::enumerate
           | views::transform(getEnumeratedU32)
           | to<std::vector>();
}

std::vector<std::uint32_t>
vul::QueueFamilyProperties::getPresentationQueueFamilyIndexes(
        const vul::Surface &surface, vul::QueueBitMask queueFamilyBits) const {
    auto notHasPresentationQueueBits =
            [&physicalDevice = *m_physicalDevice, &surface,
                    queueFamilyBits](const auto &pair) {
                auto queueFamilyIndex = pair.first;
                const auto &props = pair.second;
                return !(props.queueCount > 0 &&
                       vul::QueueBitMask(props.queueFlags).contains(
                               queueFamilyBits) &&
                       surface.isSupportedBy(physicalDevice, queueFamilyIndex));
            };
    auto getEnumeratedU32 = [](const auto &pair) {
        return static_cast<std::uint32_t>(pair.first);
    };
    using namespace ranges;
    return m_queueFamilyProperties
           | views::enumerate
           | views::remove_if(notHasPresentationQueueBits)
           | views::transform(getEnumeratedU32)
           | to<std::vector>();
}

std::vector<std::uint32_t>
vul::QueueFamilyProperties::getPresentationQueueFamilyIndexes(
        const vul::Surface &surface) const {
    return getPresentationQueueFamilyIndexes(surface, vul::QueueBitMask{});
}


std::optional<std::uint32_t>
vul::QueueFamilyProperties::getMinimumQueueFamilyIndex(
        const gsl::span<std::uint32_t> &queueFamilyIndexes) const {
    if (queueFamilyIndexes.empty()) {
        return std::nullopt;
    } else {
        auto minQueueFamilyFlags = [&queueFamilyProperties = m_queueFamilyProperties](
                std::uint32_t lhsIdx,
                std::uint32_t rhsIdx) {
            auto lhsQueueFlags = queueFamilyProperties[lhsIdx].queueFlags;
            auto rhsQueueFlags = queueFamilyProperties[rhsIdx].queueFlags;
            return std::bitset<32>(lhsQueueFlags).count() <
                   std::bitset<32>(rhsQueueFlags).count();
        };
        return ranges::min(queueFamilyIndexes, minQueueFamilyFlags);
    }
}

bool vul::QueueFamilyProperties::contains(
        const std::vector<vul::QueueBitMask> &queueFamilies) const {
    for (const auto &queueFamily: queueFamilies) {
        if (ranges::find_if(m_queueFamilyProperties, [queueFamily](
                const VkQueueFamilyProperties &props) {
            return props.queueCount > 0 &&
                   vul::QueueBitMask(props.queueFlags).contains(queueFamily);
        }) == m_queueFamilyProperties.end()) {
            return false;
        }
    }
    return true;
}

bool
vul::QueueFamilyProperties::contains(vul::QueueBitMask queueFamilies) const {
    return contains(std::vector<vul::QueueBitMask>{queueFamilies});
}

bool vul::QueueFamilyProperties::contains(const vul::Surface &surface,
                                          vul::QueueBitMask queueFamilyBits) const {
    using namespace ranges;
    auto enumerated = m_queueFamilyProperties | views::enumerate;
    if (ranges::find_if(enumerated,
                        [&physicalDevice = *m_physicalDevice, queueFamilyBits, &surface](
                                const auto &pair) {
                            auto queueFamilyIndex = pair.first;
                            const auto &props = pair.second;
                            return props.queueCount > 0 &&
                                   vul::QueueBitMask(props.queueFlags).contains(
                                           queueFamilyBits) &&
                                   surface.isSupportedBy(physicalDevice,
                                                         queueFamilyIndex);;
                        }) == enumerated.end()) {
        return false;
    }
    return true;
}


vul::ExtensionsProperties::ExtensionsProperties(
        std::vector<VkExtensionProperties> extensionProperties)
        : m_extensionProperties(std::move(extensionProperties)) {
    m_extensionNameSet = m_extensionProperties | ranges::views::transform(
            [](const VkExtensionProperties &properties) {
                return std::string_view(properties.extensionName);
            }) | ranges::to<std::unordered_set>();
}

bool vul::ExtensionsProperties::supports(
        const gsl::span<const std::string> &extensions) const {
    using namespace ranges;
    return accumulate(extensions | views::transform(
            [&extensionNameSet = m_extensionNameSet](const auto &extension) {
                return ranges::contains(extensionNameSet, extension) ? 1u : 0u;
            }), 0u) == extensions.size();
}

bool vul::ExtensionsProperties::supports(
        const gsl::span<const std::string_view> &extensions) const {
    using namespace ranges;
    return accumulate(extensions | views::transform(
            [&extensionNameSet = m_extensionNameSet](const auto &extension) {
                return ranges::contains(extensionNameSet, extension) ? 1u : 0u;
            }), 0u) == extensions.size();
}

bool vul::ExtensionsProperties::supports(
        const gsl::span<const char *const> &extensions) const {
    using namespace ranges;
    return accumulate(extensions | views::transform(
            [&extensionNameSet = m_extensionNameSet](const auto &extension) {
                return ranges::contains(extensionNameSet,
                                        std::string(extension)) ? 1u : 0u;
            }), 0u) == extensions.size();
}


vul::MemoryProperties vul::PhysicalDevice::getMemoryProperties() const {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_handle, &memoryProperties);
    return vul::MemoryProperties(memoryProperties);
}

vul::QueueFamilyProperties
vul::PhysicalDevice::getQueueFamilyProperties() const {
    std::uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount,
                                             nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount,
                                             queueFamilies.data());
    return vul::QueueFamilyProperties{*this, queueFamilies};
}

vul::ExtensionsProperties vul::PhysicalDevice::getExtensionProperties() const {
    std::uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extensionCount,
                                         nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extensionCount,
                                         availableExtensions.data());
    return vul::ExtensionsProperties{availableExtensions};
}

VkPhysicalDevice vul::PhysicalDevice::get() const {
    return m_handle;
}

bool vul::PhysicalDevice::supportsPresentationTo(
        uint32_t queueFamilyIndex, VkSurfaceKHR surface) const {
    VkBool32 supportsPresentationToSurface = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, queueFamilyIndex,
                                         surface,
                                         &supportsPresentationToSurface);
    return supportsPresentationToSurface;
}


const std::array<vul::Format, 5> vul::PhysicalDevice::depthFormats = {
        vul::Format::D32Sfloat,
        vul::Format::D32SfloatS8Uint,
        vul::Format::D24UnormS8Uint,
        vul::Format::D16Unorm,
        vul::Format::D16UnormS8Uint
};

VkFormatProperties
vul::PhysicalDevice::getFormatProperties(vul::Format format) const {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_handle, vul::get(format),
                                        &formatProperties);
    return formatProperties;
}

std::vector<vul::Format> vul::PhysicalDevice::filterOptimalTilingFormat(
        const gsl::span<const vul::Format> &candidates,
        vul::FormatFeatureBitMask features) const {
    return candidates | ranges::views::remove_if(
            [&physicalDevice = *this, features](vul::Format format) {
                auto formatProperties = physicalDevice.getFormatProperties(
                        format);
                auto optimalTilingFeatures = static_cast<FormatFeatureBitMask>(formatProperties.optimalTilingFeatures);
                return !optimalTilingFeatures.contains(features);
            }) | ranges::to<std::vector>();
}

std::vector<vul::Format> vul::PhysicalDevice::filterLinearTilingFormat(
        const gsl::span<const vul::Format> &candidates,
        vul::FormatFeatureBitMask features) const {
    return candidates | ranges::views::remove_if(
            [&physicalDevice = *this, features](vul::Format format) {
                auto formatProperties = physicalDevice.getFormatProperties(
                        format);
                auto linearTilingFeatures = static_cast<FormatFeatureBitMask>(formatProperties.linearTilingFeatures);
                return !linearTilingFeatures.contains(features);
            }) | ranges::to<std::vector>();
}

std::vector<vul::Format>
vul::PhysicalDevice::filterBufferFormat(
        const gsl::span<const vul::Format> &candidates,
        vul::FormatFeatureBitMask features) const {
    return candidates | ranges::views::remove_if(
            [&physicalDevice = *this, features](vul::Format format) {
                auto formatProperties = physicalDevice.getFormatProperties(
                        format);
                auto bufferFeatures = static_cast<FormatFeatureBitMask>(formatProperties.bufferFeatures);
                return !bufferFeatures.contains(features);
            }) | ranges::to<std::vector>();
}

std::vector<vul::Format> vul::PhysicalDevice::filterDepthFormat(
        vul::FormatFeatureBitMask additionalFeatures) const {
    return filterOptimalTilingFormat(depthFormats,
                                     vul::FormatFeatureFlagBits::DepthStencilAttachmentBit |
                                     additionalFeatures);
}

std::vector<vul::Format> vul::PhysicalDevice::filterDepthFormat() const {
    return filterDepthFormat(vul::FormatFeatureBitMask{});
}

vul::Features vul::PhysicalDevice::getFeatures() const {
    return vul::Features(m_handle);
}

vul::ExpectedResult<vul::Device> vul::PhysicalDevice::createDevice(
        const gsl::span<const SingleQueueCreateInfo> &queueCreateInfos,
        const gsl::span<const char * const> &extensions,
        const vul::Features &features,
        const VkAllocationCallbacks *pAllocator) const {
    std::unordered_map<std::uint32_t, std::vector<float>> indexPriorityMap;
    std::vector<QueueFamilyIndexMapping> queueFamilyMappings;
    queueFamilyMappings.reserve(queueCreateInfos.size());
    for (const auto &queueInfo : queueCreateInfos) {
        if (indexPriorityMap.find(queueInfo.queueFamilyIndex) ==
            indexPriorityMap.end()) {
            indexPriorityMap[queueInfo.queueFamilyIndex] = {};
        }
        indexPriorityMap[queueInfo.queueFamilyIndex].push_back(
                queueInfo.priority);
        queueFamilyMappings.push_back({queueInfo.queueFamilyIndex,
                                       static_cast<std::uint32_t>(
                                               indexPriorityMap[queueInfo.queueFamilyIndex].size() -
                                               1)});
    }
    std::vector<VkDeviceQueueCreateInfo> multiQueueCreateInfos;
    multiQueueCreateInfos.reserve(indexPriorityMap.size());
    for (const auto&[queueFamilyIndex, priorities] : indexPriorityMap) {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = queueFamilyIndex;
        createInfo.queueCount = priorities.size();
        createInfo.pQueuePriorities = priorities.data();
        multiQueueCreateInfos.push_back(createInfo);
    }
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(multiQueueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = multiQueueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    deviceCreateInfo.pEnabledFeatures = nullptr;
    deviceCreateInfo.pNext = &(features.get());
    deviceCreateInfo.flags = 0;

    VkDevice device;
    auto result = Result(
            vkCreateDevice(m_handle, &deviceCreateInfo, pAllocator, &device));
    return {result, Device(*this, device, queueFamilyMappings, pAllocator)};
}

vul::ExpectedResult<vul::Device> vul::PhysicalDevice::createDevice(
        const gsl::span<const SingleQueueCreateInfo> &queueCreateInfo,
        const gsl::span<std::string> &extensions, const vul::Features &features,
        const VkAllocationCallbacks *pAllocator) const {
    using namespace ranges;
    auto stringExtensions =
            extensions | views::transform([](const auto &value) {
                return value.c_str();
            }) | to<std::vector>();
    return createDevice(queueCreateInfo, stringExtensions, features,
                        pAllocator);
}

vul::ExpectedResult<vul::Device> vul::PhysicalDevice::createDevice(
        const gsl::span<const SingleQueueCreateInfo> &queueCreateInfo,
        const gsl::span<std::string_view> &extensions,
        const vul::Features &features,
        const VkAllocationCallbacks *pAllocator) const {
    using namespace ranges;
    auto stringExtensions =
            extensions | views::transform([](const auto &value) {
                return std::string(value);
            }) | to<std::vector>();
    return createDevice(queueCreateInfo, stringExtensions, features,
                        pAllocator);
}

VkPhysicalDeviceLimits vul::PhysicalDevice::getLimits() const {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_handle, &properties);
    return properties.limits;
}


vul::SingleQueueCreateInfo::SingleQueueCreateInfo(
        std::uint32_t queueFamilyIndex_t, float priority_t) : queueFamilyIndex(
        queueFamilyIndex_t), priority(priority_t) {

}
