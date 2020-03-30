# Find dependencies
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(MPI) # If MPI isn't found, WSimMPI target won't be created
if(UNIX)
    # It is used by gtest but find_package made by gtest project is done in a
    # different directory, and Threads::Threads variable is not visible here,
    # hence the manual call.
    find_package(Threads)
endif()

# General setup
project(WSim)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Helper macro
macro(get_sources VARIABLE)
    set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    file(GLOB_RECURSE ${VARIABLE}
        "${SOURCE_DIR}/*/CMakeLists.txt"
        "${SOURCE_DIR}/*.cmake"
        "${SOURCE_DIR}/*.h"
        "${SOURCE_DIR}/*.cpp"
        "${SOURCE_DIR}/*.cl"
    )
    source_group (TREE ${SOURCE_DIR} FILES ${VARIABLE})
endmacro()

# Add source and tests
add_subdirectory(Source)
add_subdirectory(Tests)
