list( APPEND eckit_mpi_lib_srcs
Comm.h
Comm.cc
Environment.h
Environment.cc
)

ecbuild_add_library( TARGET              eckit_mpi
                     INSTALL_HEADERS     ALL
                     HEADER_DESTINATION  ${INSTALL_INCLUDE_DIR}/eckit/mpi
                     SOURCES             ${eckit_mpi_lib_srcs}
                     LIBS                eckit )
