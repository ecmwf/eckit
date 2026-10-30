# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

function(eckit_geo_add_test _source_we)
    cmake_parse_arguments(_p "" "" "LIBS;ENVIRONMENT" ${ARGN})
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_source_we}
        SOURCES     ${_source_we}.cc
        LIBS        eckit_geo ${_p_LIBS}
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache" "ECKIT_GEO_CACHING=1"
                    "ECKIT_GEO_SEARCH_LOADER=mapped-file" ${_p_ENVIRONMENT}
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
        grid_crop
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
        projection_laea
        projection_lcc
        projection_ll_to_xyz
        projection_mercator
        projection_plate-caree
        projection_rotation
        projection_space_view
        projection_stere
        range
        search_kdtree
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
        projection_proj)
    eckit_geo_add_test(${_test} CONDITION eckit_HAVE_PROJ)
endforeach()

eckit_geo_add_test(area_library WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# searches, by each k-d tree loader (of the library, LibEcKitGeo::searchCacheLoader); shared memory not on macOS, where
# the default limits (shmmax 4 MiB) are below the test grids k-d trees
set(_loaders memory mapped-file file mapped-anonymous-memory)
if(NOT APPLE)
    list(APPEND _loaders tmp-shared-memory)
endif()

foreach(_loader ${_loaders})
    ecbuild_add_test(
        TARGET      eckit_test_geo_search_${_loader}
        SOURCES     search.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache" "ECKIT_GEO_CACHING=1"
                    "ECKIT_GEO_SEARCH_LOADER=${_loader}" )
endforeach()

# cache directories: read-only, then writable (given by an environment variable, as an expansion)
eckit_geo_add_test(cache_roots ENVIRONMENT
    "ECKIT_GEO_TEST_CACHE_RW=${CMAKE_CURRENT_BINARY_DIR}/cache_roots/rw"
    "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/cache_roots/ro:{ENVVAR?ECKIT_GEO_TEST_CACHE_RW}")

# read-only cache (not caching): cache files are only found, and computed in memory otherwise
eckit_geo_add_test(cache_readonly ENVIRONMENT
    "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/cache_readonly"
    "ECKIT_GEO_CACHING=0")

add_subdirectory(integration)
