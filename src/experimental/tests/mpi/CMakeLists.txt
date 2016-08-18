ecbuild_add_test(
    TARGET      eckit_test_mpi
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
    BOOST
)

    
