if( MPI_CXX_FOUND )

	message( STATUS "MPI")

	message( STATUS "	MPI_CXX_COMPILER      : [${MPI_CXX_COMPILER}]")
	message( STATUS "	MPI_CXX_INCLUDE_PATH  : [${MPI_CXX_INCLUDE_PATH}]")
	message( STATUS "	MPI_CXX_LIBRARIES     : [${MPI_CXX_LIBRARIES}]")

	message( STATUS "	MPIEXEC               : [${MPIEXEC}]")

endif()

if(CURSES_FOUND)
  message( STATUS " Curses   includes : [${CURSES_INCLUDE_DIR}]" )
  message( STATUS "          libs     : [${CURSES_LIBRARIES}]" )
endif()

if(READLINE_FOUND)
  message( STATUS " ReadLine version  : [${READLINE_INCLUDE_DIRS}]" )
  message( STATUS "          includes : [${READLINE_INCLUDE_DIRS}]" )
  message( STATUS "          libs     : [${READLINE_LIBRARIES}]" )
endif()
