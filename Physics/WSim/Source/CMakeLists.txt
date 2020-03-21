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
add_library(${TARGET_WSIM_CORE_LIB} STATIC ${SOURCE_FILES})
target_link_libraries(${TARGET_WSIM_CORE_LIB} PRIVATE 
    OpenCL::OpenCL 
    OpenGL::GL
    freeglut_static
)
target_include_directories(${TARGET_WSIM_CORE_LIB} PRIVATE ${SOURCE_DIR})
set_target_properties(${TARGET_WSIM_CORE_LIB} PROPERTIES CXX_STANDARD 17)
target_compile_definitions(${TARGET_WSIM_CORE_LIB} PRIVATE SHADERS_DIR="${SOURCE_DIR}/Kernels")

# Define executable
add_executable(${TARGET_WSIM} ${EXE_ONLY_FILES})
target_link_libraries(${TARGET_WSIM} ${TARGET_WSIM_CORE_LIB})
target_link_libraries(${TARGET_WSIM} $<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},LINK_LIBRARIES>)
target_include_directories(${TARGET_WSIM} PRIVATE $<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},INCLUDE_DIRECTORIES>)
set_target_properties(${TARGET_WSIM} PROPERTIES CXX_STANDARD 17)
