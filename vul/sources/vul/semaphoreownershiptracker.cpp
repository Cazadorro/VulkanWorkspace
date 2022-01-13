//
// Created by Shae Bolt on 12/23/2021.
//

#include "semaphoreownershiptracker.h"
#include "vul/device.h"
#include "vul/semaphore.h"
#include "vul/vkassert.h"
#include <vulkan/vulkan.h>
#include <range/v3/view/zip.hpp>

void deleterFunction(VkDevice deviceHandle,
                     vul::ThreadSafeQueue<vul::SemaphoreOwnershipTracker::IdAndDeleter> &deletionQueue,
                     std::mutex &semaphoreMapMutex,
                     std::unordered_map<std::uint64_t, vul::SemaphoreOwnershipTracker::SemaphoreObjectList> &semaphoreObjectMap) {
    vul::SemaphoreOwnershipTracker::IdAndDeleter item;
    while (deletionQueue.pop_sync(item)) {
        std::vector<std::uint64_t> waitValues;
        std::vector<VkSemaphore> semaphoreHandles;
        {
            std::scoped_lock lock(semaphoreMapMutex);
            for (auto&[semaphoreID, semaphoreObjectList]: semaphoreObjectMap) {
                if (auto it = semaphoreObjectList.find(item.id); it !=
                                                                 semaphoreObjectList.objectValues.end()) {
                    auto value = *it;
                    auto semaphoreHandle = static_cast<VkSemaphore>((void *) semaphoreID);
                    std::uint64_t currentSemaphoreValue;

                    auto result = static_cast<vul::Result>(vkGetSemaphoreCounterValue(
                            deviceHandle, semaphoreHandle,
                            &currentSemaphoreValue));
                    VUL_ASSERT(result == vul::Result::Success,
                               fmt::format(
                                       "Couldn't get semaphore value: {}",
                                       vul::to_string(result)).c_str());

                    if (currentSemaphoreValue < value.semaphoreValue) {
                        waitValues.push_back(value.semaphoreValue);
                        semaphoreHandles.push_back(semaphoreHandle);
                    }
                    semaphoreObjectList.objectValues.erase(it);
                }
            }
        }
        if (!semaphoreHandles.empty()) {
            VkSemaphoreWaitInfo waitInfo = {};
            waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
            waitInfo.pNext = nullptr;
            waitInfo.flags = 0;
            waitInfo.semaphoreCount = semaphoreHandles.size();
            waitInfo.pSemaphores = semaphoreHandles.data();
            waitInfo.pValues = waitValues.data();
            auto waitResult = static_cast<vul::Result>(vkWaitSemaphores(
                    deviceHandle, &waitInfo, UINT64_MAX));
            VUL_ASSERT(waitResult == vul::Result::Success,
                       fmt::format("Couldn't wait on semaphore: result == {}",
                                   vul::to_string(waitResult)).c_str());
        }
        //deleting the actual object now
        item.deleter();
    }
}

void
vul::SemaphoreOwnershipTracker::updateSemaphoreOwnership(
        const std::vector<std::uint64_t> &ids,
        std::uint64_t semaphoreHandle,
        std::uint64_t semaphoreValue) {

    std::uint64_t currentSemaphoreValue;
    auto result = static_cast<vul::Result>(vkGetSemaphoreCounterValue(
            m_pDevice->get(),
            static_cast<VkSemaphore>((void *) semaphoreHandle),
            &currentSemaphoreValue));
    VUL_ASSERT(result == vul::Result::Success,
               fmt::format("Couldn't get semaphore value: {}",
                           vul::to_string(result)).c_str());
    auto idsSorted = ids;
    std::sort(idsSorted.begin(), idsSorted.end());
    std::scoped_lock lock(m_semaphoreMapMutex);
    if (auto it = m_semaphoreObjectMap.find(semaphoreHandle);
            it != m_semaphoreObjectMap.end()) {
        it->second.merge(ids, semaphoreValue);
        it->second.currentValue = currentSemaphoreValue;
    } else {
        m_semaphoreObjectMap[semaphoreHandle] = SemaphoreObjectList::fromObjectIDList(
                ids, semaphoreValue, currentSemaphoreValue);
    }
}

void vul::SemaphoreOwnershipTracker::removeSemaphoreOwnership(
        const TimelineSemaphore &semaphore) {
    //should be fine if handle doesn't exist?

    auto currentSemaphoreValue = semaphore.getCounterValue().assertValue();
    auto semaphoreHandle = reinterpret_cast<std::uintptr_t>(semaphore.get());
    std::scoped_lock lock(m_semaphoreMapMutex);
    if (auto it = m_semaphoreObjectMap.find(semaphoreHandle);
            it != m_semaphoreObjectMap.end()) {
#ifdef VUL_DEBUG
        auto &objectValues = it->second.objectValues;
        auto semaphore_done = !std::any_of(objectValues.begin(),
                                           objectValues.end(),
                                           [currentSemaphoreValue](
                                                   const auto &value) {
                                               return value.semaphoreValue >
                                                      currentSemaphoreValue;
                                           });
        VUL_DEBUG_ASSERT(semaphore_done, fmt::format(
                "Timeline Semaphore has resources associated with it whose associated value are larger than the current semaphore state").c_str());
#endif
        m_semaphoreObjectMap.erase(semaphoreHandle);
    }
}

bool vul::SemaphoreOwnershipTracker::objectUsedInQueue(
        std::uint64_t objectID) const {
    std::scoped_lock lock(m_semaphoreMapMutex);
    return std::any_of(m_semaphoreObjectMap.begin(), m_semaphoreObjectMap.end(),
                       [objectID](const auto pair) {
                           return pair.second.contains(objectID);
                       });
}

bool vul::SemaphoreOwnershipTracker::semaphoreUsedInOwnership(
        std::uint64_t semaphoreHandle) const {
    std::scoped_lock lock(m_semaphoreMapMutex);
    auto it = m_semaphoreObjectMap.find(semaphoreHandle);
    return it != m_semaphoreObjectMap.end();
}

vul::SemaphoreOwnershipTracker::SemaphoreOwnershipTracker(
        const vul::Device &device) : m_pDevice(&device) {
    m_deletionThread = std::thread(deleterFunction, m_pDevice->get(), std::ref(m_deletionQueue), std::ref(m_semaphoreMapMutex), std::ref(m_semaphoreObjectMap));
}

vul::SemaphoreOwnershipTracker::~SemaphoreOwnershipTracker() {
    m_deletionQueue.finish();
    m_deletionThread.join();
}

void vul::SemaphoreOwnershipTracker::addDeletedObject(std::uint64_t objectID,
                                                      vul::unique_function<void()>&& deleter) {
    m_deletionQueue.push({objectID, std::move(deleter)});
}


std::size_t vul::SemaphoreOwnershipTracker::SemaphoreObjectList::size() const {
    return objectValues.size();
}

void vul::SemaphoreOwnershipTracker::SemaphoreObjectList::merge(
        const std::vector<std::uint64_t> &objectIDs,
        std::uint64_t semaphoreValue) {
    auto idsSorted = objectIDs;
    std::sort(idsSorted.begin(), idsSorted.end());
    std::vector<ObjectSemaphoreValue> newObjectSemaphoreValues;
    newObjectSemaphoreValues.reserve(
            std::max(objectValues.size(),
                     idsSorted.size()));
    std::size_t currentListIdx = 0;
    std::size_t newListIdx = 0;
    while (currentListIdx < objectValues.size() ||
           newListIdx < idsSorted.size()) {
        //both lists are now sorted, so we insert into a new list linearly guaranteeing even the next one will be in order. 
        auto currentObjectID = objectValues[currentListIdx].objectId;
        auto newObjectID = idsSorted[newListIdx];
        if (currentObjectID == newObjectID) { //updating old one
            newObjectSemaphoreValues.push_back({newObjectID, semaphoreValue});
            currentListIdx += 1;
            newListIdx += 1;
        } else if (currentObjectID > newObjectID) { //new one
            newObjectSemaphoreValues.push_back({newObjectID, semaphoreValue});
            newListIdx += 1;
        } else { // not overwriting old one.
            newObjectSemaphoreValues.push_back(objectValues[currentListIdx]);
            currentListIdx += 1;
        }
    }
    //fill in the rest of the current list, if there's still left over
    for (std::size_t i = currentListIdx; i < objectValues.size(); ++i) {
        newObjectSemaphoreValues.push_back(objectValues[i]);
    }
    //fill in the rest of the new list, if there's still left over, current won't run if this one does
    for (std::size_t i = newListIdx; i < idsSorted.size(); ++i) {
        newObjectSemaphoreValues.push_back(
                {idsSorted[newListIdx], semaphoreValue});
    }
    objectValues = newObjectSemaphoreValues;

}

vul::SemaphoreOwnershipTracker::SemaphoreObjectList
vul::SemaphoreOwnershipTracker::SemaphoreObjectList::fromObjectIDList(
        const std::vector<std::uint64_t> &objectIDs,
        std::uint64_t semaphoreValue, std::uint64_t currentValue) {
    SemaphoreObjectList newObjectList;
    newObjectList.currentValue = currentValue;
    newObjectList.merge(objectIDs, semaphoreValue);
    return newObjectList;
}

bool vul::SemaphoreOwnershipTracker::SemaphoreObjectList::contains(
        std::uint64_t objectID) const {
    auto it = std::find_if(objectValues.begin(), objectValues.end(),
                           [objectID](const auto &value) {
                               return value.objectId == objectID;
                           });
    return it != objectValues.end();
}

std::vector<vul::SemaphoreOwnershipTracker::ObjectSemaphoreValue>::const_iterator
vul::SemaphoreOwnershipTracker::SemaphoreObjectList::find(
        std::uint64_t objectID) const {
    auto it = std::find_if(objectValues.begin(), objectValues.end(),
                           [objectID](const auto &value) {
                               return value.objectId == objectID;
                           });
    return it;
}

std::vector<vul::SemaphoreOwnershipTracker::ObjectSemaphoreValue>::iterator
vul::SemaphoreOwnershipTracker::SemaphoreObjectList::find(
        std::uint64_t objectID)  {
    return std::find_if(objectValues.begin(), objectValues.end(),
                        [objectID](const auto &value) {
                            return value.objectId == objectID;
                        });
}

