ecbuild_add_test( TARGET   eckit_test_testing
                  CONDITION HAVE_CXX11
                  SOURCES  test_testing.cc
                  LIBS     eckit )

