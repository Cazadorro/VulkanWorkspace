//
// Created by Shae Bolt on 6/9/2021.
//

#include "vul/queue.h"
#include "vul/device.h"
#include "vul/temparrayproxy.h"
#include "vul/enums.h"

VkQueue vul::Queue::get() const {
    return m_handle;
}

vul::Result vul::Queue::setObjectName(const std::string &string,
                                      const vul::Device &device) {
    return device.setObjectName(m_handle, string);
}

vul::Result vul::Queue::waitIdle() const {
    return static_cast<Result>(vkQueueWaitIdle(m_handle));
}

vul::Result vul::Queue::submit(const TempArrayProxy<const VkSubmitInfo2KHR>& submitInfos) const {
    return static_cast<Result>(vkQueueSubmit2KHR(m_handle, submitInfos.size(), submitInfos.data(), VK_NULL_HANDLE));
}
