//
// Created by Bolt on 7/7/2023.
//

#include "vul/dependencyinfo.h"
#include "vul/memorybarrier.h"
#include "vul/buffermemorybarrier.h"
#include "vul/imagememorybarrier.h"

vul::DependencyInfo::DependencyInfo() :
        pNext(nullptr),
        dependencyFlags(0),
        memoryBarrierCount(0),
        pMemoryBarriers(nullptr),
        bufferMemoryBarrierCount(0),
        pBufferMemoryBarriers(nullptr),
        imageMemoryBarrierCount(0),
        pImageMemoryBarriers(nullptr) {

}

vul::DependencyInfo::DependencyInfo(
        std::span<const MemoryBarrier> memoryBarriers,
        std::span<const BufferMemoryBarrier> bufferMemoryBarriers,
        std::span<const ImageMemoryBarrier> imageMemoryBarriers,
        uul::EnumFlags<vul::DependencyFlagBits> dependencyFlags,
        const void *pNext) :
        pNext(pNext),
        dependencyFlags(dependencyFlags),
        memoryBarrierCount(memoryBarriers.size()),
        pMemoryBarriers(memoryBarriers.data()),
        bufferMemoryBarrierCount(bufferMemoryBarriers.size()),
        pBufferMemoryBarriers(bufferMemoryBarriers.data()),
        imageMemoryBarrierCount(imageMemoryBarriers.size()),
        pImageMemoryBarriers(imageMemoryBarriers.data()) {

}

vul::DependencyInfo::DependencyInfo(const vul::DependencyInfo &rhs) noexcept:
        pNext(rhs.pNext),
        dependencyFlags(rhs.dependencyFlags),
        memoryBarrierCount(rhs.memoryBarrierCount),
        pMemoryBarriers(rhs.pMemoryBarriers),
        bufferMemoryBarrierCount(rhs.bufferMemoryBarrierCount),
        pBufferMemoryBarriers(rhs.pBufferMemoryBarriers),
        imageMemoryBarrierCount(rhs.imageMemoryBarrierCount),
        pImageMemoryBarriers(rhs.pImageMemoryBarriers) {

}

vul::DependencyInfo::DependencyInfo(vul::DependencyInfo &&rhs) noexcept:
        pNext(rhs.pNext),
        dependencyFlags(rhs.dependencyFlags),
        memoryBarrierCount(rhs.memoryBarrierCount),
        pMemoryBarriers(rhs.pMemoryBarriers),
        bufferMemoryBarrierCount(rhs.bufferMemoryBarrierCount),
        pBufferMemoryBarriers(rhs.pBufferMemoryBarriers),
        imageMemoryBarrierCount(rhs.imageMemoryBarrierCount),
        pImageMemoryBarriers(rhs.pImageMemoryBarriers) {

}

vul::DependencyInfo &vul::DependencyInfo::operator=(const vul::DependencyInfo &rhs) noexcept {
    if (this != &rhs) {
        pNext = rhs.pNext;
        dependencyFlags = rhs.dependencyFlags;
        memoryBarrierCount = rhs.memoryBarrierCount;
        pMemoryBarriers = rhs.pMemoryBarriers;
        bufferMemoryBarrierCount = rhs.bufferMemoryBarrierCount;
        pBufferMemoryBarriers = rhs.pBufferMemoryBarriers;
        imageMemoryBarrierCount = rhs.imageMemoryBarrierCount;
        pImageMemoryBarriers = rhs.pImageMemoryBarriers;
    }
    return *this;
}

vul::DependencyInfo::~DependencyInfo() = default;

vul::DependencyInfo::operator VkDependencyInfo() const noexcept {
    return get();
}

VkDependencyInfo &vul::DependencyInfo::get() noexcept {
    return *reinterpret_cast<VkDependencyInfo *>(this);
}

const VkDependencyInfo &vul::DependencyInfo::get() const noexcept {
    return *reinterpret_cast<const VkDependencyInfo *>(this);
}
