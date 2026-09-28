# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

set(_cache_path "${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

foreach(_test
        area_boundingbox
        area_polygon
        figure
        figure_earth
        figure_sun
        gaussian
        great_circle
        grid
        grid_healpix
        grid_reduced_gg
        grid_reduced_ll
        grid_regular_gg
        grid_regular_ll
        grid_regular_xy
        grid_reorder
        grid_to_points
        gridspec
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
        projection_ll_to_xyz
        projection_plate-caree
        projection_rotation
        range
        search
        sh
        util )
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${_cache_path}" )
endforeach()

foreach(_test
        grid_fesom
        grid_icon
        grid_orca
        grid_unstructured_ll )
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_test}
        SOURCES     ${_test}.cc
        CONDITION   eckit_HAVE_LZ4 AND eckit_HAVE_CURL
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${_cache_path}" )
endforeach()

ecbuild_add_test(
    TARGET      eckit_test_geo_cache
    SOURCES     cache.cc
    ARGS        --cacheable_zip "${CMAKE_CURRENT_SOURCE_DIR}/test.zip"
    LIBS        eckit_geo
    ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${_cache_path}" )

foreach(_test
        grid_regular_xy_swisslv03
        grid_regular_xy_swisslv95
        projection_albers
        projection_mercator
        projection_proj
    )
    ecbuild_add_test(
        TARGET    eckit_test_geo_${_test}
        SOURCES   ${_test}.cc
        CONDITION eckit_HAVE_PROJ
        LIBS      eckit_geo )
endforeach()

ecbuild_add_test(
    TARGET            eckit_test_geo_area_library
    SOURCES           area_library.cc
    LIBS              eckit_geo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory(integration)
