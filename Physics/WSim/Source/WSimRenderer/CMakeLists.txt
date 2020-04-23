# Define WSimRenderer - an OpenGL executable
get_sources(SOURCE_FILES)
add_executable(${TARGET_WSIM_RENDERER} ${SOURCE_FILES})
target_include_directories(${TARGET_WSIM_RENDERER} PRIVATE . ${DIR_GLFW}/include)
target_link_libraries(${TARGET_WSIM_RENDERER}
    $<TARGET_PROPERTY:glfw,LINK_LIBRARIES>
    $<TARGET_FILE:glfw>
)
set_target_properties(${TARGET_WSIM_RENDERER} PROPERTIES CXX_STANDARD 17)

# Generate run_renderer.sh script
file(GENERATE OUTPUT ${DIR_ROOT}/run_renderer.sh CONTENT "$<TARGET_FILE:${TARGET_WSIM_RENDERER}> $@")
