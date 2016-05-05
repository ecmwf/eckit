ecbuild_add_test(
    TARGET    eckit_test_geometry
    BOOST
    LIBS      eckit_geometry
    SOURCES
        test_rotategrid.cc
        test_kdtree.cc
    )
