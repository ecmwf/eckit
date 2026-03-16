foreach(_test custom layered)
    ecbuild_add_test(
        TARGET  eckit_test_spec_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_spec )
endforeach()
