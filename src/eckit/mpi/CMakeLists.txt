
list( APPEND eckit_mpi_srcs
mpi.h
mpi.cc
Collectives.h
Comm.h
Comm.cc
Exceptions.h
ParallelContextBehavior.h
ParallelContextBehavior.cc
BroadcastFile.h
BroadcastFile.cc
)

if( NOT HAVE_MPI )
  set( MPI_CXX_INCLUDE_PATH ${MPISTUBS_INCLUDE_DIRS} )
  set( MPI_CXX_LIBRARIES    mpistubs )
  set( MPI_CXX_DEFINITIONS ECKIT_MPISTUBS)
endif()


if( HAVE_ECKIT_MPI )

  ecbuild_add_library(TARGET eckit_mpi
                      INSTALL_HEADERS ALL
                      HEADER_DESTINATION
                        ${INSTALL_INCLUDE_DIR}/eckit/mpi
                      SOURCES
                        ${eckit_mpi_srcs}
                      DEFINITIONS
                          ${MPI_CXX_DEFINITIONS}
                      INCLUDES
                          ${MPI_CXX_INCLUDE_PATH}
                      LIBS
                          eckit ${MPI_CXX_LIBRARIES} )
endif()


if( HAVE_ECKIT_MPISTUBS)

  ecbuild_add_library(TARGET eckit_mpistubs
                      INSTALL_HEADERS ALL
                      HEADER_DESTINATION
                        ${INSTALL_INCLUDE_DIR}/eckit/mpi
                      SOURCES
                        ${eckit_mpi_srcs}
                      INCLUDES
                          ${MPISTUBS_INCLUDE_DIRS}
                      LIBS
                          eckit mpistubs
                      DEFINITIONS ECKIT_USE_MPISTUBS )

  get_property( defs TARGET eckit_mpistubs PROPERTY COMPILE_DEFINITIONS )
    foreach(def ${defs})
      message(STATUS "eckit_mpistubs definition: '${def}'")
    endforeach()

endif()

