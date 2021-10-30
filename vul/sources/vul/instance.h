//
// Created by Shae Bolt on 6/2/2021.
//

#ifndef VULKANWORKSPACE_INSTANCE_H
#define VULKANWORKSPACE_INSTANCE_H

#include <vulkan/vulkan.h>
#include <gsl/span>
#include <array>
#include "surface.h"


namespace vul {
    template<typename T>
    class ExpectedResult;
    //TODO add validated stuff.
    class PhysicalDevice;

    class Instance {
    public:
        static const std::vector<const char *> validationLayers;
        static const std::vector<const char *> debugUtilsExtensions;


        Instance() = default;
        //TODO add way to create instance with validation, ie static validated
        Instance(const VkApplicationInfo &appInfo,
                 const gsl::span<const char *const> &enabledLayers,
                 const gsl::span<const char *const> &enabledExtensions,
                 const void *pNext = nullptr,
                 const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        static ExpectedResult<Instance>
        create(const VkApplicationInfo &appInfo,
               const gsl::span<const char *const> &enabledLayers,
               const gsl::span<const char *const> &enabledExtensions,
               const void *pNext = nullptr,
               const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        static ExpectedResult<Instance> withDebugCallback(const VkApplicationInfo &appInfo,
                                          const gsl::span<const char *const> &enabledLayers = validationLayers,
                                          const gsl::span<const char *const> &enabledExtensions = debugUtilsExtensions,
                                          const void *pNext = nullptr,
                                          const VkAllocationCallbacks *pAllocator = nullptr);

        [[nodiscard]]
        PFN_vkVoidFunction getProcAddr(const std::string &function_name) const;

        [[nodiscard]]
        VkInstance get() const noexcept;

        [[nodiscard]]
        std::vector<vul::PhysicalDevice> enumeratePhysicalDevices() const;

        ~Instance();

        Instance(Instance &&rhs) noexcept;

//        was noexcept?
        Instance &operator=(Instance &&rhs) noexcept;

        Instance(const Instance &rhs) = delete;

        Instance &operator=(const Instance &rhs) = delete;

    private:
        VkInstance m_handle = VK_NULL_HANDLE;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
    };


    [[nodiscard]]
    std::vector<VkLayerProperties> enumerateLayerProperties();

    [[nodiscard]]
    std::vector<VkExtensionProperties> enumerateExtensionProperties();

    [[nodiscard]]
    bool extensionsSupported(const gsl::span<const char *const> &extensions);

    [[nodiscard]]
    bool layersSupported(const gsl::span<const char *const> &layers);
}


#endif //VULKANWORKSPACE_INSTANCE_H
