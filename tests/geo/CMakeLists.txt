# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

function(eckit_geo_add_test _source_we)
    cmake_parse_arguments(_p "" "" "LIBS;ENVIRONMENT" ${ARGN})
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_source_we}
        SOURCES     ${_source_we}.cc
        LIBS        eckit_geo ${_p_LIBS}
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache" ${_p_ENVIRONMENT}
        ${_p_UNPARSED_ARGUMENTS} )
endfunction()

foreach(_test
        area_boundingbox
        area_polygon
        cache
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
        util)
    eckit_geo_add_test(${_test})
endforeach()

foreach(_test
        grid_fesom
        grid_icon
        grid_orca
        grid_unstructured_ll)
    eckit_geo_add_test(${_test} CONDITION eckit_HAVE_LZ4 AND eckit_HAVE_CURL)
endforeach()

foreach(_test
        grid_regular_xy_swisslv03
        grid_regular_xy_swisslv95
        projection_albers
        projection_mercator
        projection_proj)
    eckit_geo_add_test(${_test} CONDITION eckit_HAVE_PROJ)
endforeach()

eckit_geo_add_test(unzip ARGS --unzip "${CMAKE_CURRENT_SOURCE_DIR}/test.zip" CONDITION eckit_HAVE_ZIP)
eckit_geo_add_test(area_library WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(integration)
