foreach( _test
        area_boundingbox
        area_polygon
        convex_hull_n
        coordinate_helpers
        figure_sphere
        great_circle
        grid
        grid_regular_ll
        grid_reorder
        grid_to_points
        gridspec
        increments
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

ecbuild_add_test(
    TARGET    eckit_test_geo_qhull
    SOURCES   qhull.cc
    LIBS      eckit_geo
    CONDITION eckit_HAVE_GEO_CONVEX_HULL
)

