ecbuild_add_test( TARGET  eckit_test_runtime_producer
                  SOURCES test_producer.cc
                  ENVIRONMENT _TEST_ECKIT_HOME=/tmp/$ENV{USER}
                  LIBS    eckit
)

ecbuild_add_test( TARGET  eckit_test_runtime_telemetry
                  SOURCES test_telemetry.cc
                  LIBS    eckit
)

ecbuild_add_test( TARGET  eckit_test_runtime_context
                  SOURCES test_context.cc
                  LIBS    eckit
)
