# Find dependencies
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
# Threads::Threads is used by some dependent libraries (such as gtest, glfw),
# but find_package made by them are done in different directories, and
# Threads::Threads variable is not visible here, hence the manual call.
find_package(Threads)

# General setup
project(WSim)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Helper macro for selecting all sources recursively and accumulating them in a variable
macro(get_sources VARIABLE)
    set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    file(GLOB_RECURSE ${VARIABLE}
        "${SOURCE_DIR}/*/CMakeLists.txt"
        "${SOURCE_DIR}/*.cmake"
        "${SOURCE_DIR}/*.h"
        "${SOURCE_DIR}/*.hpp"
        "${SOURCE_DIR}/*.cpp"
        "${SOURCE_DIR}/*.cl"
        "${SOURCE_DIR}/*.c"
        "${SOURCE_DIR}/*.glsl"
    )
    source_group (TREE ${SOURCE_DIR} FILES ${${VARIABLE}})
endmacro()

# Library target names
set_with_parent_scope(TARGET_WSIM_COMMON_LIB WsimCommon)
set_with_parent_scope(TARGET_WSIM_SIMULATION_LIB WSimSimulation)
set_with_parent_scope(TARGET_WSIM_RENDERER_LIB WSimRenderer)

# Add source and tests
add_subdirectory(Source)
add_subdirectory(Tests)
