cmake_minimum_required(VERSION 3.21)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vuladdexample ./main.cpp)
target_link_libraries(vuladdexample PRIVATE vul gul imgui)
target_compile_options(vuladdexample PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_function(vuladdexample
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/add.comp
        )
#target_shader_function(vulraytracingweekend shaders/example.comp)
target_include_directories(vulclouds PRIVATE "${CMAKE_SOURCE_DIR}/shaderutils")