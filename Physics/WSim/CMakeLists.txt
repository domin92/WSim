# Find dependencies
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT)
if(UNIX)
    # It is used by gtest but find_package made by gtest project is done in a
    # different directory, and Threads::Threads variable is not visible here,
    # hence the manual call.
    find_package(Threads)
endif()

# General setup
project(WSim)
add_subdirectory(Source)
add_subdirectory(Tests)
