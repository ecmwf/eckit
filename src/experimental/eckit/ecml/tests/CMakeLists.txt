list( APPEND ecml_tests test_core.ecml )

# Get location of test runner executable
get_target_property ( ecml_test_runner_bin ecml_test LOCATION )

set( test_environment
  ECML_HOME=${PROJECT_SOURCE_DIR}
  #ECML_TEST_DATA_PATH=${CMAKE_CURRENT_BINARY_DIR}
  PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH})

### Add ECML tests
foreach( _test ${ecml_tests} )
    ecbuild_add_test( TARGET       ${_test}
                      COMMAND      ${ecml_test_runner_bin}
                      ARGS         ${PROJECT_SOURCE_DIR}/src/experimental/eckit/ecml/tests/${_test}
                      ENVIRONMENT  ${test_environment}
                      TEST_DEPENDS ${_dependencies})
endforeach()


