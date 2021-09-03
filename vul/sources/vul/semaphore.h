//
// Created by Shae Bolt on 8/17/2021.
//

#ifndef VULKANWORKSPACE_SEMAPHORE_H
#define VULKANWORKSPACE_SEMAPHORE_H

#include "vul/enumsfwd.h"
#include "vul/bitmasksfwd.h"
#include <gsl/span>
#include <vulkan/vulkan.h>
#include <string>

namespace vul {
    template<typename T>
    class ExpectedResult;

    class Device;

    class BinarySemaphore {
    public:
        BinarySemaphore() = default;

        BinarySemaphore(const Device &device, VkSemaphore handle,
                        const VkAllocationCallbacks *pAllocator = nullptr);

        BinarySemaphore(BinarySemaphore &&rhs) noexcept;

//        was noexcept?
        BinarySemaphore &operator=(BinarySemaphore &&rhs) noexcept;

        BinarySemaphore(const BinarySemaphore &rhs) = delete;

        BinarySemaphore &operator=(BinarySemaphore &rhs) = delete;

        ~BinarySemaphore();

        [[nodiscard]]
        const VkSemaphore& get() const;

        Result setObjectName(const std::string &string);
        [[nodiscard]]
        VkSemaphoreSubmitInfoKHR createSubmitInfo(
                vul::PipelineStageFlagBitMask stageMask,
                std::uint32_t deviceIndex = 0, const void* pNext = nullptr) const;
    private:
        const Device *m_pDevice = nullptr;
        VkSemaphore m_handle = VK_NULL_HANDLE;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
    };

    class TimelineSemaphore {
    public:
        TimelineSemaphore() = default;

        TimelineSemaphore(const Device &device, VkSemaphore handle,
                          const VkAllocationCallbacks *pAllocator = nullptr);

        TimelineSemaphore(TimelineSemaphore &&rhs) noexcept;

//        was noexcept?
        TimelineSemaphore &operator=(TimelineSemaphore &&rhs) noexcept;

        TimelineSemaphore(const TimelineSemaphore &rhs) = delete;

        TimelineSemaphore &operator=(TimelineSemaphore &rhs) = delete;

        ~TimelineSemaphore();

        [[nodiscard]]
        const VkSemaphore& get() const;

        Result setObjectName(const std::string &string);

        [[nodiscard]]
        ExpectedResult<std::uint64_t> getCounterValue() const;

        Result signal(std::uint64_t signalValue);

        Result
        wait(std::uint64_t waitValue, std::uint64_t timeout_ns = UINT64_MAX);
        //semaphores must not be empty, waitValues must contain the same number of values as semaphores,
        // and all semaphores must share the same device parent

        static Result wait(const gsl::span<const TimelineSemaphore * const>& semaphores,
                    const gsl::span<std::uint64_t const>& waitValues,
                    std::uint64_t timeout_ns,
                    SemaphoreWaitBitMask flags);

        static Result wait(const gsl::span<const TimelineSemaphore * const>& semaphores,
                    const gsl::span<std::uint64_t const>& waitValues,
                    std::uint64_t timeout_ns = UINT64_MAX);

        static Result waitAny(const gsl::span<const TimelineSemaphore * const>& semaphores,
                       const gsl::span<std::uint64_t const>& waitValues,
                       std::uint64_t timeout_ns = UINT64_MAX);

        [[nodiscard]]
        VkSemaphoreSubmitInfoKHR createSubmitInfo(
                std::uint64_t value, vul::PipelineStageFlagBitMask stageMask,
                std::uint32_t deviceIndex = 0, const void* pNext = nullptr) const;


    private:
        const Device *m_pDevice = nullptr;
        VkSemaphore m_handle = VK_NULL_HANDLE;
        const VkAllocationCallbacks *m_pAllocator = nullptr;
    };




}
#endif //VULKANWORKSPACE_SEMAPHORE_H
