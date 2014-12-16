ecbuild_add_test( TARGET      test_geometry
                  BOOST
				  SOURCES     test_rotategrid.cc test_polarstereographicproj.cc
				  LIBS        eckit_geometry
                )
