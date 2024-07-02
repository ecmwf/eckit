set(CACHE_PATH "${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache")

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
        ordering
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
        spec_custom
        spec_layered
        util )
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo )
endforeach()

if(eckit_HAVE_GEO_GRID_ORCA)
    set(URL "https://get.ecmwf.int/repository/atlas/grids/orca/v0/ORCA2_T.atlas")
    set(FILE "${CACHE_PATH}/eckit/geo/grid/orca/d5bde4f52ff3a9bea5629cd9ac514410.atlas")
    set(MD5 "f279b48c171409f46bfd27dff98d454a")

    file(DOWNLOAD ${URL} ${FILE} EXPECTED_MD5 ${MD5} TIMEOUT 30 STATUS DOWNLOAD_STATUS)

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(${STATUS_CODE} EQUAL 0)
        message(STATUS "File downloaded: '${FILE}'")
    else()
        list(GET DOWNLOAD_STATUS 1 STATUS_MESSAGE)
        message(FATAL_ERROR "File download failed: ${STATUS_MESSAGE}, file: '${FILE}'")
    endif()

    ecbuild_add_test(
        TARGET      eckit_test_geo_grid_orca
        SOURCES     grid_orca.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CACHE_PATH}" )
endif()

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_1_1
    COMMAND eckit_grid_spec ARGS "grid: 1/1.0" )

set_tests_properties(
    eckit_test_geo_tool_grid_spec_1_1
    PROPERTIES
    PASS_REGULAR_EXPRESSION [=[{"grid":\[1,1\]}]=] )

ecbuild_add_test(
    TARGET  eckit_test_geo_tool_grid_spec_025_01
    COMMAND eckit_grid_spec ARGS "grid: .250/001e-1" )

set_tests_properties(
    eckit_test_geo_tool_grid_spec_025_01
    PROPERTIES
    PASS_REGULAR_EXPRESSION [=[{"grid":\[0\.25,0\.1\]}]=] )

