cmake_minimum_required(VERSION 3.13)
project(vma)

include(FetchContent)

FetchContent_Declare(
        vmaheaderonly
#        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
#        GIT_TAG master

        URL https://raw.githubusercontent.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/master/include/vk_mem_alloc.h
        DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(vmaheaderonly)
find_package(Vulkan REQUIRED)

add_library(VulkanMemoryAllocator STATIC)
target_sources(VulkanMemoryAllocator PRIVATE
        vk_mem_alloc.h
        vk_mem_alloc.cpp)

target_include_directories(VulkanMemoryAllocator PUBLIC
        ./)

target_link_libraries(VulkanMemoryAllocator
        PUBLIC
        Vulkan::Vulkan
        )