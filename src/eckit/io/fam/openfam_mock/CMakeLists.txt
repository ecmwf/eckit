# OpenFAM Mock — standalone shared library
#
# Provides the same fam/fam.h API as the real OpenFAM library.
# creates a LibOPENFAM alias target so eckit and downstream projects link uniformly.

ecbuild_add_library(
    TARGET eckit_openfam_mock
    TYPE SHARED
    INSTALL_HEADERS LISTED
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}

    SOURCES
        fam/fam.cc
        fam/fam.h
        fam/fam_exception.h
        FamMockSession.cc

    PRIVATE_INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

    PRIVATE_LIBS
        ${RT_LIBRARIES}
        ${THREADS_LIBRARIES}
)

# Same target name as FindLibOPENFAM would create for the real library.
add_library( LibOPENFAM ALIAS eckit_openfam_mock )

# Expose the mock's include directory (so eckit sources find fam/fam.h)
set( LIB_OPENFAM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
