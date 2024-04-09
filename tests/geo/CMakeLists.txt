foreach(_test
        area_boundingbox
        area_polygon
        cache
        figure_sphere
        great_circle
        grid
        grid_healpix
        grid_reduced_gg
        grid_regular_gg
        grid_regular_ll
        grid_reorder
        grid_to_points
        increments
        iterator
        kdtree
        point
        point2
        point3
        pointlonlat
        projection
        projection_ll_to_xyz
        projection_mercator
        projection_plate-caree
        projection_proj
        projection_rotation
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
    TARGET    eckit_test_geo_grid_orca
    SOURCES   grid_orca.cc
    LIBS      eckit_geo
    CONDITION eckit_HAVE_ECKIT_CODEC )

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_1_1
    COMMAND eckit_grid_spec ARGS "grid: 1/1.0" [=[--check=[{]"grid":\[1,1\][}]]=] )

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_025_01
    COMMAND eckit_grid_spec ARGS "grid: .250/001e-1" [=[--check=[{]"grid":\[0\.25,0\.1\][}]]=] )

