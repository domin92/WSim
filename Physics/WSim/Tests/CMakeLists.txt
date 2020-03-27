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

# Define tests executable
add_executable(${TARGET_WSIM_TESTS} ${SOURCE_FILES})
target_link_libraries(${TARGET_WSIM_TESTS} 
    $<TARGET_FILE:gtest>
    $<TARGET_FILE:${TARGET_WSIM_CORE_LIB}>
    $<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},LINK_LIBRARIES>
    $<TARGET_PROPERTY:gtest,LINK_LIBRARIES>
)
target_include_directories(${TARGET_WSIM_TESTS} PRIVATE $<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},INCLUDE_DIRECTORIES> )
target_include_directories(${TARGET_WSIM_TESTS} PRIVATE ${DIR_GTEST}/googletest/include)
set_target_properties(${TARGET_WSIM_TESTS} PROPERTIES CXX_STANDARD 17)
