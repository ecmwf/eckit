ecbuild_add_library( TARGET     eckit_test_custom_params
                     NOINSTALL
                     SOURCES    AnyKeyParams.cc
                     LIBS       eckit )

ecbuild_add_test( TARGET   eckit_test_value_params
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_value_params.cc
                  LIBS     eckit eckit_test_custom_params )

ecbuild_add_test( TARGET   eckit_test_value_properties
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_value_properties.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_value
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_value.cc
                  LIBS     eckit )
