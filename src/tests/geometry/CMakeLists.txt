ecbuild_add_test( TARGET      eckit_test_geometry
                  BOOST
                  SOURCES
                      test_rotategrid.cc
                      test_kdtree.cc
                  LIBS        eckit_geometry )
