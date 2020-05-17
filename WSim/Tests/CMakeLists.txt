set(MAIN_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

macro(append_main_file_to_sources VARIABLE)
    list(APPEND ${VARIABLE} ${MAIN_FILE})
    source_group (TREE ${MAIN_FILE_DIR} FILES ${MAIN_FILE})
endmacro()

# Add subdirectories. Each define a separate compilation  target
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(subdir ${subdirectories})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
        add_subdirectory(${subdir})
    endif()
endforeach()
