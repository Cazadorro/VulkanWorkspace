//
// Created by Shae Bolt on 6/2/2021.
//

#ifndef VULKANWORKSPACE_DEBUGUTILS_H
#define VULKANWORKSPACE_DEBUGUTILS_H
#include "vul/instance.h"
#include "vul/enumsfwd.h"
#include "vul/expectedresult.h"
#include "vul/objecttypeutils.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <functional>

namespace vul{
    using DebugCallback = std::function<VkBool32(
            vul::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            uul::EnumFlags<vul::DebugUtilsMessageTypeFlagBitsEXT> messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData)>;
    class DebugCreateInfoBuilder;
    class DebugUtilsMessengerEXT{
    public:
        DebugUtilsMessengerEXT ( DebugUtilsMessengerEXT && rhs) noexcept;
        DebugUtilsMessengerEXT &  operator= ( DebugUtilsMessengerEXT && rhs) noexcept;
        DebugUtilsMessengerEXT ( const DebugUtilsMessengerEXT & ) = delete;
        DebugUtilsMessengerEXT & operator= ( const DebugUtilsMessengerEXT & ) = delete;
        ~DebugUtilsMessengerEXT();
        [[nodiscard]]
        VkDebugUtilsMessengerEXT get() const;
    private:
        friend DebugCreateInfoBuilder;
        DebugUtilsMessengerEXT();
        const Instance* m_pInstance = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        DebugCallback m_debugCallback  = nullptr;
        VkDebugUtilsMessengerEXT m_handle = VK_NULL_HANDLE;
        DebugUtilsMessengerEXT** m_ppthis = new DebugUtilsMessengerEXT*;
    };
    class DebugCreateInfoBuilder {
    public:
        explicit DebugCreateInfoBuilder(const Instance& instance, const VkAllocationCallbacks *pAllocator = nullptr);
        DebugCreateInfoBuilder& pNext(const void * value);
        DebugCreateInfoBuilder& messageSeverity(uul::EnumFlags<vul::DebugUtilsMessageSeverityFlagBitsEXT> value);
        DebugCreateInfoBuilder& messageType( uul::EnumFlags<vul::DebugUtilsMessageTypeFlagBitsEXT> value);
        //removed to force you to use better stuff instead
        //DebugCreateInfoBuilder& pfnUserCallback(PFN_vkDebugUtilsMessengerCallbackEXT value);
        DebugCreateInfoBuilder& pfnUserCallback(DebugCallback value);
        DebugCreateInfoBuilder& setDefaults();
        [[nodiscard]]
        ExpectedResult<DebugUtilsMessengerEXT> create() const;
        [[nodiscard]]
        const VkDebugUtilsMessengerCreateInfoEXT& getCreateInfo() const;

        [[nodiscard]]
        static VkDebugUtilsMessengerCreateInfoEXT createInstanceCallbackInfo(
                uul::EnumFlags<vul::DebugUtilsMessageSeverityFlagBitsEXT> messageSeverity,
                uul::EnumFlags<vul::DebugUtilsMessageTypeFlagBitsEXT> messageType, const void * pNext = nullptr);
    private:
        const Instance* m_pInstance = nullptr;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
        DebugCallback m_debugCallback = nullptr;
        VkDebugUtilsMessengerCreateInfoEXT m_createInfo = {};
    };
    //per object so we have access to device?
//    template<typename T>
//    void setDebugObjectName(T handle)
    VkBool32 defaultDebugMessengerCallback(
            vul::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            uul::EnumFlags<vul::DebugUtilsMessageTypeFlagBitsEXT> messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *callbackData);

    VkBool32 defaultDebugMessengerCallbackVk(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                             void *pUserData);

    template<typename T>
    VkDebugUtilsObjectNameInfoEXT createObjectNameInfo(T t, const std::string& string, void* pNext = nullptr){
        VkDebugUtilsObjectNameInfoEXT objectNameInfo = {};
        objectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        objectNameInfo.pNext = pNext;
        objectNameInfo.objectType = static_cast<VkObjectType>(vul::getObjectType<T>());
        objectNameInfo.objectHandle = reinterpret_cast<std::uintptr_t>(t);
        objectNameInfo.pObjectName = string.c_str();
        return objectNameInfo;
    }
}
#endif //VULKANWORKSPACE_DEBUGUTILS_H
