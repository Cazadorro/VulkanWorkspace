cmake_minimum_required(VERSION 3.23)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 23)

add_executable(openal_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        openal_util.cpp
        openal_util.h
        openal_bool.h
        openal_context_attribute_builder.cpp
        openal_context_attribute_builder.h
        openal_device.cpp
        openal_device.h
        openal_context.cpp
        openal_context.h
        openal_listener.cpp
        openal_listener.h
        openal_buffer.cpp
        openal_buffer.h
        openal_source.cpp
        openal_source.h
        openal_auxilary_effects_slots.cpp
        openal_auxilary_effects_slots.h
        openal_effect.cpp
        openal_effect.h
        openal/alc.cpp
        openal/alc.h
        openal/utils.cpp
        openal/utils.h
        openal/al.cpp
        openal/al.h)

target_include_directories(openal_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}./)
target_compile_features(openal_test PRIVATE cxx_std_20)
target_link_libraries(openal_test PRIVATE
        OpenAL::OpenAL
        fmt::fmt
        range-v3::range-v3
        tl::expected
        Microsoft.GSL::GSL
        glm
        uul
)
target_compile_options(openal_test PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>
        $<$<CXX_COMPILER_ID:MSVC>:/W4
        /w14640
        /w14242
        /w14254
        /w14263
        /w14265
        /w14287
        /we4289
        /w14296
        /w14311
        /w14545
        /w14546
        /w14547
        /w14549
        /w14555
        /w14619
        /w14640
        /w14826
        /w14905
        /w14906
        /w14928
        /std:c++latest
        /Zc:__cplusplus
        > )
