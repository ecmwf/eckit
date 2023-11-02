ecbuild_add_test(
    TARGET  eckit_test_config_resource
    SOURCES test_resource.cc
    ARGS    -integer 100 -listlong 88,99,11,22
    LIBS    eckit
)

foreach( _test configuration mapped_configuration )
    ecbuild_add_test(
        TARGET  eckit_test_config_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit
        )
endforeach()

