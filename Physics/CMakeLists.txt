# General setup
cmake_minimum_required(VERSION 3.0.0)
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
project(WSim)

# Find dependencies
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT)
if (GLUT_FOUND)
    set(GLUT_LIB GLUT::GLUT)
else()
    find_package(FREEGLUT REQUIRED)
    set(GLUT_LIB FreeGLUT::freeglut_static)
endif()


add_subdirectory(Source)
add_subdirectory(Tests)

# Set startup project
set_directory_properties(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTIES VS_STARTUP_PROJECT WSim)
