ecbuild_add_test( TARGET   test_value_params
                  ARGS --log_level=message
                  BOOST
                  SOURCES  test_value_params.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   test_value_params_streaming
                  ARGS --log_level=message
                  BOOST
                  SOURCES  test_value_params_streaming.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   test_value_properties
                  ARGS --log_level=message
                  BOOST
                  SOURCES  test_value_properties.cc
                  LIBS     eckit )
