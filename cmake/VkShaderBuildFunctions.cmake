set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
set(GLSLC "$ENV{VULKAN_SDK}/Bin/glslc")
set(SPIRV_OPT "$ENV{VULKAN_SDK}/Bin/spirv-opt")

function(target_shader_postbuild SHADER_TARGET)
    #set_target_properties(${TARGET} PROPERTIES SOURCES ${SHADER_LIST})
    foreach (SHADER_SOURCE_FILEPATH ${ARGN})
        get_filename_component(SHADER_SOURCE_FILENAME ${SHADER_SOURCE_FILEPATH} NAME)
        set(SHADER_TARGET_NAME "${SHADER_TARGET}_${SHADER_SOURCE_FILENAME}")
        set(SHADER_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/spirv")
        set(SHADER_BINARY_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.test.spv")
        set(SHADER_FINAL_BINARY_FILEPATH "${SHADER_BINARY_DIRECTORY}/${SHADER_SOURCE_FILENAME}.spv")
        #        add_custom_target(${SHADER_TARGET_NAME} DEPENDS ${SHADER_FINAL_BINARY_FILEPATH})
        #        add_dependencies(${SHADER_TARGET} ${SHADER_TARGET_NAME})
        #        add_custom_command(
        #
        #                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIRECTORY}
        ##                COMMAND ${GLSL_VALIDATOR} -V ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH}
        #                COMMAND ${GLSLC}  ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH} --target-env=vulkan1.2 -I ${CMAKE_SOURCE_DIR}/shaderutils
        #                COMMAND ${SPIRV_OPT} -O ${SHADER_BINARY_FILEPATH} -o ${SHADER_FINAL_BINARY_FILEPATH}  --skip-validation
        #                DEPENDS ${SHADER_SOURCE_FILEPATH}
        #                OUTPUT ${SHADER_FINAL_BINARY_FILEPATH}
        #                COMMENT "Compiling SPIRV for \nsource: \n\t${SHADER_SOURCE_FILEPATH} \nbinary: \n\t${SHADER_BINARY_FILEPATH} \n"
        #                )

        add_custom_command(
                TARGET ${SHADER_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIRECTORY}
                #                COMMAND ${GLSL_VALIDATOR} -V ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH}
                COMMAND ${GLSLC}  ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH} --target-env=vulkan1.2 -I ${CMAKE_SOURCE_DIR}/shaderutils
                COMMAND ${SPIRV_OPT} -O ${SHADER_BINARY_FILEPATH} -o ${SHADER_FINAL_BINARY_FILEPATH}  --skip-validation
                DEPENDS ${SHADER_SOURCE_FILEPATH}
                BYPRODUCTS ${SHADER_FINAL_BINARY_FILEPATH}
                COMMENT "Compiling SPIRV for \nsource: \n\t${SHADER_SOURCE_FILEPATH} \nbinary: \n\t${SHADER_BINARY_FILEPATH} \n"
        )
        #        add_custom_command(
        #                #                TARGET ${SHADER_TARGET} PRE_LINK
        ##                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIRECTORY}
        #                #                COMMAND ${GLSL_VALIDATOR} -V ${SHADER_SOURCE_FILEPATH} -o ${SHADER_BINARY_FILEPATH}
        #                COMMAND ${SPIRV_OPT}  spirv-opt -O ${SHADER_BINARY_FILEPATH}
        #                DEPENDS ${SHADER_BINARY_FILEPATH}
        #                OUTPUT ${SHADER_BINARY_FILEPATH}
        #                COMMENT "Optimizing SPIRV for \nbinary_input: \n\t${SHADER_BINARY_FILEPATH} \nbinary_output: \n\t${SHADER_BINARY_FILEPATH} \n"
        #        )
    endforeach (SHADER_SOURCE_FILEPATH)
endfunction()