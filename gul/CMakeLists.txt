cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_library(gul STATIC gul/imguirenderer.cpp gul/imguirenderer.h gul/kdtree.cpp gul/kdtree.h gul/geometry.cpp gul/geometry.h gul/noise/opensimplex.cpp gul/noise/opensimplex.h gul/noise/fbm.h gul/noise/fbm.cpp)
target_sources(gul
        PRIVATE
#        ./gul/glfwwindow.cpp
#        ./gul/glfwwindow.h
        ./gul/stbimage.cpp
        ./gul/stbimage.h
        ./gul/firstpersoncamera.cpp
        ./gul/firstpersoncamera.h
        ./gul/bitmask.cpp
        ./gul/bitmask.h
        ./gul/collision.cpp
        ./gul/collision.h
        )
target_include_directories(gul
        PUBLIC
        ./)

target_link_libraries(gul
        PUBLIC
        fmt::fmt
        Vulkan::Vulkan
        glm::glm
        glfw
        czdr::vkutil
        czdr::glfw-wrapper
        stb::stb
        imgui)


target_compile_options(gul PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic>)