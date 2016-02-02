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
    INCLUDES    ${MPISTUBS_INCLUDE_DIRS}
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
    INCLUDES    ${MPISTUBS_INCLUDE_DIRS}
    LIBS eckit_mpistubs
    DEFINITIONS ECKIT_USE_MPISTUBS
    BOOST
)
endif()
