ecbuild_add_test(   TARGET      test_filesystem
                    SOURCES     test_aiohandle.cc
                    LIBS        eckit )

ecbuild_add_test( TARGET      test_localpathname
                  BOOST
				  SOURCES     test_localpathname.cc
				  LIBS        eckit
                )