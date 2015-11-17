ecbuild_add_test( TARGET      eckit_test_log
                  SOURCES     test_log.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_log_channels
                  SOURCES     test_log_channels.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_log_callback
                  SOURCES     test_log_callback.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_log_threads
                  SOURCES     test_log_threads.cc
                  ENVIRONMENT _TEST_ECKIT_HOME=/tmp/$ENV{USER}
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_colour
                  SOURCES     test_colour.cc
                  LIBS        eckit )

