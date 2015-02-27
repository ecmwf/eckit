ecbuild_add_test(
		TARGET      test_mpi_comm
		SOURCES     test_mpi_comm.cc
		CONDITION   OFF
		INCLUDES ${MPI_CXX_INCLUDE_PATH}
		LIBS eckit_mpi eckit ${MPI_CXX_LIBRARIES}
		COMMAND ${MPIEXEC} ARGS -np 4 test_mpi_comm
		)

if( ECKIT_HAVE_ECKIT_MPI )
ecbuild_add_test(
    TARGET      eckit_test_mpi
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_ECKIT_MPI
    LIBS eckit_mpi
    MPI 4
    BOOST
)
endif()

if( ECKIT_HAVE_ECKIT_MPISTUBS )
ecbuild_add_test(
    TARGET      eckit_test_mpistubs
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_ECKIT_MPISTUBS
    LIBS eckit_mpistubs
    DEFINITIONS ECKIT_USE_MPISTUBS
    BOOST
)
endif()


if( ECKIT_HAVE_ECKIT_MPI )
ecbuild_add_test(
    TARGET      eckit_test_mpi_parallelcontext
    SOURCES     eckit_test_mpi_parallelcontext.cc
    CONDITION   HAVE_ECKIT_MPI
    LIBS eckit_mpi
    MPI 4
    BOOST
)
endif()

if( ECKIT_HAVE_ECKIT_MPISTUBS )
ecbuild_add_test(
    TARGET      eckit_test_mpistubs_parallelcontext
    SOURCES     eckit_test_mpi_parallelcontext.cc
    CONDITION   HAVE_ECKIT_MPISTUBS
    LIBS eckit_mpistubs
    DEFINITIONS ECKIT_USE_MPISTUBS
    BOOST
)
endif()
