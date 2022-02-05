set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
set(GLSLC "$ENV{VULKAN_SDK}/Bin/glslc")
set(SPIRV_OPT "$ENV{VULKAN_SDK}/Bin/spirv-opt")
set(GLSL_REGEX_MULTI_LINE_COMMENT "/\\*[^(/\\*)]*\\*/")
set(GLSL_REGEX_SINGLE_LINE_COMMENT "//[^\n]*\n")
set(GLSL_REGEX_ANY_COMMENT "(${GLSL_REGEX_MULTI_LINE_COMMENT})|(${GLSL_REGEX_SINGLE_LINE_COMMENT})")
set(GLSL_REGEX_FILE_NAME_IDENTIFIER "[A-Za-z_]+[A-Za-z0-9_]*")
set(GLSL_REGEX_FULL_FILE_IDENTIFIER "${GLSL_REGEX_FILE_NAME_IDENTIFIER}(\\.${GLSL_REGEX_FILE_NAME_IDENTIFIER})*")
set(GLSL_REGEX_INCLUDE_STATEMENT "#include[ ]+\"(${GLSL_REGEX_FULL_FILE_IDENTIFIER})\"")
function(target_shader_postbuild SHADER_TARGET)
    #set_target_properties(${TARGET} PROPERTIES SOURCES ${SHADER_LIST})
    foreach (SHADER_SOURCE_FILEPATH ${ARGN})
        get_filename_component(SHADER_SOURCE_FILENAME ${SHADER_SOURCE_FILEPATH} NAME)
        get_filename_component(SHADER_SOURCE_DIRECTORY ${SHADER_SOURCE_FILEPATH} DIRECTORY)
        set(SHADER_TARGET_NAME "${SHADER_TARGET}_${SHADER_SOURCE_FILENAME}")
        set(SHADER_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/spirv")
        set(SHADER_BINARY_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.test.spv")
        set(SHADER_FINAL_BINARY_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.spv")
        set(SHADER_SOURCE_INCLUDE_DIRECTORY_LIST ${SHADER_SOURCE_DIRECTORY}/ ${CMAKE_SOURCE_DIR}/shaderutils/)

        if(${SHADER_SOURCE_FILENAME} STREQUAL "raytrace.comp")
            file(READ ${SHADER_SOURCE_FILEPATH} SHADER_SOURCE_FILE)
            string(REGEX REPLACE ${GLSL_REGEX_ANY_COMMENT} "" SHADER_SOURCE_FILE ${SHADER_SOURCE_FILE})
            string(REGEX MATCHALL ${GLSL_REGEX_INCLUDE_STATEMENT} INCLUDE_STATEMENT_LIST ${SHADER_SOURCE_FILE})
#            list(LENGTH INCLUDE_STATEMENT_LIST  INCLUDE_STATEMENT_LIST_LENGTH)
#            message("LIST LENGTH ${INCLUDE_STATEMENT_LIST_LENGTH}")
            list(TRANSFORM INCLUDE_STATEMENT_LIST REPLACE "${GLSL_REGEX_INCLUDE_STATEMENT}" "\\1")
            message("DIR ${SHADER_SOURCE_INCLUDE_DIRECTORY_LIST}")
            foreach (INCLUDED_SHADER_TEMP IN ITEMS ${INCLUDE_STATEMENT_LIST})
                message("TEST ${INCLUDED_SHADER_TEMP}")
                find_file (INCLUDED_SHADER_PATH_X NAMES ${INCLUDED_SHADER_TEMP} PATHS ${SHADER_SOURCE_INCLUDE_DIRECTORY_LIST} NO_DEFAULT_PATH)
                if("${INCLUDED_SHADER_PATH_X}" STREQUAL "")
                    message("SHADER_NOT_FOUND ${INCLUDED_SHADER_TEMP}")
                else()
                    message("SHADER_FOUND ${INCLUDED_SHADER_PATH}")
                endif()
                set(INCLUDED_SHADER_PATH_X "")
#                foreach(DIRECTORY IN ITEMS ${SHADER_SOURCE_INCLUDE_DIRECTORY_LIST})
#                    message("\t shader: ${INCLUDED_SHADER}")
#                endforeach()
            endforeach()
            message("HELLO324234 ${INCLUDE_STATEMENT_LIST}")
#            foreach(INCLUDE_STATEMENT IN ITEMS ${INCLUDE_STATEMENT_LIST})
#                string(REGEX MATCH ${GLSL_REGEX_INCLUDE_STATEMENT} INCLUDE_STATEMENT ${INCLUDE_STATEMENT})
#            endforeach()
#            message("HELLO!? ${CMAKE_MATCH_1}")
#            message("HELLO!? ${SHADER_SOURCE_FILE}")
            file(STRINGS ${SHADER_SOURCE_FILEPATH} INCLUDED_SHADERS REGEX "^[ ]*(#include \"([[A-Za-z]+[A-Za-z0-9_]*)(\\.)(glsl)\")")
            #        file(STRINGS ${SHADER_SOURCE_FILEPATH} INCLUDED_SHADERS REGEX "((#include ))")
            message("included shaders:\n ${INCLUDED_SHADERS}")
            foreach (INCLUDED_SHADER IN ITEMS ${INCLUDED_SHADERS})
                message("\t shader: ${INCLUDED_SHADER}")
            endforeach()

        endif()
        #we can use depfiles instead
        #https://stackoverflow.com/questions/60420700/cmake-invocation-of-glslc-with-respect-to-includes-dependencies
        add_custom_command(
                #                TARGET ${SHADER_TARGET} POST_BUILD
                OUTPUT ${SHADER_FINAL_BINARY_FILEPATH}
                DEPENDS ${SHADER_SOURCE_FILEPATH}
                DEPFILE ${SHADER_SOURCE_FILEPATH}.d
                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIRECTORY}
                #                COMMAND ${GLSL_VALIDATOR} -V ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH}
                COMMAND ${GLSLC} -MD -MF ${SHADER_SOURCE_FILEPATH}.d -O ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH} --target-env=vulkan1.2 -I ${CMAKE_SOURCE_DIR}/shaderutils
                COMMAND ${SPIRV_OPT} -O ${SHADER_BINARY_FILEPATH} -o ${SHADER_FINAL_BINARY_FILEPATH}  --skip-validation
                DEPENDS ${SHADER_SOURCE_FILEPATH}
                BYPRODUCTS ${SHADER_FINAL_BINARY_FILEPATH}
                COMMENT "Compiling SPIRV for \nsource: \n\t${SHADER_SOURCE_FILEPATH} \nbinary: \n\t${SHADER_BINARY_FILEPATH} \n"
        )

        add_custom_target(${SHADER_TARGET_NAME} DEPENDS ${SHADER_FINAL_BINARY_FILEPATH})
        add_dependencies(${SHADER_TARGET} ${SHADER_TARGET_NAME})

    endforeach (SHADER_SOURCE_FILEPATH)
endfunction()