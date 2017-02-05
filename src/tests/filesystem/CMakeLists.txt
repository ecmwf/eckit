# aio system calls are know to fail on the Cray
# see https://sourceware.org/bugzilla/show_bug.cgi?id=11787

ecbuild_add_test( TARGET      eckit_test_multihandle
                  SOURCES     test_multihandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_aiohandle
                  CONDITION   NOT DEFINED ENV{CRAYOS_VERSION}
                  SOURCES     test_aiohandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_localpathname
                  BOOST
                  SOURCES     test_localpathname.cc
                  LIBS        eckit )


