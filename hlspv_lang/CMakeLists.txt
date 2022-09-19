cmake_minimum_required(VERSION 3.17)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)
add_executable(hlspv_lang main.cpp token_type.h token.cpp token.cpp token.h scan.cpp scan.h parse.cpp parse.h expression.cpp expression.h)

target_include_directories(hlspv_lang
        PRIVATE
        ./
        )

target_link_libraries(hlspv_lang
        PRIVATE
        uul
        PUBLIC
        glm
        Vulkan::Vulkan
        fmt::fmt
        VulkanMemoryAllocator
        Microsoft.GSL::GSL
        range-v3::range-v3
        pugixml::pugixml
        )

target_compile_options(hlspv_lang PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /w14640
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
        > )