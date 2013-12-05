ecbuild_add_executable(   
	TARGET      mpi_comm
	SOURCES     mpi_comm.cc 
	CONDITION   ENABLE_GRIB AND ENABLE_MPI
	INCLUDES    ${MPI_CXX_INCLUDE_PATH} 
	LIBS        eckit_mpi eckit_grib eckit ${MPI_CXX_LIBRARIES} 
	)

