find_package(MPI)
if (MPI_FOUND)
    set(TARGET_WSIM_MPI WSimMPI)
    if (WSIM_TEXT_ONLY)
        message("MPI was found but needs graphical interface. Skipping ${TARGET_WSIM_MPI}...")
        return()
    endif()

    # Define WSimMPI - an MPI executable conducting the simulation
    get_sources(SOURCE_FILES)
    add_executable(${TARGET_WSIM_MPI} ${SOURCE_FILES})
    add_dependencies(${TARGET_WSIM_MPI} ${TARGET_WSIM_SIMULATION_LIB} ${TARGET_WSIM_RENDERER_LIB})
    target_link_libraries(${TARGET_WSIM_MPI}
        ${MPI_LIBRARIES}
        ${TARGET_WSIM_SIMULATION_LIB}
        ${TARGET_WSIM_RENDERER_LIB}
    )
    target_include_directories(${TARGET_WSIM_MPI} PRIVATE
        $<TARGET_PROPERTY:${TARGET_WSIM_SIMULATION_LIB},INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:${TARGET_WSIM_RENDERER_LIB},INCLUDE_DIRECTORIES>
        ${MPI_INCLUDE_PATH}
        ${DIR_WSIM}
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
