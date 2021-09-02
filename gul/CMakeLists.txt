cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_library(gul STATIC gul/stbimage.cpp gul/stbimage.h)
target_sources(gul
        PRIVATE
        ./gul/glfwwindow.cpp
        ./gul/glfwwindow.h
        )
target_include_directories(gul
        PUBLIC
        ./)

target_link_libraries(gul
        PRIVATE

        PUBLIC
        fmt::fmt
        Vulkan::Vulkan
        glm
        glfw
        vul
        stb::stb)


target_compile_options(gul PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic>)