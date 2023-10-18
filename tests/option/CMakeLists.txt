foreach( TESTCASE RANGE 1 12 )
    ecbuild_add_test(
        TARGET      eckit_test_option_cmdargs_${TESTCASE}
        SOURCES     eckit_test_option_cmdargs.cc
        LIBS        eckit_option
        DEFINITIONS TESTCASE=${TESTCASE}
    )
endforeach()

foreach( OPTION factory vector )
    ecbuild_add_test(
        TARGET  eckit_test_option_${OPTION}
        SOURCES eckit_test_option_${OPTION}.cc
        LIBS    eckit_option
    )
endforeach()
