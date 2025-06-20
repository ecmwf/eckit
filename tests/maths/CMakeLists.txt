ecbuild_add_test(
    TARGET    eckit_test_maths_eigen
    SOURCES   test_eigen.cc
    CONDITION HAVE_EIGEN
    LIBS      eckit_maths
)

foreach( _test floating_point_exceptions fraction matrix matrix3 )
    ecbuild_add_test(
        TARGET  eckit_test_maths_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_maths
    )
endforeach()

ecbuild_add_executable(
    TARGET  floating_point_exception
    SOURCES floating_point_exception.cc
    LIBS    eckit_maths
    NOINSTALL
)

foreach( _fe FE_INVALID FE_INEXACT FE_DIVBYZERO FE_OVERFLOW FE_UNDERFLOW)
    ecbuild_add_test(
        TARGET            eckit_test_maths_floating_point_exception_${_fe}
        COMMAND           floating_point_exception.sh
        ARGS              ${_fe}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        TEST_PROPERTIES   WILL_FAIL TRUE
    )
endforeach()

ecbuild_add_test(
    TARGET    eckit_test_maths_convex_hull
    SOURCES   test_convex_hull.cc
    CONDITION HAVE_CONVEX_HULL
    LIBS      eckit_maths
)
