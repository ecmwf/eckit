ecbuild_add_test(
    TARGET    eckit_test_geometry_kdtree
    LIBS      eckit_geometry
    SOURCES   test_kdtree.cc
    )

ecbuild_add_test(
    TARGET    eckit_test_geometry_sphere
    LIBS      eckit_geometry
    SOURCES   test_sphere.cc
    )
