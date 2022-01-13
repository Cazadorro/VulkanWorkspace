//
// Created by Shae Bolt on 6/2/2021.
//

#include "vul/debugutils.h"
#include "vul/bitmasks.h"
#include "vul/enums.h"
#include <cassert>
#include <iostream>

vul::DebugCreateInfoBuilder &
vul::DebugCreateInfoBuilder::pNext(const void *value) {
    m_createInfo.pNext = value;
    return *this;
}

vul::DebugCreateInfoBuilder &vul::DebugCreateInfoBuilder::messageSeverity(
        vul::DebugUtilsMessageSeverityFlagBitMask value) {
    m_createInfo.messageSeverity = static_cast<VkDebugUtilsMessageSeverityFlagsEXT>(value);
    return *this;
}

vul::DebugCreateInfoBuilder &vul::DebugCreateInfoBuilder::messageType(
        vul::DebugUtilsMessageTypeFlagBitMask value) {
    m_createInfo.messageType = static_cast<VkDebugUtilsMessageTypeFlagsEXT>(value);
    return *this;
}

vul::DebugCreateInfoBuilder &
vul::DebugCreateInfoBuilder::pfnUserCallback(vul::DebugCallback value) {
    m_debugCallback = std::move(value);
    return *this;
}

vul::DebugCreateInfoBuilder::DebugCreateInfoBuilder(
        const vul::Instance &instance,
        const VkAllocationCallbacks *pAllocator) : m_pInstance(&instance),
                                                   m_pAllocator(pAllocator),
                                                   m_createInfo{} {
    m_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    m_createInfo.flags = 0;
}

vul::ExpectedResult<vul::DebugUtilsMessengerEXT>
vul::DebugCreateInfoBuilder::create() const {
    ////TODO don't need this? vkCreateDebugUtilsMessengerEXT already is loaded when used statically?
    auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) m_pInstance->getProcAddr(
            "vkCreateDebugUtilsMessengerEXT");
    if (createDebugUtilsMessenger) {
        VkDebugUtilsMessengerCreateInfoEXT create_info_copy = m_createInfo;
        DebugUtilsMessengerEXT debugMessenger;
        auto user_callback = [](
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) -> VkBool32 {
            auto this_ptr = *static_cast<DebugUtilsMessengerEXT **>(pUserData);
            return this_ptr->m_debugCallback(
                    static_cast<DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity),
                    static_cast<vul::DebugUtilsMessageTypeFlagBitMask>(
                            static_cast<DebugUtilsMessageTypeFlagBitsEXT>(messageTypes)
                    ),
                    pCallbackData);
        };
        create_info_copy.pfnUserCallback = user_callback;
        create_info_copy.pUserData = debugMessenger.m_ppthis;
        VkDebugUtilsMessengerEXT vkDebugMessenger;
        VkResult result = createDebugUtilsMessenger(m_pInstance->get(),
                                                    &create_info_copy,
                                                    m_pAllocator,
                                                    &vkDebugMessenger);
        debugMessenger.m_pInstance = m_pInstance;
        debugMessenger.m_pAllocator = m_pAllocator;
        debugMessenger.m_debugCallback = m_debugCallback;
        debugMessenger.m_handle = vkDebugMessenger;
        return vul::ExpectedResult<vul::DebugUtilsMessengerEXT>(
                static_cast<Result>(result), std::move(debugMessenger));
    } else {
        return vul::ExpectedResult<vul::DebugUtilsMessengerEXT>(
                vul::Result::ErrorExtensionNotPresent,
                DebugUtilsMessengerEXT{});
    }
}

const VkDebugUtilsMessengerCreateInfoEXT &
vul::DebugCreateInfoBuilder::getCreateInfo() const {
    return m_createInfo;
}

VkDebugUtilsMessengerCreateInfoEXT
vul::DebugCreateInfoBuilder::createInstanceCallbackInfo(
        vul::DebugUtilsMessageSeverityFlagBitMask messageSeverity,
        vul::DebugUtilsMessageTypeFlagBitMask messageType, const void *pNext) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.flags = 0;
    createInfo.pNext = pNext;
    createInfo.messageSeverity = static_cast<VkDebugUtilsMessageSeverityFlagsEXT>(messageSeverity);
    createInfo.messageType = static_cast<VkDebugUtilsMessageTypeFlagsEXT>(messageType);
    //we don't allow customization of this callback because it is a pain to deal with
    //and makes vul::Instance a lot more complicated.
    //It's only relevant for creation and destruction of the instance anyway.
    createInfo.pfnUserCallback = defaultDebugMessengerCallbackVk;
    createInfo.pUserData = nullptr;
    return createInfo;
}

vul::DebugCreateInfoBuilder &vul::DebugCreateInfoBuilder::setDefaults() {
    auto messageSeverityFlags =
            DebugUtilsMessageSeverityFlagBitsEXT::VerboseBit |
            DebugUtilsMessageSeverityFlagBitsEXT::WarningBit |
            DebugUtilsMessageSeverityFlagBitsEXT::ErrorBit;
    auto messageTypeFlags =
            DebugUtilsMessageTypeFlagBitsEXT::GeneralBit |
            DebugUtilsMessageTypeFlagBitsEXT::ValidationBit |
            DebugUtilsMessageTypeFlagBitsEXT::PerformanceBit;
    return messageSeverity(messageSeverityFlags)
            .messageType(messageTypeFlags)
            .pfnUserCallback(defaultDebugMessengerCallback);
}


vul::DebugUtilsMessengerEXT::DebugUtilsMessengerEXT() {
    //set the pointer that the pointer points to, to this, for callback purposes.
    *m_ppthis = this;
}

vul::DebugUtilsMessengerEXT::DebugUtilsMessengerEXT(
        vul::DebugUtilsMessengerEXT &&rhs) noexcept {
    //by default all values should be nullptr etc.., so this should work.
    using std::swap;
    swap(m_pInstance, rhs.m_pInstance);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_debugCallback, rhs.m_debugCallback);
    swap(m_handle, rhs.m_handle);
    swap(m_ppthis, rhs.m_ppthis);
    (*m_ppthis) = this;
    (*(rhs.m_ppthis)) = &rhs;
}

vul::DebugUtilsMessengerEXT &
vul::DebugUtilsMessengerEXT::operator=(
        vul::DebugUtilsMessengerEXT &&rhs) noexcept {
    using std::swap;
    swap(m_pInstance, rhs.m_pInstance);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_debugCallback, rhs.m_debugCallback);
    swap(m_handle, rhs.m_handle);
    swap(m_ppthis, rhs.m_ppthis);
    (*m_ppthis) = this;
    (*(rhs.m_ppthis)) = &rhs;
    return *this;
}

vul::DebugUtilsMessengerEXT::~DebugUtilsMessengerEXT() {
    if(m_handle != VK_NULL_HANDLE) {
        if(m_pInstance == nullptr){
            fmt::print(stderr,
                       "Parent instance is nullptr, can't delete object\n");
            std::terminate();
        }
        auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) m_pInstance->getProcAddr(
                "vkDestroyDebugUtilsMessengerEXT");
        if (destroyDebugUtilsMessenger != nullptr) {
            destroyDebugUtilsMessenger(m_pInstance->get(), m_handle,
                                       m_pAllocator);
        } else {
            fmt::print(stderr,
                       "Couldn't load PFN_vkDestroyDebugUtilsMessengerEXT\n");
            std::terminate();
        }
    }
    delete m_ppthis;
}

VkDebugUtilsMessengerEXT vul::DebugUtilsMessengerEXT::get() const {
    return m_handle;
}

//source https://www.lunarg.com/wp-content/uploads/2018/05/Vulkan-Debug-Utils_05_18_v1.pdf
VkBool32 vul::defaultDebugMessengerCallback(
        vul::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vul::DebugUtilsMessageTypeFlagBitMask messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData) {
//    if(callbackData == nullptr){
//        std::cout << "DefaultDebugMessenger NULLPTR" << std::endl;
//        return VK_FALSE;
//    }
    assert(!std::string(callbackData->pMessage).empty());
    std::string prefix;
    if (messageSeverity ==
        vul::DebugUtilsMessageSeverityFlagBitsEXT::VerboseBit) {
        prefix = "VERBOSE : ";
        return VK_FALSE;
    } else if (messageSeverity ==
               vul::DebugUtilsMessageSeverityFlagBitsEXT::InfoBit) {
        prefix = "INFO : ";
    } else if (messageSeverity ==
               vul::DebugUtilsMessageSeverityFlagBitsEXT::WarningBit) {
        prefix = "WARNING : ";
    } else if (messageSeverity ==
               vul::DebugUtilsMessageSeverityFlagBitsEXT::ErrorBit) {
        prefix = "ERROR : ";
    }
    if (messageType.get(vul::DebugUtilsMessageTypeFlagBitsEXT::GeneralBit)) {
        prefix += "GENERAL";
    } else {
        if (messageType.get(
                vul::DebugUtilsMessageTypeFlagBitsEXT::ValidationBit)) {
            prefix += "SPEC_INVALIDATION";
            //validation_error = 1;
        }
        if (messageType.get(
                vul::DebugUtilsMessageTypeFlagBitsEXT::PerformanceBit)) {
            if (messageType.get(
                    vul::DebugUtilsMessageTypeFlagBitsEXT::ValidationBit)) {
                prefix += "|";
            }
            prefix += "PERFORMANCE";
        }
    }


    std::string message = fmt::format(
            "{} - Message ID Number {}, Message ID String {}:\n{}",
            prefix,
            callbackData->messageIdNumber,
            callbackData->pMessageIdName,
            callbackData->pMessage);
    if (callbackData->objectCount > 0) {
        std::string object_message = fmt::format("\n Objects - {}\n",
                                                 callbackData->objectCount);
        message += object_message;
        for (uint32_t object = 0;
             object < callbackData->objectCount; ++object) {
            std::string object_name;
            if (callbackData->pObjects[object].pObjectName != nullptr) {
                object_name = std::string(
                        callbackData->pObjects[object].pObjectName);
            }
            std::string tmp_object_message = fmt::format(
                    " Object[{}] - Type {}, Value {}, Name \"{}\"\n",
                    object,
                    vul::to_string(
                            static_cast<ObjectType>(callbackData->pObjects[object].objectType)),
                    reinterpret_cast<void *>(callbackData->pObjects[object].objectHandle),
                    object_name);
            message += tmp_object_message;
        }
    }
    if (callbackData->cmdBufLabelCount > 0) {
        message += fmt::format("\n Command Buffer Labels - {}\n",
                               callbackData->cmdBufLabelCount);
        for (uint32_t label = 0;
             label < callbackData->cmdBufLabelCount; ++label) {
            message += fmt::format(" Label[{}] - {} {{ {}, {} {}, {}}}\n",
                                   label,
                                   callbackData->pCmdBufLabels[label].pLabelName,
                                   callbackData->pCmdBufLabels[label].color[0],
                                   callbackData->pCmdBufLabels[label].color[1],
                                   callbackData->pCmdBufLabels[label].color[2],
                                   callbackData->pCmdBufLabels[label].color[3]);
        }
    }
    fmt::print("{}\n", message);
    std::cout.flush();
    // Don't bail out, but keep going.
    return VK_FALSE;
}

VkBool32 vul::defaultDebugMessengerCallbackVk(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void */*pUserData*/) {
    return vul::defaultDebugMessengerCallback(
            static_cast<DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity),
            static_cast<vul::DebugUtilsMessageTypeFlagBitMask>(
                    static_cast<DebugUtilsMessageTypeFlagBitsEXT>(messageTypes)
            ), pCallbackData);
}