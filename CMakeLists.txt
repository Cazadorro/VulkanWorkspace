cmake_minimum_required(VERSION 3.21)
#cmake_policy(SET CMP0116 NEW)
if(WIN32)
        # Set MSVC runtime to MultiThreaded (/MT)
#        cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
project(VulkanWorkspace)
#TODO cite goodvibes texture pack for textures
set(CMAKE_CXX_STANDARD 20)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(VkShaderBuildFunctions)
#set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")
#set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=lld")
#set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld")

# VCPKG packages.
#find_package(freetype CONFIG REQUIRED)

find_package(Vulkan REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_compile_definitions(glm::glm INTERFACE
        -DGLM_FORCE_RADIANS
        -DGLM_FORCE_DEPTH_ZERO_TO_ONE
        -DGLM_FORCE_LEFT_HANDED
        -DGLM_FORCE_SWIZZLE
        -DGLM_FORCE_SILENT_WARNINGS
        )
find_package(glfw3 CONFIG REQUIRED)
target_compile_definitions(glfw INTERFACE
        -DGLFW_INCLUDE_VULKAN
        )
find_package(czdr-vkutil CONFIG REQUIRED)
find_package(czdr-bitutil CONFIG REQUIRED)
find_package(czdr-stdutil CONFIG REQUIRED)
find_package(czdr-cmakeutil CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(czdr-glfw-wrapper CONFIG REQUIRED)
find_package(readerwriterqueue CONFIG REQUIRED)
get_target_property(LIBA_INCLUDES moodycamel::readerwriterqueue INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(LIBB_INCLUDES czdr::glfw-wrapper INCLUDE_DIRECTORIES)
get_target_property(LIBC_INCLUDES czdr::glfw-wrapper INTERFACE_INCLUDE_DIRECTORIES)
message(${LIBA_INCLUDES})
message(${LIBB_INCLUDES})
message(${LIBC_INCLUDES})
option(TINYGLTF_BUILD_LOADER_EXAMPLE "" OFF)
option(TINYGLTF_HEADER_ONLY "" OFF)
add_subdirectory(external/tinygltf)
add_subdirectory(external/cxxopts)

add_subdirectory(external/imgui)
add_subdirectory(external/json)

#add_subdirectory(external/vma)
add_subdirectory(external/stb)
add_subdirectory(external/msdfgen)


add_library(pcg_cpp INTERFACE)
target_sources(pcg_cpp INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/pcg_cpp/include/pcg_extras.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/pcg_cpp/include/pcg_random.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/pcg_cpp/include/pcg_uint128.hpp
        )
target_include_directories(pcg_cpp INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/pcg_cpp/include/
        )



#add_subdirectory(external/glslang)

add_subdirectory(vulgenerated)
#add_subdirectory(uul)
#add_subdirectory(vul)
add_subdirectory(gul)

#add_subdirectory(vultest)
#add_subdirectory(vullbm2d)
#add_subdirectory(vulvoxel)

#add_subdirectory(vulparsetest)

#add_subdirectory(vullbmtesting)
#add_subdirectory(vullbm2dcpu)

add_subdirectory(vulraytracingweekend)

#add_executable(testmain testmain.cpp)
#target_link_libraries(testmain PRIVATE fmt::fmt)
#add_subdirectory(external/enkiMI)
#add_subdirectory(chunk_compression_testing)

add_subdirectory(vulclouds)
add_subdirectory(cudaexample)
#add_subdirectory(vuladdexample)
#add_subdirectory(vultext)
add_subdirectory(vulvoxel)
add_subdirectory(vulgridworld)
add_subdirectory(vulantworld)
add_subdirectory(hlspv_lang)
add_subdirectory(vullifeengine)
add_subdirectory(vulbreakout)