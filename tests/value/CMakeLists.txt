ecbuild_add_library( TARGET     eckit_test_value_custom_params
                     NOINSTALL
                     SOURCES    AnyKeyParams.cc
                     LIBS       eckit )

ecbuild_add_test( TARGET   eckit_test_value_value_params
                  SOURCES  test_value_params.cc
                  LIBS     eckit eckit_test_value_custom_params )

ecbuild_add_test( TARGET   eckit_test_value_value_properties
                  SOURCES  test_value_properties.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_value_value
                  SOURCES  test_value.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_value_value_const
                  SOURCES  test_value_const.cc
                  LIBS     eckit )
