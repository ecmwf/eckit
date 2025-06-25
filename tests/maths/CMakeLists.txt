ecbuild_add_test(
    TARGET    eckit_test_maths_eigen
    SOURCES   test_eigen.cc
    CONDITION HAVE_EIGEN
    LIBS      eckit_maths
)

foreach( _test fraction matrix matrix3 )
    ecbuild_add_test(
        TARGET  eckit_test_maths_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_maths
    )
endforeach()

set(_linux_x86 CMAKE_SYSTEM_NAME STREQUAL "Linux" AND (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i386"))
set(_fpe_not_supported NOT eckit_HAVE_FEENABLEEXCEPT AND NOT APPLE AND NOT _linux_x86)

ecbuild_add_test(
    TARGET          eckit_test_maths_floating_point_exceptions
    SOURCES         test_floating_point_exceptions.cc
    LIBS            eckit_maths
    TEST_PROPERTIES WILL_FAIL _fpe_not_supported
)

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
