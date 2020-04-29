# Define WSimRenderer - an OpenGL library providing rendering algorithms
get_sources(SOURCE_FILES)
add_library(${TARGET_WSIM_RENDERER_LIB} STATIC ${SOURCE_FILES})
target_include_directories(${TARGET_WSIM_RENDERER_LIB} PUBLIC
    ${DIR_WSIM}
    ${DIR_GLFW}/include
    ${DIR_GLAD}/include
    ${DIR_STB}
    ${DIR_GLM}
)
target_link_libraries(${TARGET_WSIM_RENDERER_LIB} PUBLIC
    $<TARGET_PROPERTY:glfw,LINK_LIBRARIES>
    $<TARGET_PROPERTY:glad,LINK_LIBRARIES>
    $<TARGET_PROPERTY:glm_static,LINK_LIBRARIES>
    $<TARGET_FILE:glfw>
    $<TARGET_FILE:glad>
    $<TARGET_FILE:glm_static>
)
set_target_properties(${TARGET_WSIM_RENDERER_LIB} PROPERTIES FOLDER Libraries)
set_target_properties(${TARGET_WSIM_RENDERER_LIB} PROPERTIES CXX_STANDARD 17)

