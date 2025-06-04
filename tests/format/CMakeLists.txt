ecbuild_add_test( TARGET      eckit_test_format
                  SOURCES     test_format.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_format_to
                  SOURCES     test_format_to.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_format_log
                  SOURCES     test_format_log.cc
                  LIBS        eckit )

