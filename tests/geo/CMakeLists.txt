foreach(_test
        area_boundingbox
        area_polygon
        cache
        figure
        figure_earth
        great_circle
        grid
        grid_healpix
        grid_reduced_gg
        grid_regular_gg
        grid_regular_ll
        grid_reorder
        grid_to_points
        increments
        integration
        iterator
        kdtree
        order
        point
        point2
        point3
        pointlonlat
        pointlonlatr
        projection
        projection_albers
        projection_ll_to_xyz
        projection_mercator
        projection_plate-caree
        projection_rotation
        range
        search
        spec
        spec_custom
        spec_layered
        util )
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" )
endforeach()

set_tests_properties(
    eckit_test_geo_search
    PROPERTIES
    ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache" )

if(eckit_HAVE_ZIP)
    target_compile_definitions(eckit_test_geo_cache PRIVATE ZIP_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test.zip")
endif()

if(eckit_HAVE_PROJ)
    ecbuild_add_test(
        TARGET  eckit_test_geo_projection_proj
        SOURCES projection_proj.cc
        LIBS    eckit_geo )
endif()

ecbuild_add_test(
    TARGET            eckit_test_geo_area_library
    SOURCES           area_library.cc
    LIBS              eckit_geo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

function(ecbuild_add_test_geo_grid _grid)
    ecbuild_add_test(
        TARGET      eckit_test_geo_grid_${_grid}
        SOURCES     grid_${_grid}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" )
endfunction()

if(ECKIT_GEO_CODEC_GRIDS)
    ecbuild_add_test_geo_grid(fesom)
    ecbuild_add_test_geo_grid(icon)
    ecbuild_add_test_geo_grid(orca)
endif()
