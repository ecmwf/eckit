list( APPEND ecml_tests
test_print.ecml
test_let_and_value.ecml
test_function.ecml
test_one_letter_variables.ecml
test_first.ecml
test_rest.ecml
test_recursion.ecml
test_higher_order_functions.ecml
)

# Get location of test runner executable (for the ECML tests)
get_target_property ( ecml_test_runner_bin ecml_test LOCATION )

# Get location of unit test runner executable (C++ tests)
get_target_property ( ecml_unittests_bin ecml_unittests LOCATION )

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
                      TEST_DEPENDS ${_dependencies} )
endforeach()

# Add C++ unit tests
ecbuild_add_test( TARGET       ecml_unittests
                  COMMAND      ${ecml_unittests_bin}
                  ENVIRONMENT  ${test_environment}
                  TEST_DEPENDS ${_dependencies} )
