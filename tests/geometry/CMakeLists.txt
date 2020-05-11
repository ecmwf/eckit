foreach( _test great_circle kdtree sphere kpoint points polygon )
    ecbuild_add_test( TARGET  eckit_test_geometry_${_test}
                      SOURCES test_${_test}.cc
                      LIBS    eckit_geometry )
endforeach()
