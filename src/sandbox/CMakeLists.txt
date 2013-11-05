ecbuild_add_executable(   
	TARGET      mpi_comm
	SOURCES     mpi_comm.cc
	CONDITION   ECKIT_MPI
	INCLUDES    ${MPI_CXX_INCLUDE_PATH}
	LIBS        eckit_mpi eckit ${MPI_CXX_LIBRARIES}
	)

