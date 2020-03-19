# Get source files
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*/CMakeLists.txt"
    "${SOURCE_DIR}/*.cmake"
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.cl"
)
source_group (TREE ${SOURCE_DIR} FILES ${SOURCE_FILES})

# Files which will be only in executable file, not in core library
set(EXE_ONLY_FILES "${SOURCE_DIR}/Main.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_DIR}/Main.cpp)

# Define core library
set(CORE_LIB_NAME WSimCore)
add_library(${CORE_LIB_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${CORE_LIB_NAME} PRIVATE OpenCL::OpenCL OpenGL::GL ${GLUT_LIB})
target_include_directories(${CORE_LIB_NAME} PRIVATE ${SOURCE_DIR})
set_target_properties(${CORE_LIB_NAME} PROPERTIES CXX_STANDARD 17)
target_compile_definitions(${CORE_LIB_NAME} PRIVATE SHADERS_DIR="${SOURCE_DIR}/Kernels")

# Define executable
set(EXE_NAME WSim)
add_executable(${EXE_NAME} ${EXE_ONLY_FILES})
target_link_libraries(${EXE_NAME} ${CORE_LIB_NAME})
target_link_libraries(${EXE_NAME} $<TARGET_PROPERTY:${CORE_LIB_NAME},LINK_LIBRARIES>)
target_include_directories(${EXE_NAME} PRIVATE $<TARGET_PROPERTY:${CORE_LIB_NAME},INCLUDE_DIRECTORIES>)
set_target_properties(${EXE_NAME} PROPERTIES CXX_STANDARD 17)
