cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
#Universal Utility Library
add_library(uul STATIC uul/array.h uul/bit.h uul/math.h)
target_sources(uul
        PRIVATE
        ./uul/assert.h
        ./uul/unreachable.h
        ./uul/assert.cpp

        )
target_include_directories(uul
        PUBLIC
        ./)

target_link_libraries(uul
        PRIVATE
        fmt::fmt
        )


target_compile_options(uul PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic>)

target_compile_definitions(uul PUBLIC $<$<CONFIG:Debug>:UUL_DEBUG> $<$<CONFIG:Release>:UUL_RELEASE>)