cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)


add_executable(vulbreakout ./main.cpp)
target_link_libraries(vulbreakout PRIVATE
        czdr::vkutil
        gul
        imgui
        glm::glm
        GPUOpen::VulkanMemoryAllocator
        czdr::stdutil
        czdr::bitutil
        czdr::glfw-wrapper
        Freetype::Freetype
        )
target_compile_options(vulbreakout PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)
CZDR_CMAKE_UTILS_SET_MSVC_RUNTIME_LIBRARY(vulbreakout)
target_compile_options(vulbreakout
        PRIVATE ${CZDR_CMAKE_UTILS_GENERATED_EXTRA_AND_CONFIG_COMPILE_ARGS}
)

target_shader_postbuild(vulbreakout
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_sprites_2d.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_sprites_2d.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_fullscreen_texture.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_fullscreen_texture.frag)

