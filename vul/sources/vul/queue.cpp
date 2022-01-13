//
// Created by Shae Bolt on 6/9/2021.
//

#include "vul/queue.h"
#include "vul/device.h"
#include "vul/temparrayproxy.h"
#include "vul/enums.h"
#include "vul/extensionfunctions.h"

VkQueue vul::Queue::get() const {
    return m_handle;
}

vul::Result vul::Queue::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::Result vul::Queue::waitIdle() const {
    return static_cast<Result>(vkQueueWaitIdle(m_handle));
}

vul::Result vul::Queue::submit(const TempArrayProxy<const VkSubmitInfo2KHR>& submitInfos) const {
    auto vkQueueSubmit2KHR_f = (PFN_vkQueueSubmit2KHR) vkGetDeviceProcAddr(m_pDevice->get(), "vkQueueSubmit2KHR");
    return static_cast<Result>(vkQueueSubmit2KHR_f(m_handle, static_cast<std::uint32_t>(submitInfos.size()), submitInfos.data(), VK_NULL_HANDLE));
}

vul::Queue::Queue(const vul::Device &device, VkQueue handle) : m_pDevice(&device), m_handle(handle){

}
