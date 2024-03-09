cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)


add_executable(vulpaint ./main.cpp)
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
        moodycamel::readerwriterqueue
        )
target_compile_options(vulpaint PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)
CZDR_CMAKE_UTILS_SET_MSVC_RUNTIME_LIBRARY(vulpaint)
target_compile_options(vulpaint
        PRIVATE ${CZDR_CMAKE_UTILS_GENERATED_EXTRA_AND_CONFIG_COMPILE_ARGS}
)

target_shader_postbuild(vulpaint
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_fullscreen_texture.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_fullscreen_texture.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/pre_process_updated_brush_strokes.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/render_updated_brush_strokes.comp)

