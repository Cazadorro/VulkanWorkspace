//
// Created by Shae Bolt on 8/31/2021.
//

#include "vmaallocation.h"
#include "vul/vmaallocator.h"
#include "vul/tempvoidarrayproxy.h"

vul::VmaAllocation::VmaAllocation(const vul::VmaAllocator &allocator,
                                  ::VmaAllocation handle) : m_pAllocator(
        &allocator), m_handle(handle) {

}

VmaAllocationInfo vul::VmaAllocation::getAllocationInfo() const {
    VmaAllocationInfo info;
    vmaGetAllocationInfo(m_pAllocator->get(), m_handle, &info);
    return info;
}

::VmaAllocation vul::VmaAllocation::get() const {
    return m_handle;
}

const vul::VmaAllocator &vul::VmaAllocation::getAllocator() const{
    return *m_pAllocator;
}

void *vul::VmaAllocation::mapMemory() {
    void *mappedData = getMappedMemory();
    if (mappedData == nullptr) {
        vmaMapMemory(m_pAllocator->get(), m_handle, &mappedData);
        m_mapCounter += 1;
    }else if(m_mapCounter == 0){
//        m_mapCounter += 1;
    }
    return mappedData;
}

void *vul::VmaAllocation::getMappedMemory() const {
    return getAllocationInfo().pMappedData;
}

void vul::VmaAllocation::unmapMemory() {
    void *mappedData = getMappedMemory();
    if (mappedData != nullptr) {
        vmaUnmapMemory(m_pAllocator->get(), m_handle);
    }
}

bool vul::VmaAllocation::isMapped() const {
    return getMappedMemory() != nullptr;
}

void vul::VmaAllocation::flush(VkDeviceSize offset, VkDeviceSize size) {
    vmaFlushAllocation(m_pAllocator->get(), m_handle, offset, size);
}

void vul::VmaAllocation::invalidate(VkDeviceSize offset, VkDeviceSize size) {
    vmaInvalidateAllocation(m_pAllocator->get(), m_handle, offset, size);
}

vul::VmaAllocation::VmaAllocation(vul::VmaAllocation &&rhs) noexcept : VmaAllocation(){
    using std::swap;
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_mapCounter, rhs.m_mapCounter);
}

vul::VmaAllocation &
vul::VmaAllocation::operator=(vul::VmaAllocation &&rhs) noexcept {
    using std::swap;
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    swap(m_mapCounter, rhs.m_mapCounter);
    return *this;
}

void vul::VmaAllocation::unmapAllCounts() {
    for(std::int64_t i = 0; i < m_mapCounter; ++i){
        unmapMemory();
    }
}

void
vul::VmaAllocation::mappedCopyFrom(const vul::TempConstVoidArrayProxy &array,
                                   std::size_t offsetBytes) {
    void * mappedPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(mapMemory()) + offsetBytes);
    memcpy(mappedPtr, array.data(), array.size_bytes());
}


