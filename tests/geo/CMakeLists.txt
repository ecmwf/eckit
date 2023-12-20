set( _tests
    area_boundingbox
    area_polygon
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

if( eckit_HAVE_GEO_CONVEX_HULL )
    list( APPEND _tests convex_hull )
endif()

foreach( _test ${_tests} )
    ecbuild_add_test(
        TARGET  eckit_test_geo_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_geo )
endforeach()

