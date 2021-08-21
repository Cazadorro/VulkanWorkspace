cmake_minimum_required(VERSION 3.13)
project(stb)

include(FetchContent)

FetchContent_Declare(
        stbimagefile
        URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
        DOWNLOAD_NO_EXTRACT TRUE
        DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
FetchContent_Declare(
        stbimagewritefile
        URL https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
        DOWNLOAD_NO_EXTRACT TRUE
        DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
FetchContent_MakeAvailable(stbimagefile stbimagewritefile)

add_library(stb STATIC)
target_sources(stb PRIVATE
        stb_image.h
        stb_image.cpp
        stb_image_write.h
        stb_image_write.cpp)

target_include_directories(stb PUBLIC
        ./)
add_library(stb::stb ALIAS stb)