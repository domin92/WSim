# Define WSimStandalone - a water simulation done locally, using only one computer (no MPI involved).
set(TARGET_WSIM_STANDALONE WSimStandalone)
get_sources(SOURCE_FILES)
add_executable(${TARGET_WSIM_STANDALONE} ${SOURCE_FILES})
add_dependencies(${TARGET_WSIM_STANDALONE} ${TARGET_WSIM_SIMULATION_LIB} ${TARGET_WSIM_RENDERER_LIB})
target_link_libraries(${TARGET_WSIM_STANDALONE} PRIVATE
    ${TARGET_WSIM_SIMULATION_LIB}
    ${TARGET_WSIM_RENDERER_LIB}
)
target_include_directories(${TARGET_WSIM_STANDALONE} PRIVATE
    ${DIR_WSIM}
    $<TARGET_PROPERTY:${TARGET_WSIM_SIMULATION_LIB},INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:${TARGET_WSIM_RENDERER_LIB},INCLUDE_DIRECTORIES>
)
set_target_properties(${TARGET_WSIM_STANDALONE} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_WSIM_STANDALONE} PROPERTIES FOLDER Executables)

# Generate run_standalone.sh script
file(GENERATE OUTPUT ${DIR_ROOT}/run_standalone.sh CONTENT "$<TARGET_FILE:${TARGET_WSIM_STANDALONE}> $@")
