# Define WSimCommon - a library with utility functions and types
get_sources(SOURCE_FILES)
add_library(${TARGET_WSIM_COMMON_LIB} STATIC ${SOURCE_FILES})
target_include_directories(${TARGET_WSIM_COMMON_LIB} PUBLIC ${DIR_WSIM})
set_target_properties(${TARGET_WSIM_COMMON_LIB} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_WSIM_COMMON_LIB} PROPERTIES FOLDER Libraries)
