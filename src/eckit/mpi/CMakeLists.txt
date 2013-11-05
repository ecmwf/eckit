list( APPEND eckit_mpi_srcs
	Comm.h
	Comm.cc
)

ecbuild_add_library( TARGET eckit_mpi
					 INSTALL_HEADERS ALL
					 HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/mpi
					 CONDITION
						ECKIT_MPI
					 SOURCES
						${eckit_mpi_srcs}
					 INCLUDES
						${MPI_CXX_INCLUDE_PATH}
					 LIBS
						eckit ${MPI_CXX_LIBRARIES} )

