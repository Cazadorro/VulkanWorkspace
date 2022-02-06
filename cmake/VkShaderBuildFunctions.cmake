set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
set(GLSLC "$ENV{VULKAN_SDK}/Bin/glslc")
set(SPIRV_OPT "$ENV{VULKAN_SDK}/Bin/spirv-opt")
#set(GLSL_REGEX_MULTI_LINE_COMMENT "/\\*[^(/\\*)]*\\*/")
#set(GLSL_REGEX_SINGLE_LINE_COMMENT "//[^\n]*\n")
#set(GLSL_REGEX_ANY_COMMENT "(${GLSL_REGEX_MULTI_LINE_COMMENT})|(${GLSL_REGEX_SINGLE_LINE_COMMENT})")
#set(GLSL_REGEX_FILE_NAME_IDENTIFIER "[A-Za-z_]+[A-Za-z0-9_]*")
#set(GLSL_REGEX_FULL_FILE_IDENTIFIER "${GLSL_REGEX_FILE_NAME_IDENTIFIER}(\\.${GLSL_REGEX_FILE_NAME_IDENTIFIER})*")
#set(GLSL_REGEX_INCLUDE_STATEMENT "#include[ ]+\"(${GLSL_REGEX_FULL_FILE_IDENTIFIER})\"")
function(target_shader_postbuild SHADER_TARGET)
    #set_target_properties(${TARGET} PROPERTIES SOURCES ${SHADER_LIST})
    foreach (SHADER_SOURCE_FILEPATH ${ARGN})
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE_FILEPATH OUTPUT_VARIABLE SHADER_SOURCE_FILEPATH_ABS)
        get_filename_component(SHADER_SOURCE_FILENAME ${SHADER_SOURCE_FILEPATH} NAME)
        get_filename_component(SHADER_SOURCE_DIRECTORY ${SHADER_SOURCE_FILEPATH} DIRECTORY)
        set(SHADER_TARGET_NAME "${SHADER_TARGET}_${SHADER_SOURCE_FILENAME}")
        set(SHADER_BINARY_DIRECTORY "spirv")
        set(SHADER_BINARY_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.test.spv")
        set(SHADER_DEPFILE_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.d")
        set(SHADER_FINAL_BINARY_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.spv")
        set(SHADER_SOURCE_INCLUDE_DIRECTORY_LIST ${SHADER_SOURCE_DIRECTORY}/ ${CMAKE_SOURCE_DIR}/shaderutils/)


        #we can use depfiles instead
        #https://stackoverflow.com/questions/60420700/cmake-invocation-of-glslc-with-respect-to-includes-dependencies
        add_custom_command(
                #                TARGET ${SHADER_TARGET} POST_BUILD
                OUTPUT ${SHADER_FINAL_BINARY_FILEPATH}

                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIRECTORY}
                #                COMMAND ${GLSL_VALIDATOR} -V ${SHADER_SOURCE_FILEPATH_ABS} -o ${SHADER_BINARY_FILEPATH}
                COMMAND ${GLSLC} -MD -MF ${SHADER_DEPFILE_FILEPATH} -O ${SHADER_SOURCE_FILEPATH_ABS} -o ${SHADER_FINAL_BINARY_FILEPATH} --target-env=vulkan1.2 -I ${CMAKE_SOURCE_DIR}/shaderutils
#                COMMAND ${SPIRV_OPT} -O ${SHADER_BINARY_FILEPATH} -o ${SHADER_FINAL_BINARY_FILEPATH}  --skip-validation
                DEPENDS ${SHADER_SOURCE_FILEPATH_ABS}
                DEPFILE  ${SHADER_DEPFILE_FILEPATH}
                BYPRODUCTS ${SHADER_DEPFILE_FILEPATH}
                COMMENT "Compiling SPIRV for \nsource: \n\t${SHADER_SOURCE_FILEPATH_ABS} \nbinary: \n\t${SHADER_BINARY_FILEPATH} \n"
                DEPFILE  ${SHADER_DEPFILE_FILEPATH}
                VERBATIM
                COMMAND_EXPAND_LISTS
        )

#        target_sources(${SHADER_TARGET} PRIVATE ${SHADER_FINAL_BINARY_FILEPATH})
        add_custom_target(${SHADER_TARGET_NAME} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_FINAL_BINARY_FILEPATH})
        add_dependencies(${SHADER_TARGET} ${SHADER_TARGET_NAME})

    endforeach (SHADER_SOURCE_FILEPATH)
endfunction()

function(target_shader_function TARGET)
    find_package(Vulkan REQUIRED)

    if (NOT TARGET Vulkan::glslc)
        message(FATAL_ERROR "Could not find glslc")
    endif ()

    foreach (SHADER_SOURCE_FILEPATH IN LISTS ARGN)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE_FILEPATH OUTPUT_VARIABLE SHADER_SOURCE_FILEPATH_ABS)
        cmake_path(GET SHADER_SOURCE_FILEPATH FILENAME SHADER_FILE_NAME)

        set(GLSLC_DEPFILE "spirv/${SHADER_FILE_NAME}.d")
        set(GLSLC_OUTPUT_SPIRV "spirv/${SHADER_FILE_NAME}.spv")
        set(TARGET_INCLUDE_DIRS "$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>")
        set(SHADER_INCLUDE_FLAGS "$<$<BOOL:${TARGET_INCLUDE_DIRS}>:-I$<JOIN:${TARGET_INCLUDE_DIRS},;-I>>")

        add_custom_command(
                OUTPUT "${GLSLC_OUTPUT_SPIRV}"
                COMMAND "${CMAKE_COMMAND}" -E make_directory spirv
                COMMAND Vulkan::glslc -MD -MF "${GLSLC_DEPFILE}" -O "${SHADER_SOURCE_FILEPATH_ABS}"
                -o "${GLSLC_OUTPUT_SPIRV}" --target-env=vulkan1.2 "${SHADER_INCLUDE_FLAGS}"
                DEPENDS "${SHADER_SOURCE_FILEPATH_ABS}"
                BYPRODUCTS "${GLSLC_DEPFILE}"
                COMMENT "Compiling SPIRV: ${SHADER_SOURCE_FILEPATH} -> ${GLSLC_OUTPUT_SPIRV}"
                DEPFILE "${GLSLC_DEPFILE}"
                VERBATIM
                COMMAND_EXPAND_LISTS
        )

        set(SHADER_TARGET "${TARGET}_${SHADER_FILE_NAME}")
        add_custom_target("${SHADER_TARGET}"
                DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${GLSLC_OUTPUT_SPIRV}")
        add_dependencies("${TARGET}" "${SHADER_TARGET}")
    endforeach ()
endfunction()
