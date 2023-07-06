//
// Created by Shae Bolt on 6/5/2021.
//

#ifndef VULKANWORKSPACE_PHYSICALDEVICE_H
#define VULKANWORKSPACE_PHYSICALDEVICE_H

#include "vul/enumsfwd.h"
#include "vul/featuresfwd.h"
#include "vul/temparrayproxyfwd.h"
#include <uul/enumflagsfwd.h>
#include <gsl/span>
#include <vulkan/vulkan.h>
#include <unordered_set>
#include <vector>
#include <optional>
#include <string_view>
#include <string>
#include <cstdint>

//TODO need to take care of Surface when new actual object is made for it.
namespace vul {
    template<typename T>
    class ExpectedResult;
    class Instance;
    class Surface;
    class Features;
    class SurfaceFormat;
    class PhysicalDevice;
    class Device;
    class ExtensionsProperties{
    public:
        explicit ExtensionsProperties(std::vector<VkExtensionProperties> extensionProperties);
        bool supports(const gsl::span<const std::string> &extensions) const;
        bool supports(const gsl::span<const std::string_view> &extensions) const;
        bool supports(const gsl::span<const char * const> &extensions) const;
    private:
        std::vector<VkExtensionProperties> m_extensionProperties;
        std::unordered_set<std::string_view> m_extensionNameSet;
    };

    class QueueFamilyProperties{
    public:
        explicit QueueFamilyProperties(const PhysicalDevice& physicalDevice, std::vector<VkQueueFamilyProperties> queueFamilyProperties);
        [[nodiscard]]
        std::vector<std::uint32_t> getQueueFamilyIndexes(uul::EnumFlags<QueueFlagBits> queueFamilyBits) const;
        [[nodiscard]]
        std::vector<std::uint32_t> getPresentationQueueFamilyIndexes(const vul::Surface& surface, uul::EnumFlags<QueueFlagBits> queueFamilyBits) const;
        [[nodiscard]]
        std::vector<std::uint32_t> getPresentationQueueFamilyIndexes(const vul::Surface& surface) const;
        [[nodiscard]]
        std::optional<std::uint32_t> findMinimumQueueFamilyIndex(const gsl::span<std::uint32_t>& queueFamilyIndexes) const;
        [[nodiscard]]
        std::optional<std::uint32_t> calcMinimumQueueFamilyIndex(uul::EnumFlags<QueueFlagBits> queueFamilyBits);
        [[nodiscard]]
        std::optional<std::uint32_t> calcMinimumPresentationQueueFamilyIndex(const vul::Surface& surface, uul::EnumFlags<QueueFlagBits> queueFamilyBits);
        [[nodiscard]]
        bool contains(const std::vector<uul::EnumFlags<QueueFlagBits>>& queueFamilies) const;
        [[nodiscard]]
        bool contains(uul::EnumFlags<QueueFlagBits> queueFamilies) const;
        [[nodiscard]]
        bool contains(const vul::Surface& surface, uul::EnumFlags<QueueFlagBits> queueFamilyBits) const;
    private:
        const PhysicalDevice* m_physicalDevice = nullptr;
        std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
    };

    class MemoryProperties{
    public:
        explicit MemoryProperties(VkPhysicalDeviceMemoryProperties memoryProperties);
        [[nodiscard]]
        std::optional<std::uint32_t> findMemoryType(const VkMemoryRequirements & memoryRequirements,
                                                    uul::EnumFlags<MemoryPropertyFlagBits> properties) const;
        [[nodiscard]]
        std::optional<std::uint32_t> findMemoryType(const gsl::span<const VkMemoryRequirements> &memoryRequirementsList,
                                                    uul::EnumFlags<MemoryPropertyFlagBits> properties) const;
        [[nodiscard]]
        const VkPhysicalDeviceMemoryProperties& get()const;
    private:
        VkPhysicalDeviceMemoryProperties m_memoryProperties;
    };

    struct SingleQueueCreateInfo{
        std::uint32_t queueFamilyIndex;
        float priority = 1.0;
        explicit SingleQueueCreateInfo(std::uint32_t queueFamilyIndex, float priority = 1.0);
    };

    class PhysicalDevice {
    public:
        const static std::array<vul::Format,5> depthFormats;
        PhysicalDevice() = default;
        [[nodiscard]]
        MemoryProperties getMemoryProperties() const;
        [[nodiscard]]
        QueueFamilyProperties getQueueFamilyProperties() const;
        [[nodiscard]]
        ExtensionsProperties getExtensionProperties() const;

        [[nodiscard]]
        VkPhysicalDeviceLimits getLimits() const;

        [[nodiscard]]
        vul::PhysicalDeviceType getType() const;

        [[nodiscard]]
        VkPhysicalDeviceProperties getProperties() const;

        [[nodiscard]]
        vul::Features getFeatures() const;
        [[nodiscard]]
        bool supportsPresentationTo(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const;
        [[nodiscard]]
        VkFormatProperties getFormatProperties(vul::Format format) const;
        [[nodiscard]]
        std::vector<vul::Format> filterOptimalTilingFormat(const gsl::span<const vul::Format> &candidates,
                                                        uul::EnumFlags<FormatFeatureFlagBits> features) const;
        [[nodiscard]]
        std::vector<vul::Format> filterLinearTilingFormat(const gsl::span<const vul::Format> &candidates,
                                                       uul::EnumFlags<FormatFeatureFlagBits> features) const;
        [[nodiscard]]
        std::vector<vul::Format> filterBufferFormat(const gsl::span<const vul::Format> &candidates,
                                                 uul::EnumFlags<FormatFeatureFlagBits> features) const;
        [[nodiscard]]
        std::vector<vul::Format> filterDepthFormat(uul::EnumFlags<FormatFeatureFlagBits> additionalFeatures) const;
        [[nodiscard]]
        std::vector<vul::Format> filterDepthFormat() const;

        //post: if multiple queue famlies are the same in queueCreateInfos, still create that total number of queues.
        [[nodiscard]]
        ExpectedResult<Device> createDevice(const gsl::span<const SingleQueueCreateInfo>& queueCreateInfos,
                              const gsl::span<const char * const> &extensions,
                              const Features &features,
                              const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Device> createDevice(const gsl::span<const SingleQueueCreateInfo>& queueCreateInfos,
                              const gsl::span<std::string> &extensions,
                              const Features &features,
                              const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Device> createDevice(const gsl::span<const SingleQueueCreateInfo>& queueCreateInfos,
                              const gsl::span<std::string_view> &extensions,
                              const Features &features,
                              const VkAllocationCallbacks *pAllocator = nullptr) const;

        //TODO make version for string view string as well? or make sure anything compatible with char * ?
        [[nodiscard]]
        ExpectedResult<Device> createDevice(const vul::Surface& surface,
                                            TempArrayProxy<const uul::EnumFlags<vul::QueueFlagBits>> presentationQueueFlagMasks,
                                            TempArrayProxy<const uul::EnumFlags<vul::QueueFlagBits>> commandQueueFlagMasks,
                                            const gsl::span<const char * const> &extensions,
                                            const Features &features,
                                            const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        ExpectedResult<Device> createDevice(
                TempArrayProxy<const uul::EnumFlags<vul::QueueFlagBits>> commandQueueFlagMasks,
                const gsl::span<const char * const> &extensions,
                const Features &features,
                const VkAllocationCallbacks *pAllocator = nullptr) const;
        [[nodiscard]]
        VkPhysicalDevice get() const;
    private:
        VkPhysicalDevice m_handle = VK_NULL_HANDLE;
    };
    static_assert(sizeof(PhysicalDevice) == sizeof(VkPhysicalDevice));


    std::optional<vul::PhysicalDevice>
    pickPhysicalDevice(const vul::Instance &instance, const vul::Surface &surface,
                       const vul::Features &features,
                       const gsl::span<const char *const> &deviceExtensions,
                       const vul::SurfaceFormat &surfaceFormat,
                       vul::PresentModeKHR presentationMode);
    std::optional<vul::PhysicalDevice>
    pickPhysicalDevice(const vul::Instance &instance,
                       const vul::Features &features,
                       const gsl::span<const char *const> &deviceExtensions);
}


#endif //VULKANWORKSPACE_PHYSICALDEVICE_H
