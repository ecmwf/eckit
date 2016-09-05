list( APPEND eckit_mpi_srcs
Buffer.h
Collectives.h
Comm.cc
Comm.h
DataType.cc
DataType.h
Operation.h
Operation.cc
Request.cc
Request.h
Serial.cc
Serial.h
Status.cc
Status.h
)

if( HAVE_MPI )
    list( APPEND eckit_mpi_srcs  Parallel.h Parallel.cc )
    set(eckit_mpi_defs ${MPI_CXX_DEFINITIONS} )
    set(eckit_mpi_incs ${MPI_CXX_INCLUDE_PATH} )
    set(eckit_mpi_libs ${MPI_CXX_LIBRARIES} )
endif()

ecbuild_add_library(TARGET eckit_mpi
                    INSTALL_HEADERS ALL
                    HEADER_DESTINATION
                        ${INSTALL_INCLUDE_DIR}/eckit/mpi
                    SOURCES
                        ${eckit_mpi_srcs}
                    DEFINITIONS
                        "${eckit_mpi_defs}"
                    INCLUDES
                        "${eckit_mpi_incs}"
                    LIBS
                        eckit ${eckit_mpi_libs} )
