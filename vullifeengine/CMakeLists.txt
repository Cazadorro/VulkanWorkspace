cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_executable(vullifeengine
        ./main.cpp
#        gridworld.cpp
#        gridworld.h
        lifeenginegrid.cpp lifeenginegrid.h)
target_link_libraries(vullifeengine PRIVATE uul vul gul imgui glm pcg_cpp)
target_compile_options(vullifeengine PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_postbuild(vullifeengine
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/antkernel.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.frag)

