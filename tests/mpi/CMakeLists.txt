ecbuild_add_test(
    TARGET      eckit_test_mpi
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)

 ecbuild_add_test(
    TARGET      eckit_test_mpi_addcomm
    SOURCES     eckit_test_mpi_addcomm.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)

    
