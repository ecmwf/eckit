ecbuild_add_test( TARGET      eckit_test_multiplexer
                  CONDITION   HAVE_GRIB
                  BOOST
                  SOURCES     test_multiplexer.cc
                  LIBS        eckit_sandbox_multiplexer )

