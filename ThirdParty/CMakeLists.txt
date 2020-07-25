# Download submodules
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt OR
   NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glm/CMakeLists.txt OR
   NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt OR
   NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/OpenCL-Headers/README.md)
    message(STATUS "Cloning submodules")
    execute_process(COMMAND git submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
else()
    message(STATUS "All submodules present")
endif()

# Paths for header-only dependencies
set_with_parent_scope(DIR_OPENCL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/OpenCL-Headers)
set_with_parent_scope(DIR_STB ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)

# Paths for built dependencies
set_with_parent_scope(DIR_GTEST ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
set_with_parent_scope(DIR_GLAD ${CMAKE_CURRENT_SOURCE_DIR}/glad)
set_with_parent_scope(DIR_GLFW ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set_with_parent_scope(DIR_GLM ${CMAKE_CURRENT_SOURCE_DIR}/glm)

# Add dependencies to the project
set(gtest_force_shared_crt on)
set(BUILD_GMOCK off)
add_subdirectory(${DIR_GTEST})
add_subdirectory(${DIR_GLAD})
set(GLFW_BUILD_DOCS off)
set(GLFW_INSTALL off)
add_subdirectory(${DIR_GLFW})
set(BUILD_STATIC_LIBS on)
add_subdirectory(${DIR_GLM})
