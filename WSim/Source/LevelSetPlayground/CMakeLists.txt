# Define WSimRendererExe - a temporary target for testing WSimRenderer
set(TARGET_LEVEL_SET_PLAYGROUND LevelSetPlayground)
get_sources(SOURCE_FILES)
add_executable(${TARGET_LEVEL_SET_PLAYGROUND} ${SOURCE_FILES})
target_link_libraries(${TARGET_LEVEL_SET_PLAYGROUND} PRIVATE ${TARGET_WSIM_RENDERER_LIB} ${TARGET_WSIM_SIMULATION_LIB})
set_target_properties(${TARGET_LEVEL_SET_PLAYGROUND} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_LEVEL_SET_PLAYGROUND} PROPERTIES FOLDER Executables)
