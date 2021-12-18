cmake_minimum_required(VERSION 3.19)
project(VulkanWorkspace)
#TODO cite goodvibes texture pack for textures
set(CMAKE_CXX_STANDARD 17)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(VkShaderBuildFunctions)
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld")

find_package(Vulkan REQUIRED)
add_subdirectory(external/glm)
target_compile_definitions(glm INTERFACE
        -DGLM_FORCE_RADIANS
        -DGLM_FORCE_DEPTH_ZERO_TO_ONE
        -DGLM_FORCE_LEFT_HANDED
        -DGLM_FORCE_SWIZZLE
        )
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
add_subdirectory(external/glfw)
target_compile_definitions(glfw INTERFACE
        -DGLFW_INCLUDE_VULKAN
        )
option(TINYGLTF_BUILD_LOADER_EXAMPLE "" OFF)
option(TINYGLTF_HEADER_ONLY "" OFF)
add_subdirectory(external/tinygltf)
add_subdirectory(external/cxxopts)
add_subdirectory(external/fmt)
add_subdirectory(external/gsl)
add_subdirectory(external/imgui)
add_subdirectory(external/json)
add_subdirectory(external/pugixml)
add_subdirectory(external/range)
add_subdirectory(external/vma)
add_subdirectory(external/stb)

add_subdirectory(external/glslang)

add_subdirectory(vulgenerated)
add_subdirectory(vul)
add_subdirectory(gul)
add_subdirectory(vultest)
add_subdirectory(vullbm2d)
add_subdirectory(vulvoxel)
#add_subdirectory(vulparsetest)
add_subdirectory(vullbmtesting)
add_subdirectory(vullbm2dcpu)
add_subdirectory(vulraytracingweekend)

#add_subdirectory(external/enkiMI)
#add_subdirectory(chunk_compression_testing)

