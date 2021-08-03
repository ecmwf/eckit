option( ENABLE_ECKIT-395 "Control if ECKIT-395 tests should be enabled" ON )
#  eckit_test_types_fixedstring known to fail compilation with PGI 19.4 (working with PGI 18.10)


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
                  LIBS        eckit
                  CONDITION   ENABLE_ECKIT-395 )

ecbuild_add_test( TARGET      eckit_test_types_fraction
                  SOURCES     test_fraction.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_types_hour
                  SOURCES     test_hour.cc
                  LIBS        eckit )

# performance test

ecbuild_add_test( TARGET      eckit_test_types_double_compare_speed
                  SOURCES     test-double-compare-speed.cc
                  LIBS        eckit )
