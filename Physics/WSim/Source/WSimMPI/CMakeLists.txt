if (MPI_FOUND)
    # Define WSimMPI - an MPI executable conducting the simulation
    get_sources(SOURCE_FILES)
    add_executable(${TARGET_WSIM_MPI} ${SOURCE_FILES})
    add_dependencies(${TARGET_WSIM_MPI} ${TARGET_WSIM_CORE_LIB})
    target_link_libraries(${TARGET_WSIM_MPI}
        ${MPI_LIBRARIES}
        ${TARGET_WSIM_CORE_LIB}
        $<GENEX_EVAL:$<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},LINK_LIBRARIES>>
        $<TARGET_PROPERTY:freeglut_static,LINK_LIBRARIES>
        $<TARGET_PROPERTY:glfw,LINK_LIBRARIES>
        $<TARGET_PROPERTY:glad,LINK_LIBRARIES>
        $<TARGET_FILE:glfw>
        $<TARGET_FILE:glad>
    )
    target_include_directories(${TARGET_WSIM_MPI} PRIVATE
        $<TARGET_PROPERTY:${TARGET_WSIM_CORE_LIB},INCLUDE_DIRECTORIES>
        ${MPI_INCLUDE_PATH}
        .
        ${DIR_GLFW}/include
        ${DIR_GLAD}/include
        ${DIR_STB}
    )
    set_target_properties(${TARGET_WSIM_MPI} PROPERTIES FOLDER Executables)
    set_target_properties(${TARGET_WSIM_MPI} PROPERTIES CXX_STANDARD 17)

    # MPI settings
    if(MPI_COMPILE_FLAGS)
        set_target_properties(${TARGET_WSIM_MPI} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()
    if(MPI_LINK_FLAGS)
        set_target_properties(${TARGET_WSIM_MPI} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()

    # Generate run_mpi.sh script
    file(GENERATE OUTPUT ${DIR_ROOT}/run_mpi.sh CONTENT "mpirun -np 9 $<TARGET_FILE:${TARGET_WSIM_MPI}> $@")
endif()
