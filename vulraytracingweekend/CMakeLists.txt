cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(vulraytracingweekend ./main.cpp bitmask.cpp bitmask.h)
target_link_libraries(vulraytracingweekend PRIVATE vul gul imgui)
target_compile_options(vulraytracingweekend PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

target_shader_postbuild(vulraytracingweekend
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytrace.comp
        )

