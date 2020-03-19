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
set(CORE_LIB_NAME WSimCore)
set(TESTS_EXE_NAME WSimTests)
add_executable(${TESTS_EXE_NAME} ${SOURCE_FILES})
target_link_libraries(${TESTS_EXE_NAME} ${CORE_LIB_NAME} ${GTEST_LIBRARY_PATH})
target_link_libraries(${TESTS_EXE_NAME} $<TARGET_PROPERTY:${CORE_LIB_NAME},LINK_LIBRARIES>)
target_include_directories(${TESTS_EXE_NAME} PRIVATE $<TARGET_PROPERTY:${CORE_LIB_NAME},INCLUDE_DIRECTORIES>)
target_include_directories(${TESTS_EXE_NAME} PRIVATE ${SOURCE_DIR} ${GTEST_INCLUDE_PATH})
set_target_properties(${TESTS_EXE_NAME} PROPERTIES CXX_STANDARD 17)
