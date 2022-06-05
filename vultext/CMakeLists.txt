cmake_minimum_required(VERSION 3.21)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vultext ./main.cpp bvh.cpp bvh.h)
target_link_libraries(vultext PRIVATE vul gul imgui msdfgen::msdfgen-core msdfgen::msdfgen-ext)
target_compile_options(vultext PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)



target_shader_function(vultext
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cloudmarch.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.vert
        )
#target_shader_function(vulraytracingweekend shaders/example.comp)
target_include_directories(vultext PRIVATE "${CMAKE_SOURCE_DIR}/shaderutils")