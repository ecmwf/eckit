# aio system calls are know to fail on the Cray
# see https://sourceware.org/bugzilla/show_bug.cgi?id=11787

ecbuild_add_test( TARGET      eckit_test_filesystem_multihandle
                  SOURCES     test_multihandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_filesystem_aiohandle
                  CONDITION   NOT DEFINED ENV{CRAYOS_VERSION}
                  SOURCES     test_aiohandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_filesystem_asynchandle
                  SOURCES     test_asynchandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_filesystem_localpathname
                  SOURCES     test_localpathname.cc
                  LIBS        eckit )


ecbuild_add_test( TARGET      eckit_test_filesystem_restarthandle
                  SOURCES     test_restarthandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_filesystem_pathexpander
                  SOURCES     test_pathexpander.cc
                  ENVIRONMENT "CURRENT_TEST_DIR=${CMAKE_CURRENT_BINARY_DIR}"
                  LIBS        eckit )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp/foo)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdir/foo/1)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdir/foo/2)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdir/foo/2/1)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdir/bar)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testdir/baz)
