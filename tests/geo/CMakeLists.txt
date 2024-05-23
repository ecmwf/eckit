foreach(_test
        area_boundingbox
        area_polygon
        cache
        figure
        geometry_sphere
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
        pointlonlatr
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
        TARGET      eckit_test_geo_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache" )
endforeach()

if(eckit_HAVE_ECKIT_CODEC)
    file( DOWNLOAD
        "https://get.ecmwf.int/repository/atlas/grids/orca/v0/ORCA2_T.atlas"
        "${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache/eckit/geo/grid/orca/d5bde4f52ff3a9bea5629cd9ac514410.atlas"
        EXPECTED_MD5 "f279b48c171409f46bfd27dff98d454a"
        SHOW_PROGRESS
        TIMEOUT 30 )
    
    ecbuild_add_test(
        TARGET      eckit_test_geo_grid_orca
        SOURCES     grid_orca.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache" )
endif()

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_1_1
    COMMAND eckit_grid_spec ARGS "grid: 1/1.0" [=[--check=[{]"grid":\[1,1\][}]]=] )

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_025_01
    COMMAND eckit_grid_spec ARGS "grid: .250/001e-1" [=[--check=[{]"grid":\[0\.25,0\.1\][}]]=] )

