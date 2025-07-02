set(TEST_ECKIT_GEO_CACHE_PATH "${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_cache")

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
        TARGET  eckit_test_geo_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_geo )
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

if(eckit_HAVE_GEO_GRID_ORCA)
    set(FILE "${TEST_ECKIT_GEO_CACHE_PATH}/grid/orca/d5bde4f52ff3a9bea5629cd9ac514410-313218f18f94c235fd8ef2d6cc6ad6fe.ek")

    configure_file("ORCA2_T.ek" "${FILE}" COPYONLY)
    if(NOT EXISTS "${FILE}")
        message(FATAL_ERROR "Required file not found: '${FILE}'")
    endif()

    ecbuild_add_test(
        TARGET      eckit_test_geo_grid_orca
        SOURCES     grid_orca.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${TEST_ECKIT_GEO_CACHE_PATH}" )
endif()

if(eckit_HAVE_GEO_GRID_FESOM)
    ecbuild_add_test(
        TARGET      eckit_test_geo_grid_fesom
        SOURCES     grid_fesom.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${TEST_ECKIT_GEO_CACHE_PATH}" )
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

add_subdirectory( integration )
