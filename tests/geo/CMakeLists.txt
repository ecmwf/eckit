foreach(_test
        area_boundingbox
        area_polygon
        cache
        coordinate_helpers
        figure_sphere
        great_circle
        grid
        grid_regular_ll
        grid_reorder
        grid_to_points
        increments
        iterator
        kdtree
        kpoint
        point
        point2
        point3
        pointlonlat
        projection
        range
        search
        spec
        util )
    ecbuild_add_test(
        TARGET  eckit_test_geo_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_geo )
endforeach()

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_1_1
    COMMAND eckit_grid_spec 1/1. )

#set_tests_properties(eckit_test_geo_tool_grid_spec_1_1 PROPERTIES
#    PASS_REGULAR_EXPRESSION "GridSpec: 1/1")

