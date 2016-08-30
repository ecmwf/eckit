ecbuild_add_test( TARGET      eckit_test_cache
                  SOURCES     test_cache.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_floatcompare
                  BOOST
                  SOURCES     test_floatcompare.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types
                  BOOST
                  SOURCES     test_uuid.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_print_vector
                  BOOST
                  SOURCES     test_print_vector.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_fixedstring
                  BOOST
                  SOURCES     test_fixedstring.cc
                  LIBS        eckit )
