
ecbuild_add_test( TARGET      test_rotategrid
                  BOOST
                  SOURCES     test_rotategrid.cc
                  LIBS        eckit  eckit_geometry
                )

ecbuild_add_test( TARGET      test_polarstereographicproj
                  BOOST
                  SOURCES     test_polarstereographicproj.cc
                  LIBS        eckit  eckit_geometry
                )
                