//
// Created by Shae Bolt on 8/30/2021.
//

#include "pipelinecache.h"
#include "vul/enums.h"
#include "vul/device.h"
#include "vul/expectedresult.h"

vul::PipelineCache::PipelineCache(const vul::Device &device,
                                  VkPipelineCache handle,
                                  const VkAllocationCallbacks *pAllocator): m_pDevice(
        &device), m_pAllocator(pAllocator), m_handle(handle)  {

}

VkPipelineCache vul::PipelineCache::get() const {
    return m_handle;
}

vul::PipelineCache::~PipelineCache() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyPipelineCache(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::PipelineCache::PipelineCache(vul::PipelineCache &&rhs) noexcept : PipelineCache(){
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::PipelineCache &
vul::PipelineCache::operator=(vul::PipelineCache &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::PipelineCache::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

vul::ExpectedResult<std::vector<std::byte>>
vul::PipelineCache::getCacheData() const {
    std::size_t dataSize;
    auto result = static_cast<Result>(vkGetPipelineCacheData(m_pDevice->get(), m_handle, &dataSize, nullptr));
    if(result != vul::Result::Success){
        return {result,{}};
    }
    std::vector<std::byte> bytes(dataSize);
    result=  static_cast<Result>(vkGetPipelineCacheData(m_pDevice->get(), m_handle, &dataSize, bytes.data()));
    return {result, std::move(bytes)};
}
