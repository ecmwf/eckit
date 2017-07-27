foreach( TESTCASE RANGE 1 12 )
  ecbuild_add_test(
    TARGET      eckit_test_option_cmdargs_${TESTCASE}
    SOURCES     eckit_test_option_cmdargs.cc
    LIBS        eckit_option
    DEFINITIONS TESTCASE=${TESTCASE}
  )
endforeach()

ecbuild_add_test(
    TARGET      eckit_test_option_factory
    SOURCES     eckit_test_option_factory.cc
    LIBS        eckit_option
)
