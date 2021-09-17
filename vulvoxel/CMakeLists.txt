cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vulvoxel ./main.cpp)
target_link_libraries(vulvoxel PRIVATE vul gul imgui glm)
target_compile_options(vulvoxel PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_postbuild(vulvoxel
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/rle_render.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/rle_render.frag)

