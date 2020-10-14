if( MPI_C_FOUND )

	message( STATUS "MPI")

	message( STATUS "	MPI_C_INCLUDE_PATH  : [${MPI_C_INCLUDE_PATH}]")
	message( STATUS "	MPI_C_LIBRARIES     : [${MPI_C_LIBRARIES}]")
	message( STATUS "	MPIEXEC             : [${MPIEXEC}]")

endif()

if(CURSES_FOUND)
  message( STATUS " Curses   includes : [${CURSES_INCLUDE_DIR}]" )
  message( STATUS "          libs     : [${CURSES_LIBRARIES}]" )
endif()
