ecbuild_add_test(
    TARGET      eckit_test_option_cmdargs
    SOURCES     eckit_test_option_cmdargs.cc
    LIBS        eckit_option
    BOOST
)

ecbuild_add_test(
    TARGET      eckit_test_option_factory
    SOURCES     eckit_test_option_factory.cc
    LIBS        eckit_option
    BOOST
)
