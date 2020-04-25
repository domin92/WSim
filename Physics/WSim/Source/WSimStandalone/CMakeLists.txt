# Define WSimStandalone - a water simulation done locally, using only one computer (no MPI involved).
get_sources(SOURCE_FILES)
add_executable(${TARGET_WSIM_STANDALONE} ${SOURCE_FILES})
add_dependencies(${TARGET_WSIM_STANDALONE} ${TARGET_WSIM_SIMULATION_LIB})
target_link_libraries(${TARGET_WSIM_STANDALONE} ${TARGET_WSIM_SIMULATION_LIB})
target_link_libraries(${TARGET_WSIM_STANDALONE}
    $<TARGET_PROPERTY:${TARGET_WSIM_SIMULATION_LIB},LINK_LIBRARIES>
    $<TARGET_PROPERTY:freeglut_static,LINK_LIBRARIES>
    $<TARGET_FILE:freeglut_static>
    OpenGL::GL
)
target_include_directories(${TARGET_WSIM_STANDALONE} PRIVATE
    $<TARGET_PROPERTY:${TARGET_WSIM_SIMULATION_LIB},INCLUDE_DIRECTORIES>
    ${DIR_FREEGLUT}/include
)
set_target_properties(${TARGET_WSIM_STANDALONE} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_WSIM_STANDALONE} PROPERTIES FOLDER Executables)
target_compile_definitions(${TARGET_WSIM_STANDALONE} PRIVATE FREEGLUT_STATIC)

# Generate run_standalone.sh script
file(GENERATE OUTPUT ${DIR_ROOT}/run_standalone.sh CONTENT "$<TARGET_FILE:${TARGET_WSIM_STANDALONE}> $@")
