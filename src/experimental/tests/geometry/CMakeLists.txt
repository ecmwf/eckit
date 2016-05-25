ecbuild_add_test(
    TARGET    eckit_test_geometry
    LIBS      eckit_geometry
    BOOST
    SOURCES
        test_kdtree.cc
    )
