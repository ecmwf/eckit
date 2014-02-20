ecbuild_add_test(
		TARGET      test_mpi_comm
		SOURCES     test_mpi_comm.cc
		CONDITION   HAVE_MPI
		INCLUDES ${MPI_CXX_INCLUDE_PATH}
		LIBS eckit_mpi eckit ${MPI_CXX_LIBRARIES}
		COMMAND ${MPIEXEC} ARGS -np 4 test_mpi_comm
		)
