ecbuild_add_test(   TARGET      test_config
                    SOURCES     test_config.cc
                    LIBS        eckit )

ecbuild_add_test(   TARGET      test_resource
                    SOURCES     test_resource.cc
                    ARGS        -integer 100
                    LIBS        eckit )
