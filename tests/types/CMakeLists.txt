ecbuild_add_test( TARGET      eckit_test_cache
                  SOURCES     test_cache.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_doublecompare
                  SOURCES     test_doublecompare.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_floatcompare
                  SOURCES     test_floatcompare.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_uuid
                  SOURCES     test_uuid.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_print_vector
                  SOURCES     test_print_vector.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_fixedstring
                  SOURCES     test_fixedstring.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_fraction
                  SOURCES     test_fraction.cc
                  LIBS        eckit )

# performance test

ecbuild_add_test( TARGET      eckit-test-double-compare-speed
                  SOURCES     test-double-compare-speed.cc
                  LIBS        eckit )

