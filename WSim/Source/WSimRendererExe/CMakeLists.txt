# Define WSimStandalone - a water simulation done locally, using only one computer (no MPI involved).
get_sources(SOURCE_FILES)
add_executable(${TARGET_WSIM_RENDERER_EXE} ${SOURCE_FILES})
add_dependencies(${TARGET_WSIM_RENDERER_EXE} ${TARGET_WSIM_RENDERER_LIB})
target_link_libraries(${TARGET_WSIM_RENDERER_EXE}
    $<GENEX_EVAL:$<TARGET_PROPERTY:${TARGET_WSIM_RENDERER_LIB},LINK_LIBRARIES>>
    ${TARGET_WSIM_RENDERER_LIB}
)
target_include_directories(${TARGET_WSIM_RENDERER_EXE} PRIVATE $<TARGET_PROPERTY:${TARGET_WSIM_RENDERER_LIB},INCLUDE_DIRECTORIES>)
set_target_properties(${TARGET_WSIM_RENDERER_EXE} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_WSIM_RENDERER_EXE} PROPERTIES FOLDER Executables)

# Generate run_standalone.sh script
file(GENERATE OUTPUT ${DIR_ROOT}/run_renderer_exe.sh CONTENT "$<TARGET_FILE:${TARGET_WSIM_RENDERER_EXE}> $@")
