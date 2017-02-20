# file accessed in the test

file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/etc/eckit/test/test.cfg" CONTENT "Just for testing")

ecbuild_add_test(   TARGET      eckit_test_system
					BOOST
                    SOURCES     test_system.cc
					LIBS        eckit )
