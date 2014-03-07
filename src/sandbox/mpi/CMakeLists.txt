ecbuild_add_test(
	TARGET      mpi_comm
	SOURCES     mpi_comm.cc 
	CONDITION   HAVE_GRIB AND HAVE_MPI
	INCLUDES    ${MPI_CXX_INCLUDE_PATH} 
	LIBS        eckit_mpi eckit_grib eckit ${MPI_CXX_LIBRARIES} 
	)

