ecbuild_add_test(
    TARGET      eckit_test_mpi
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
    BOOST
)

ecbuild_add_test(
    TARGET      eckit_test_mpistubs
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_MPI
    INCLUDES    ${MPISTUBS_INCLUDE_DIRS}
    LIBS eckit_mpistubs
    DEFINITIONS ECKIT_USE_MPISTUBS
    BOOST
)


ecbuild_add_test(
    TARGET      eckit_test_mpi_parallelcontext
    SOURCES     eckit_test_mpi_parallelcontext.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
    BOOST
)

ecbuild_add_test(
    TARGET      eckit_test_mpistubs_parallelcontext
    SOURCES     eckit_test_mpi_parallelcontext.cc
    CONDITION   HAVE_MPI
    INCLUDES    ${MPISTUBS_INCLUDE_DIRS}
    LIBS eckit_mpistubs
    DEFINITIONS ECKIT_USE_MPISTUBS
    BOOST
)
