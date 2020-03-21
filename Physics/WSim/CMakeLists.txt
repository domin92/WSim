# Find dependencies
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT)
if (GLUT_FOUND)
    set(GLUT_LIB GLUT::GLUT)
else()
    find_package(FREEGLUT REQUIRED)
    set(GLUT_LIB FreeGLUT::freeglut)
endif()

# General setup
project(WSim)
add_subdirectory(Source)
add_subdirectory(Tests)
