//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_QUEUE_H
#define VULKANWORKSPACE_QUEUE_H

#include "vul/temparrayproxyfwd.h"
#include <vulkan/vulkan.h>
#include <vul/enumsfwd.h>
#include <string>

namespace vul {
    class Device;

    class Queue {
    public:
        Queue() = default;

        Queue(const Device &device, VkQueue handle);

        [[nodiscard]]
        VkQueue get() const;

        Result setObjectName(const std::string &string);

        Result
        submit(const TempArrayProxy<const VkSubmitInfo2KHR> &submitInfos) const;

        Queue(Queue &&rhs) noexcept = default;

        Queue &operator=(Queue &&rhs) noexcept = default;

        Queue(const Queue &rhs) = delete;

        Queue &operator=(const Queue &rhs) = delete;

        Result waitIdle() const;

    private:
        const Device *m_pDevice = nullptr;
        VkQueue m_handle = VK_NULL_HANDLE;
    };
}


#endif //VULKANWORKSPACE_QUEUE_H
