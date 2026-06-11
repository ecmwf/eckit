set(_cache_path "${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

foreach(_test
        area_boundingbox
        area_polygon
        cache
        figure
        figure_earth
        gaussian
        great_circle
        grid
        grid_fesom
        grid_healpix
        grid_icon
        grid_orca
        grid_reduced_gg
        grid_reduced_ll
        grid_regular_gg
        grid_regular_ll
        grid_reorder
        grid_to_points
        grid_unstructured_ll
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
        projection_albers
        projection_ll_to_xyz
        projection_mercator
        projection_plate-caree
        projection_rotation
        range
        search
        util )
    ecbuild_add_test(
        TARGET      eckit_test_geo_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${_cache_path}" )
endforeach()

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

