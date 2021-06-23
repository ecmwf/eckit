ecbuild_add_test(
    TARGET      eckit_test_mpi_parallel
    SOURCES     eckit_test_mpi.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)

ecbuild_add_test(
    TARGET      eckit_test_mpi_serial
    SOURCES     eckit_test_mpi.cc
    LIBS eckit_mpi
    ENVIRONMENT ECKIT_MPI_FORCE=serial
)

ecbuild_add_test(
    TARGET      eckit_test_mpi_addcomm
    SOURCES     eckit_test_mpi_addcomm.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)

ecbuild_add_test(
    TARGET      eckit_test_mpi_splitcomm
    SOURCES     eckit_test_mpi_splitcomm.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)

ecbuild_add_test(
    TARGET      eckit_test_mpi_objserialisation
    SOURCES     eckit_test_mpi_objserialisation.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)

ecbuild_add_test(
    TARGET      eckit_test_mpi_internal_access
    SOURCES     eckit_test_mpi_internal_access.cc
    CONDITION   HAVE_MPI
    LIBS eckit_mpi
    MPI 4
)
