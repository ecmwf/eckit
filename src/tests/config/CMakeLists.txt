ecbuild_add_test(   TARGET      test_config
					BOOST
					SOURCES     test_config.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      test_resource
					BOOST
					SOURCES     test_resource.cc
					ARGS        "-integer 100 -listlong 88,99,11,22"
                    LIBS        eckit )
