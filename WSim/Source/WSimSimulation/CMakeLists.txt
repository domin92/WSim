# Define WSimSimulation - a library providing API to water simulation
get_sources(SOURCE_FILES)
add_library(${TARGET_WSIM_SIMULATION_LIB} STATIC ${SOURCE_FILES})
target_link_libraries(${TARGET_WSIM_SIMULATION_LIB} PUBLIC ${OpenCL_LIBRARIES} ${TARGET_WSIM_COMMON_LIB})
target_include_directories(${TARGET_WSIM_SIMULATION_LIB} PUBLIC ${DIR_WSIM} ${DIR_OPENCL_HEADERS})
set_target_properties(${TARGET_WSIM_SIMULATION_LIB} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_WSIM_SIMULATION_LIB} PROPERTIES FOLDER Libraries)
target_compile_definitions(${TARGET_WSIM_SIMULATION_LIB} PRIVATE SHADERS_DIR="${SOURCE_DIR}/Kernels")
