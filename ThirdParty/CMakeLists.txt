# Paths for header-only dependencies
set_with_parent_scope(DIR_OPENCL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/OpenCL-Headers)
set_with_parent_scope(DIR_STB ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)

# Paths for built dependencies
set_with_parent_scope(DIR_GTEST ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
set_with_parent_scope(DIR_GLAD ${CMAKE_CURRENT_SOURCE_DIR}/glad)
set_with_parent_scope(DIR_GLFW ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set_with_parent_scope(DIR_GLM ${CMAKE_CURRENT_SOURCE_DIR}/glm)

# Add dependencies to the project
option(gtest_force_shared_crt on)
add_subdirectory(${DIR_GTEST})
add_subdirectory(${DIR_GLAD})
add_subdirectory(${DIR_GLFW})
set(BUILD_STATIC_LIBS on)
add_subdirectory(${DIR_GLM})