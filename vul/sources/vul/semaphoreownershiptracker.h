//
// Created by Shae Bolt on 12/23/2021.
//

#ifndef VULKANWORKSPACE_SEMAPHOREOWNERSHIPTRACKER_H
#define VULKANWORKSPACE_SEMAPHOREOWNERSHIPTRACKER_H
#include "vul/threadsafequeue.h"
#include "vul/unique_function.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <thread>
#include <mutex>
namespace vul {
    class Device;
    class TimelineSemaphore;
    class SemaphoreOwnershipTracker {
    public:
        SemaphoreOwnershipTracker(const Device& device);
        ~SemaphoreOwnershipTracker();
        void updateSemaphoreOwnership(
                const std::vector<std::uint64_t>& ids,
                                      std::uint64_t semaphoreHandle,
                                      std::uint64_t semaphoreValue);
        void removeSemaphoreOwnership(const TimelineSemaphore& semaphore);
        bool objectUsedInQueue(std::uint64_t objectID) const;
        bool semaphoreUsedInOwnership(std::uint64_t semaphoreHandle) const;
        void addDeletedObject(std::uint64_t objectID, vul::unique_function<void()>&& deleter);
        struct IdAndDeleter{
            std::uint64_t id;
            vul::unique_function<void()> deleter;
        };
        struct ObjectSemaphoreValue{
            std::uint64_t objectId;
            std::uint64_t semaphoreValue;
        };
        struct SemaphoreObjectList{
            std::uint64_t currentValue;
            std::vector<ObjectSemaphoreValue> objectValues;
            [[nodiscard]]
            std::size_t size() const;
            void merge(const std::vector<std::uint64_t>& objectIDs, std::uint64_t semaphoreValue);
            [[nodiscard]]
            static SemaphoreObjectList fromObjectIDList(const std::vector<std::uint64_t>& objectIDs, std::uint64_t semaphoreValue, std::uint64_t currentValue);
            [[nodiscard]]
            bool contains(std::uint64_t objectID) const;
            [[nodiscard]]
            std::vector<ObjectSemaphoreValue>::const_iterator find(std::uint64_t objectID) const;
            [[nodiscard]]
            std::vector<ObjectSemaphoreValue>::iterator find(std::uint64_t objectID);
        };
    private:
        const Device *m_pDevice = nullptr;
        vul::ThreadSafeQueue<IdAndDeleter> m_deletionQueue;
        mutable std::mutex m_semaphoreMapMutex;
        std::unordered_map<std::uint64_t, SemaphoreObjectList> m_semaphoreObjectMap;
        std::thread m_deletionThread;
    };

}


#endif //VULKANWORKSPACE_SEMAPHOREOWNERSHIPTRACKER_H
