cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vullbm2dcpu ./main.cpp bitmask.cpp bitmask.h lbmcpu.cpp lbmcpu.h)
target_link_libraries(vullbm2dcpu PRIVATE vul gul imgui)
target_compile_options(vullbm2dcpu PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_postbuild(vullbm2dcpu
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_depth.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_depth.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/lbm2dcpu_test.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/lbm2dcpu_test.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/lbm2dcpu.comp)

