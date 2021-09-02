//
// Created by Shae Bolt on 6/9/2021.
//

#ifndef VULKANWORKSPACE_QUEUE_H
#define VULKANWORKSPACE_QUEUE_H
#include<vulkan/vulkan.hpp>
#include<vul/enumsfwd.h>
namespace vul {
    template<typename T>
    class TempArrayProxy;
    class Device;
    class Queue {
    public:
        Queue() = default;
        [[nodiscard]]
        VkQueue get() const;
        Result setObjectName(const std::string &string, const Device& device);

        Result submit(const TempArrayProxy<const VkSubmitInfo2KHR>& submitInfos) const;

        Queue(Queue &&rhs) noexcept = default;

        Queue &operator=(Queue &&rhs) noexcept = default;

        Queue(const Queue &rhs) = delete;

        Queue &operator=(Queue &rhs) = delete;

        Result waitIdle() const;
    private:
        VkQueue m_handle = VK_NULL_HANDLE;
    };
}


#endif //VULKANWORKSPACE_QUEUE_H
