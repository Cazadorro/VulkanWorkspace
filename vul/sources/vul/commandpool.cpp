//
// Created by Shae Bolt on 8/31/2021.
//

#include "commandpool.h"
#include "vul/device.h"
#include "vul/enums.h"
#include "vul/queue.h"
#include "vul/commandbuffer.h"
#include "vul/expectedresult.h"
#include "vul/semaphore.h"
#include "vul/temparrayproxy.h"
#include "vul/submitinfobuilder.h"

vul::CommandPool::CommandPool(const vul::Device &device, VkCommandPool handle,
                              const VkAllocationCallbacks *pAllocator) :
        m_pDevice(&device), m_pAllocator(pAllocator), m_handle(handle) {

}

VkCommandPool vul::CommandPool::get() const {
    return m_handle;
}

vul::CommandPool::~CommandPool() {
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_pDevice->get(), m_handle, m_pAllocator);
    }
}

vul::CommandPool::CommandPool(vul::CommandPool &&rhs) noexcept : CommandPool(){
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
}

vul::CommandPool &vul::CommandPool::operator=(vul::CommandPool &&rhs) noexcept {
    using std::swap;
    swap(m_pDevice, rhs.m_pDevice);
    swap(m_pAllocator, rhs.m_pAllocator);
    swap(m_handle, rhs.m_handle);
    return *this;
}

vul::Result vul::CommandPool::setObjectName(const std::string &string) {
    return m_pDevice->setObjectName(m_handle, string);
}

const vul::Device &vul::CommandPool::getDevice() const {
    return *m_pDevice;
}

vul::ExpectedResult<std::vector<vul::PrimaryCommandBuffer>>
vul::CommandPool::createPrimaryCommandBuffers(std::uint32_t commandBufferCount, const void* pNext) const {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = pNext;
    allocInfo.commandPool = m_handle;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = commandBufferCount;
    std::vector<VkCommandBuffer> commandBuffers(commandBufferCount);


    auto result = static_cast<Result>(vkAllocateCommandBuffers(m_pDevice->get(), &allocInfo, commandBuffers.data()));
    std::vector<PrimaryCommandBuffer> primaryCommandBuffers;
    primaryCommandBuffers.reserve(commandBufferCount);
    for(const auto& commandBuffer : commandBuffers){
        primaryCommandBuffers.emplace_back(*this, commandBuffer);
    }
    return {result, std::move(primaryCommandBuffers)};
}

vul::ExpectedResult<std::vector<vul::SecondaryCommandBuffer>>
vul::CommandPool::createSecondaryCommandBuffers(
        std::uint32_t commandBufferCount, const void *pNext) const {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = pNext;
    allocInfo.commandPool = m_handle;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = commandBufferCount;
    std::vector<VkCommandBuffer> commandBuffers(commandBufferCount);


    auto result = static_cast<Result>(vkAllocateCommandBuffers(m_pDevice->get(), &allocInfo, commandBuffers.data()));
    std::vector<SecondaryCommandBuffer> secondaryCommandBuffers;
    secondaryCommandBuffers.reserve(commandBufferCount);
    for(const auto& commandBuffer : commandBuffers){
        secondaryCommandBuffers.emplace_back(*this, commandBuffer);
    }
    return {result, std::move(secondaryCommandBuffers)};
}

vul::ExpectedResult<vul::PrimaryCommandBuffer>
vul::CommandPool::createPrimaryCommandBuffer(const void *pNext) const {
    auto expectedResult = createPrimaryCommandBuffers(1, pNext);
    if(!expectedResult.hasValue()){
        return {expectedResult.result, {}};
    }
    return {expectedResult.result, std::move(expectedResult.value[0])};
}

vul::ExpectedResult<vul::SecondaryCommandBuffer>
vul::CommandPool::createSecondaryCommandBuffer(const void *pNext) const {
    auto expectedResult = createSecondaryCommandBuffers(1, pNext);
    if(!expectedResult.hasValue()){
        return {expectedResult.result, {}};
    }
    return {expectedResult.result, std::move(expectedResult.value[0])};
}

vul::Result vul::CommandPool::singleTimeSubmit(const vul::Queue &queue,
                                               const std::function<void(
                                                       CommandBuffer &)>& commandBufferFunction) const {
    auto commandBuffer = createPrimaryCommandBuffer().assertValue();
    commandBuffer.begin(vul::CommandBufferUsageFlagBits::OneTimeSubmitBit);
    commandBufferFunction(commandBuffer);
    commandBuffer.end();
    const auto &device = getDevice();
    auto expectedSemaphore = device.createTimelineSemaphore(0);
    if (!expectedSemaphore.hasValue()) {
        return expectedSemaphore.result;
    }
    auto timelineSemaphore = std::move(expectedSemaphore.value);
    std::uint64_t waitValue = 1;
    auto signalInfo = timelineSemaphore.createSubmitInfo(waitValue,
                                                         vul::PipelineStageFlagBits2::AllTransferBit);
    auto commandBufferInfo = commandBuffer.createSubmitInfo();

    auto result = queue.submit(vul::SubmitInfoBuilder()
                                       .commandBufferInfos(commandBufferInfo)
                                       .signalSemaphoreInfos(signalInfo)
                                       .create());
    if (result != vul::Result::Success) {
        return result;
    }
    return device.wait({timelineSemaphore}, waitValue);
}


