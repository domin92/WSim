# Global general settings
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
cmake_minimum_required(VERSION 3.12.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Helper macro for setting variables for both current and parent scope
macro(set_with_parent_scope VARIABLE VALUE)
    set(${VARIABLE} ${VALUE})
    set(${VARIABLE} ${VALUE} PARENT_SCOPE)
endmacro()

# Paths
set(DIR_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DIR_WSIM ${CMAKE_CURRENT_SOURCE_DIR}/WSim)
set(DIR_THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

# Define top level project and add targets to it
project(WSimWithDependencies)
add_subdirectory(${DIR_THIRD_PARTY})
add_subdirectory(${DIR_WSIM})
set_directory_properties(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTIES VS_STARTUP_PROJECT ${TARGET_WSIM_STANDALONE})
