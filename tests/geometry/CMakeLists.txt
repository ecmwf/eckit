foreach( _test
        coordinate_helpers
        great_circle
        iterator
        kdtree
        kpoint
        param
        points
        polygon
        projection
        projection_ll_to_xyz
        projection_rotation
        search
        sphere
        types
        util )
    ecbuild_add_test(
        TARGET  eckit_test_geometry_${_test}
        SOURCES test_${_test}.cc
        LIBS    eckit_geometry )
endforeach()

if(HAVE_PROJ)
    ecbuild_add_test(
        TARGET  eckit_test_geometry_projection_proj
        SOURCES test_projection_proj.cc
        LIBS    eckit_geometry )
endif()

