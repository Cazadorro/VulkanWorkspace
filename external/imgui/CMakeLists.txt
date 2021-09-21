cmake_minimum_required(VERSION 3.13)
project(imgui)

include(FetchContent)

#FetchContent_Declare(
#        imguigithub
#        GIT_REPOSITORY https://github.com/ocornut/imgui.git
#        GIT_TAG origin/docking
##        GIT_TAG master
#        DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui
#        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui
#
#        CONFIGURE_COMMAND ""
#        BUILD_COMMAND ""
##        INSTALL_COMMAND ""
#)
##FetchContent_MakeAvailable(ImguiGithub)
#FetchContent_GetProperties(imguigithub)
#
#if(NOT imguigithub_POPULATED)
#    message(STATUS "Fetching IMGUI")
#    FetchContent_Populate(imguigithub)
#endif()

find_package(Vulkan REQUIRED)

add_library(imgui STATIC)
target_sources(imgui PRIVATE
        ./imgui/imgui.cpp
        ./imgui/imgui.h
        ./imgui/imgui_demo.cpp
        ./imgui/imgui_draw.cpp
        ./imgui/imgui_internal.h
        ./imgui/imgui_widgets.cpp
        ./imgui/imgui_tables.cpp
        ./imgui/imstb_rectpack.h
        ./imgui/imstb_textedit.h
        ./imgui/imstb_truetype.h
        ./imgui/backends/imgui_impl_glfw.cpp
        ./imgui/backends/imgui_impl_vulkan.cpp
        )
target_include_directories(imgui
        PUBLIC
        ./imgui/
        ./imgui/examples/
        ./imgui/backends/
        )

target_link_libraries(imgui PUBLIC
        glfw
        Vulkan::Vulkan
        -limm32
        )
