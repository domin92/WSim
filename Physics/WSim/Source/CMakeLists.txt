# Get source files. Exclude directories which define their own targets.
get_sources(SOURCE_FILES)
list(FILTER SOURCE_FILES EXCLUDE REGEX "^${SOURCE_DIR}/ComputeInfo/*")
list(FILTER SOURCE_FILES EXCLUDE REGEX "^${SOURCE_DIR}/WSimStandalone/*")
list(FILTER SOURCE_FILES EXCLUDE REGEX "^${SOURCE_DIR}/WSimMPI/*")

# Define core library. It can be used by other targets.
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

# Define executable targets
add_subdirectory(ComputeInfo)
add_subdirectory(WSimStandalone)
add_subdirectory(WSimMPI)
