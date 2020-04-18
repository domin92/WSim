# Define WSimCore library. It can be used by other targets.
get_sources(SOURCE_FILES)
add_library(${TARGET_WSIM_CORE_LIB} STATIC ${SOURCE_FILES})
target_link_libraries(${TARGET_WSIM_CORE_LIB}
    ${OpenCL_LIBRARIES}
    OpenGL::GL
    $<TARGET_FILE:freeglut_static>
)
target_include_directories(${TARGET_WSIM_CORE_LIB} PRIVATE ${DIR_WSIM} ${DIR_FREEGLUT}/include ${DIR_OPENCL_HEADERS})
set_target_properties(${TARGET_WSIM_CORE_LIB} PROPERTIES CXX_STANDARD 17)
target_compile_definitions(${TARGET_WSIM_CORE_LIB}
    PRIVATE SHADERS_DIR="${SOURCE_DIR}/Kernels"
    FREEGLUT_STATIC
)
