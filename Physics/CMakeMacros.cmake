# -------------------------------------------------------------------------------- Accumulating sources in global properties and collecting them in one command

macro(add_sources TARGET_NAME)
    string(CONCAT PROPERTY_NAME "SourceFilesFor_" ${TARGET_NAME})
    set_property(GLOBAL APPEND PROPERTY ${PROPERTY_NAME} ${ARGN})
endmacro()

macro(add_sources_and_cmake_file TARGET_NAME)
    add_sources(${TARGET_NAME} ${ARGN} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
endmacro()

function(add_shaders_and_cmake_file TARGET_NAME)
    add_sources_and_cmake_file(${TARGET_NAME})

    string(CONCAT PATH_PROPERTY_NAME "ShaderPathsFor_" ${TARGET_NAME})
    string(CONCAT TYPE_PROPERTY_NAME "ShaderTypesFor_" ${TARGET_NAME})
    math(EXPR maxIndex "${ARGC} - 1")
    foreach(index RANGE 1 ${maxIndex} 2)
        set(path "${ARGV${index}}")
        math(EXPR index2 "${index} + 1")
        set(type "${ARGV${index2}}")

        add_sources(${TARGET_NAME} ${path})
        set_property(GLOBAL APPEND PROPERTY ${PATH_PROPERTY_NAME} ${path})
        set_property(GLOBAL APPEND PROPERTY ${TYPE_PROPERTY_NAME} ${type})
    endforeach()
endfunction()

macro (add_models_and_cmake_file TARGET_NAME)
    add_sources_and_cmake_file(${TARGET_NAME})
    string(CONCAT PROPERTY_NAME "ModelFilesFor_" ${TARGET_NAME})
    set_property(GLOBAL APPEND PROPERTY ${PROPERTY_NAME} ${ARGN})
    add_sources(${TARGET_NAME} ${ARGN})
endmacro()

macro(collect_sources VAR TARGET_NAME)
    string(CONCAT PROPERTY_NAME "SourceFilesFor_" ${TARGET_NAME})
    get_property(${VAR} GLOBAL PROPERTY ${PROPERTY_NAME})
endmacro()

macro(target_configure_non_cpp_files TARGET_NAME TARGET_CONTAINING_FILES COMPILATION_ENABLED)
    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "ERROR: target_non_cpp_files_properties must be called after target definition")
    endif()

    # Models
    string(CONCAT MODELS_PROPERTY_NAME "ModelFilesFor_" ${TARGET_CONTAINING_FILES})
    get_property(MODELS GLOBAL PROPERTY ${MODELS_PROPERTY_NAME})
    foreach (MODEL_PATH ${MODELS})
        set_source_files_properties(${MODEL_PATH} PROPERTIES HEADER_FILE_ONLY TRUE)
    endforeach()

    # Shaders
    string(CONCAT PATH_PROPERTY_NAME "ShaderPathsFor_" ${TARGET_CONTAINING_FILES})
    string(CONCAT TYPE_PROPERTY_NAME "ShaderTypesFor_" ${TARGET_CONTAINING_FILES})
    get_property(paths GLOBAL PROPERTY ${PATH_PROPERTY_NAME})
    get_property(types GLOBAL PROPERTY ${TYPE_PROPERTY_NAME})
    list(LENGTH paths length)
    math(EXPR length "${length} - 1")
    foreach(index RANGE ${length})
        list(GET paths ${index} path)
        list(GET types ${index} type)
        if(${COMPILATION_ENABLED})
            set_source_files_properties(${path} PROPERTIES VS_SHADER_TYPE ${type} VS_SHADER_MODEL 5.0)
        else()
            set_source_files_properties(${path} PROPERTIES HEADER_FILE_ONLY TRUE)
        endif()
    endforeach()
endmacro()

# -------------------------------------------------------------------------------- Target configuring helpers

macro(target_configure_general_properties TARGET_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "/MP" )
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    set_target_properties(${TARGET_NAME} PROPERTIES
       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # .lib
       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # .dll
       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # .exe
       PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin" # .pdb
    )
endmacro()

macro(target_configure_models_directory TARGET_NAME)
    validate_parameters(MODELS_DIR)
    target_compile_definitions(${TARGET_NAME} PRIVATE MODELS_DIR="${MODELS_DIR}")
endmacro()

macro(target_solution_folder TARGET_NAME FOLDER_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${FOLDER_NAME})
endmacro()

# -------------------------------------------------------------------------------- Helpers for configuring paths

macro(get_library_name_with_suffix NAME BASE_NAME)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(${NAME} "${BASE_NAME}d.lib")
    else()
        set(${NAME} "${BASE_NAME}.lib")
    endif()
endmacro()
© 2020 GitHub, Inc.
Terms
Privacy
Security
Status
Help
Contact GitHub
Pricing
API
Training
Blog
About
