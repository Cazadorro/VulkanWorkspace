cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vulgenerated )

target_sources(vulgenerated
        PRIVATE
        main.cpp
        casestrings.cpp
        casestrings.h
        enuminfo.cpp
        enuminfo.h
        codegenutils.cpp
        codegenutils.h
        enumcodegen.cpp
        enumcodegen.h
        featurecodegen.cpp
        featurecodegen.h
        vknameutils.cpp
        vknameutils.h
        )

get_filename_component(VUL_VULKAN_VERSION $ENV{VULKAN_SDK} NAME_WLE)
message(${VUL_VULKAN_VERSION})
file(DOWNLOAD
https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v${VUL_VULKAN_VERSION}/xml/vk.xml
${CMAKE_CURRENT_SOURCE_DIR}/resources/vk.xml)

target_include_directories(vulgenerated
        PRIVATE
        ./
        )

target_link_libraries(vulgenerated
        PRIVATE
        glfw
        Vulkan::Vulkan
        glm
        stb::stb
        fmt::fmt
        VulkanMemoryAllocator
        Microsoft.GSL::GSL
        tinygltf
        range-v3::range-v3
        pugixml::pugixml
        )

target_compile_options(vulgenerated PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb>)
target_link_options(vulgenerated PUBLIC -g -ggdb)

target_compile_definitions(vulgenerated PRIVATE -DVUL_GEN_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

#glslext_target_generate_add(vkjupiter2 PRIVATE
#        SOURCE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
#        DSETLAYOUT_FILEPATHS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/lbmvertexcolor.dsetlayout)
#glslext_convert_shaders(
#        vkjupiter2
#        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
#        INCLUDE_DIRECTORIES
#            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
#        DSET_SHADER_SOURCES
#            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag.dset
#            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert.dset)
#
#target_shader_postbuild(vkjupiter2
#        ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/screen_shader.vert
#        ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/depth_example.vert
#        ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/depth_example.frag
#
#        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag
#        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert
#
#        )
#

#add_subdirectory(sources/)