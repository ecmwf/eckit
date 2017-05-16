ecbuild_add_test( TARGET   eckit_test_config
                  BOOST
                  SOURCES  test_config.cc
                  ENABLED  OFF
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_resource
                  BOOST
                  SOURCES  test_resource.cc
                  ARGS     -integer 100 -listlong 88,99,11,22
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_configuration
                  BOOST
                  SOURCES  test_configuration.cc
                  LIBS     eckit )
