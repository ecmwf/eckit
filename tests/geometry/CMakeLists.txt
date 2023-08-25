foreach( _test
        coordinate_helpers
        great_circle
        grid
        iterator
        kdtree
        kpoint
        param
        points
        polygon
        projection
        search
        sphere
        types
        util )
    ecbuild_add_test(
        TARGET  eckit_test_geometry_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_geometry )
endforeach()

