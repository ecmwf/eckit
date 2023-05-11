foreach( _test
        iterator
        param
        projection
        projection_ll_to_xyz
        projection_proj
        projection_rotation
        search
        types
        util
        )
    ecbuild_add_test(
        TARGET  eckit_test_geo_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_geo )
endforeach()

if(HAVE_ECCODES)
    ecbuild_add_test(
        TARGET  eckit_test_geo_grib
        SOURCES test_grib.cc
        LIBS    eckit_geo eccodes)
endif()

