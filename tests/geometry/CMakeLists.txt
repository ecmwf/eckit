ecbuild_add_test(
    TARGET    eckit_test_kdtree
    LIBS      eckit_geometry
    BOOST
    SOURCES   test_kdtree.cc
    )
