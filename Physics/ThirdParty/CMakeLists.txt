# Paths
set_with_parent_scope(DIR_GTEST ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
set_with_parent_scope(DIR_FREEGLUT ${CMAKE_CURRENT_SOURCE_DIR}/FreeGLUT/freeglut/freeglut )
set_with_parent_scope(DIR_GLAD ${CMAKE_CURRENT_SOURCE_DIR}/glad)
set_with_parent_scope(DIR_GLFW ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set_with_parent_scope(DIR_OPENCL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/OpenCL-Headers)

# Add dependencies to the project
set(gtest_force_shared_crt on)
add_subdirectory(${DIR_GTEST})
add_subdirectory(${DIR_FREEGLUT})
add_subdirectory(${DIR_GLAD})
add_subdirectory(${DIR_GLFW})
