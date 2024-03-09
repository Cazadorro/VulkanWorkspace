cmake_minimum_required(VERSION 3.21)
project(VulkanWorkspace)

set(CMAKE_CXX_STANDARD 20)

add_executable(vulclouds ./main.cpp bvh.cpp bvh.h)
target_link_libraries(vulclouds PRIVATE
        czdr::vkutil
        gul
        imgui
        glm::glm
        GPUOpen::VulkanMemoryAllocator
        czdr::stdutil
        czdr::bitutil
        czdr::glfw-wrapper)
target_compile_options(vulclouds PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g -ggdb -fuse-ld=lld>)

#target_shader_postbuild(vulraytracingweekend
#        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytrace.comp
#        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.frag
#        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.vert
#        )



#
#project(test)

#function(target_shader_function TARGET)
#    find_package(Vulkan REQUIRED)
#
#    if (NOT TARGET Vulkan::glslc)
#        message(FATAL_ERROR "Could not find glslc")
#    endif ()
#
#    foreach (source IN LISTS ARGN)
#        cmake_path(ABSOLUTE_PATH source OUTPUT_VARIABLE source_abs)
#        cmake_path(GET source FILENAME basename)
#
#        set(depfile "spirv/${basename}.d")
#        set(output "spirv/${basename}.spv")
#        set(dirs "$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>")
#        set(include_flags "$<$<BOOL:${dirs}>:-I$<JOIN:${dirs},;-I>>")
#
#        add_custom_command(
#                OUTPUT "${output}"
#                COMMAND "${CMAKE_COMMAND}" -E make_directory spirv
#                COMMAND Vulkan::glslc -MD -MF "${depfile}" -O "${source_abs}"
#                -o "${output}" --target-env=vulkan1.2 "${include_flags}"
#                DEPENDS "${source_abs}"
#                BYPRODUCTS "${depfile}"
#                COMMENT "Compiling SPIRV: ${source} -> ${output}"
#                DEPFILE "${depfile}"
#                VERBATIM
#                COMMAND_EXPAND_LISTS
#        )
#
#        set(shader_target "${TARGET}_${basename}")
#        add_custom_target("${shader_target}"
#                DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${output}")
#        add_dependencies("${TARGET}" "${shader_target}")
#    endforeach ()
#endfunction()



target_shader_function(vulclouds
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cloudmarch.comp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fullscreen.vert
        )
#target_shader_function(vulraytracingweekend shaders/example.comp)
target_include_directories(vulclouds PRIVATE "${CMAKE_SOURCE_DIR}/shaderutils")