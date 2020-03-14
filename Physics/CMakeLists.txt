# General setup
cmake_minimum_required(VERSION 3.15.0)
project(WSim)
find_package(OpenCL REQUIRED)

# Get source files
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*/CMakeLists.txt"
    "${SOURCE_DIR}/*.cmake"
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.cl"
)
source_group (TREE ${SOURCE_DIR} FILES ${SOURCE_FILES})

# Define target
set(TARGET_NAME WSimPhysics)
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME} PRIVATE ${OpenCL_LIBRARY})
target_include_directories(${TARGET_NAME} PRIVATE ${OpenCL_INCLUDE_DIR})
target_compile_definitions(${TARGET_NAME} PRIVATE SHADERS_DIR="${SOURCE_DIR}/Kernels")
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)

# Set startup project
set_directory_properties(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTIES VS_STARTUP_PROJECT ${TARGET_NAME})
