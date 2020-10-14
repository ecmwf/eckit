list( APPEND eckit_mpi_srcs
Buffer.h
Comm.cc
Comm.h
DataType.cc
DataType.h
Operation.cc
Operation.h
Request.cc
Request.h
Serial.cc
Serial.h
SerialData.h
SerialStatus.cc
SerialStatus.h
SerialRequest.cc
SerialRequest.h
Status.cc
Status.h
)

if( HAVE_MPI )

    list( APPEND eckit_mpi_srcs
        Parallel.cc
        Parallel.h
        ParallelStatus.cc
        ParallelStatus.h
        ParallelRequest.cc
        ParallelRequest.h
        )

    set(eckit_mpi_defs ${MPI_C_DEFINITIONS} )
    set(eckit_mpi_incs ${MPI_C_INCLUDE_PATH} )
    set(eckit_mpi_libs ${MPI_C_LIBRARIES} )
endif()

ecbuild_add_library( TARGET eckit_mpi TYPE SHARED
                     INSTALL_HEADERS ALL
                     HEADER_DESTINATION
                         ${INSTALL_INCLUDE_DIR}/eckit/mpi
                     SOURCES
                         ${eckit_mpi_srcs}
                     DEFINITIONS
                         "${eckit_mpi_defs}"
                     PUBLIC_INCLUDES
                         "${eckit_mpi_incs}"
                     PUBLIC_LIBS
                         "${eckit_mpi_libs}"
                     PUBLIC_LIBS
                         eckit )
