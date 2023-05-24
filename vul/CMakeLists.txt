cmake_minimum_required(VERSION 3.13)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_library(vul STATIC sources/vul/pushconstantrange.cpp sources/vul/pushconstantrange.h sources/vul/deviceaddress.cpp sources/vul/deviceaddress.h sources/vul/submitinfo.cpp sources/vul/submitinfo.h sources/vul/submitinfobuilder.cpp sources/vul/submitinfobuilder.h)
target_sources(vul PRIVATE
        sources/vul/temparrayproxyfwd.h
        sources/vul/instance.cpp
        sources/vul/instance.h
        sources/vul/debugutils.cpp
        sources/vul/debugutils.h
        sources/vul/expectedresult.cpp
        sources/vul/expectedresult.h
        sources/vul/physicaldevice.cpp
        sources/vul/physicaldevice.h
        sources/vul/surface.cpp
        sources/vul/surface.h
        sources/vul/device.cpp
        sources/vul/device.h
        sources/vul/queue.cpp
        sources/vul/queue.h
        sources/vul/vmaallocator.cpp
        sources/vul/vmaallocator.h
        sources/vul/image2d.cpp
        sources/vul/image2d.h
        sources/vul/imageview.cpp
        sources/vul/imageview.h
        sources/vul/swapchain.cpp
        sources/vul/swapchain.h
        sources/vul/semaphore.cpp
        sources/vul/semaphore.h
        sources/vul/renderpass.cpp
        sources/vul/renderpass.h
        sources/vul/colorformatutils.cpp
        sources/vul/colorformatutils.h
        sources/vul/containerutils.cpp
        sources/vul/containerutils.h
        sources/vul/shadermodule.cpp
        sources/vul/shadermodule.h
        sources/vul/vertexinputstate.cpp
        sources/vul/vertexinputstate.h
        sources/vul/struct_to_tuple.h
        sources/vul/repeat_macro.h
        sources/vul/tuple_to_array.h
        sources/vul/graphicspipeline.cpp
        sources/vul/graphicspipeline.h
        sources/vul/descriptorsetlayout.cpp
        sources/vul/descriptorsetlayout.h
        sources/vul/sampler.cpp
        sources/vul/sampler.h
        sources/vul/descriptorpool.cpp
        sources/vul/descriptorpool.h
        sources/vul/descriptorset.cpp
        sources/vul/descriptorset.h
        sources/vul/pipelinelayout.cpp
        sources/vul/pipelinelayout.h
        sources/vul/vkutils.h
        sources/vul/temparrayproxy.h
        sources/vul/pipelinecache.cpp
        sources/vul/pipelinecache.h
        sources/vul/computepipeline.cpp
        sources/vul/computepipeline.h
        sources/vul/commandpool.cpp
        sources/vul/commandpool.h
        sources/vul/commandbuffer.cpp
        sources/vul/commandbuffer.h
        sources/vul/framebuffer.cpp
        sources/vul/framebuffer.h
        sources/vul/buffer.cpp
        sources/vul/buffer.h
        sources/vul/vmaallocation.cpp
        sources/vul/vmaallocation.h
        sources/vul/commandutils.cpp
        sources/vul/commandutils.h
        sources/vul/image.cpp
        sources/vul/image.h
        sources/vul/ioutils.cpp
        sources/vul/ioutils.h
        sources/vul/extensionfunctions.h
        sources/vul/extensionfunctions.cpp
        sources/vul/vkstructutils.cpp
        sources/vul/vkstructutils.h
        sources/vul/tempvoidarrayproxy.cpp
        sources/vul/tempvoidarrayproxy.h
        sources/vul/traitutils.h)
target_sources(vul PRIVATE
#        ./generated_sources/vul/bitmasks.cpp
        ./generated_sources/vul/enums.cpp
        ./generated_sources/vul/features.cpp
        ./generated_sources/vul/objecttypeutils.cpp
#        ./generated_sources/vul/bitmasks.h
        ./generated_sources/vul/enums.h
        ./generated_sources/vul/features.h
        ./generated_sources/vul/bitmasksfwd.h
        ./generated_sources/vul/enumsfwd.h
        ./generated_sources/vul/featuresfwd.h
        ./generated_sources/vul/objecttypeutils.h)
target_include_directories(vul
        PUBLIC
        ./generated_sources/
        ./sources/
        PRIVATE
        ./generated_sources/vul/
        ./sources/vul/
        )

target_link_libraries(vul
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

target_compile_options(vul PRIVATE
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

target_link_options (vul PRIVATE -fuse-ld=lld)
target_compile_features(vul PUBLIC cxx_std_20)
target_compile_definitions(vul PRIVATE $<$<CONFIG:Debug>:VUL_DEBUG> $<$<CONFIG:Release>:VUL_RELEASE>)