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

ecbuild_add_test( TARGET      eckit_test_log_colour
                  SOURCES     test_colour.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_log_json
                  SOURCES     test_log_json.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_log_user_channels
                  ENABLED     OFF
                  SOURCES     test_log_user_channels.cc
                  LIBS        eckit )
