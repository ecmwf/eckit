foreach( _test
        coordinate_helpers
        great_circle
        grid
        gridspec
        iterator
        kdtree
        kpoint
        param
        points
        polygon
        projection
        range
        search
        sphere
        types
        util )
    ecbuild_add_test(
        TARGET  eckit_test_geo_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_geo )
endforeach()

