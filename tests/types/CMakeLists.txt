ecbuild_add_test( TARGET      eckit_test_types_cache
                  SOURCES     test_cache.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_doublecompare
                  SOURCES     test_doublecompare.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_floatcompare
                  SOURCES     test_floatcompare.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_uuid
                  SOURCES     test_uuid.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_print_vector
                  SOURCES     test_print_vector.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_fixedstring
                  SOURCES     test_fixedstring.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_fraction
                  SOURCES     test_fraction.cc
                  LIBS        eckit )

# performance test

ecbuild_add_test( TARGET      eckit_test_types_double_compare_speed
                  SOURCES     test-double-compare-speed.cc
                  LIBS        eckit )

