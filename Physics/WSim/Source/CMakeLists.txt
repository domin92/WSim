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

# Some directories define their own utility targets - exclude them from WSim)
list(FILTER SOURCE_FILES EXCLUDE REGEX "^${SOURCE_DIR}/ComputeInfo/*")

# Define core library
add_library(${TARGET_WSIM_CORE_LIB} STATIC ${SOURCE_FILES})
target_link_libraries(${TARGET_WSIM_CORE_LIB}
    OpenCL::OpenCL
    OpenGL::GL
    $<TARGET_FILE:freeglut_static>
)
target_include_directories(${TARGET_WSIM_CORE_LIB} PRIVATE ${SOURCE_DIR}/.. ${DIR_FREEGLUT}/include)
set_target_properties(${TARGET_WSIM_CORE_LIB} PROPERTIES CXX_STANDARD 17)
target_compile_definitions(${TARGET_WSIM_CORE_LIB}
    PRIVATE SHADERS_DIR="${SOURCE_DIR}/Kernels"
    FREEGLUT_STATIC
)

# Define executable
add_executable(${TARGET_WSIM} ${EXE_ONLY_FILES})
target_link_libraries(${TARGET_WSIM} ${TARGET_WSIM_CORE_LIB})
target_link_libraries(${TARGET_WSIM} $<GENEX_EVAL:$<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},LINK_LIBRARIES>>)
target_include_directories(${TARGET_WSIM} PRIVATE $<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},INCLUDE_DIRECTORIES>)
set_target_properties(${TARGET_WSIM} PROPERTIES CXX_STANDARD 17)

# Define utility targets
add_subdirectory(ComputeInfo)

# Generate run.sh script
file(GENERATE OUTPUT ${DIR_ROOT}/run.sh CONTENT $<TARGET_FILE:${TARGET_WSIM}>)
