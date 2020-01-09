#eckit_test_mpi is hanging for Intel 19.0.5. Disabling until a fix is proposed from upstream.
if( NOT (CMAKE_CXX_COMPILER_ID STREQUAL Intel AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19
                                              AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 20) )
    ecbuild_add_test(
        TARGET      eckit_test_mpi
        SOURCES     eckit_test_mpi.cc
        CONDITION   HAVE_MPI
        LIBS eckit_mpi
        MPI 4
    )
endif()

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
