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
    struct SubmitInfo2;
    class Queue {
    public:
        Queue() = default;

        Queue(const Device &device, VkQueue handle, std::uint32_t queueFamilyIndex, std::uint32_t queueIndex);

        [[nodiscard]]
        VkQueue get() const;

        Result setObjectName(const std::string &string);

        Result submit(const TempArrayProxy<const SubmitInfo2> &submitInfos) const;

        Queue(Queue &&rhs) noexcept = default;

        Queue &operator=(Queue &&rhs) noexcept = default;

        Queue(const Queue &rhs) = delete;

        Queue &operator=(const Queue &rhs) = delete;

        Result waitIdle() const;

        [[nodiscard]]
        std::uint32_t getQueueFamilyIndex() const;

        [[nodiscard]]
        std::uint32_t getQueueIndex() const;
    private:
        const Device *m_pDevice = nullptr;
        VkQueue m_handle = VK_NULL_HANDLE;
        std::uint32_t m_queueFamilyIndex = 0;
        std::uint32_t m_queueIndex = 0;

    };
}


#endif //VULKANWORKSPACE_QUEUE_H
