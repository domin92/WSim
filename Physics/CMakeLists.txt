# Global general settings
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
cmake_minimum_required(VERSION 3.0.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Paths
set(DIR_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DIR_WSIM ${CMAKE_CURRENT_SOURCE_DIR}/WSim)
set(DIR_GTEST ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/googletest)
set(DIR_FREEGLUT ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/FreeGLUT/freeglut/freeglut)

# Target names
set(TARGET_WSIM_CORE_LIB WSimCore)
set(TARGET_WSIM WSim)
set(TARGET_WSIM_TESTS WSimTests)

# Define project and subprojects
project(WSimWithDependencies)
set(gtest_force_shared_crt on)
add_subdirectory(${DIR_GTEST})
add_subdirectory(${DIR_FREEGLUT})
add_subdirectory(${DIR_WSIM})

# Set startup project
set_directory_properties(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTIES VS_STARTUP_PROJECT WSim)
