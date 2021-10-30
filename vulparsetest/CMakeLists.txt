cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vulparsetest ./main.cpp cpu_bitmask_intersect.cpp cpu_bitmask_intersect.h chunkmanagement.cpp chunkmanagement.h)
target_link_libraries(vulparsetest PRIVATE vul gul imgui glm glslang glslang-default-resource-limits)
target_compile_options(vulparsetest PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_postbuild(vulparsetest
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/rle_render.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/rle_render.frag)

