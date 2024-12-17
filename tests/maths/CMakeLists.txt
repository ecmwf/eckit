ecbuild_add_test(
    TARGET    eckit_test_maths_eigen
    SOURCES   test_eigen.cc
    CONDITION HAVE_EIGEN
    LIBS      eckit_maths eckit
)

foreach( _test matrix matrix3 )
    ecbuild_add_test(
        TARGET  eckit_test_maths_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_maths eckit
    )
endforeach()

ecbuild_add_test(
    TARGET    eckit_test_maths_convex_hull
    SOURCES   test_convex_hull.cc
    CONDITION HAVE_CONVEX_HULL
    LIBS      eckit_maths eckit
)

