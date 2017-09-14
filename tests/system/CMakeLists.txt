# file accessed in the test

file( WRITE "${CMAKE_BINARY_DIR}/etc/eckit/test/test.cfg" "Just for testing" )

ecbuild_add_test(   TARGET      eckit_test_system_library
                    SOURCES     test_system_library.cc
					LIBS        eckit )
