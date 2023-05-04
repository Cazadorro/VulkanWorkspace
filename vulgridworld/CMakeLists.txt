cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_executable(vulgridworld
        ./main.cpp
#        gridworld.cpp
#        gridworld.h
        )
target_link_libraries(vulgridworld PRIVATE uul vul gul imgui glm)
target_compile_options(vulgridworld PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_postbuild(vulgridworld
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.frag)

