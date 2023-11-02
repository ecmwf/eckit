foreach( _test
        area_boundingbox
        area_polygon
        coordinate_helpers
        figure_sphere
        figure_sphere_great_circle
        grid
        grid_reorder
        grid_to_points
        gridspec
        iterator
        kdtree
        kpoint
        point
        projection
        range
        search
        util )
    ecbuild_add_test(
        TARGET  eckit_test_geo_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_geo
    )
endforeach()

