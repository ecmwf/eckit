list( APPEND eckit_testing_srcs
UnitTest.h
UnitTest.cc
)

ecbuild_add_library(TARGET eckit_testing
					INSTALL_HEADERS ALL
					SOURCES ${eckit_testing_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/testing
					LIBS eckit )

