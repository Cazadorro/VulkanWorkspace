cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_executable(chunk_compression_testing ./main.cpp)
target_link_libraries(chunk_compression_testing PRIVATE
        enkiMI::enkimi
        fmt::fmt
        Microsoft.GSL::GSL
        range-v3::range-v3
        cxxopts)
target_compile_options(chunk_compression_testing PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)
