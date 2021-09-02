cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vultest ./main.cpp)
target_link_libraries(vultest PRIVATE vul gul)
target_compile_options(vultest PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb>)

target_shader_postbuild(vultest
        ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/screen_shader.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/screen_shader2.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/boxcloud_render.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cloud_light.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/blur_cloud_light.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cloud_point_light.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/blur_x.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/blur_y.frag)
