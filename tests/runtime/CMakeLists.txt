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

ecbuild_add_test( TARGET  eckit_test_runtime_taskinfo
                  SOURCES test_taskinfo.cc
                  LIBS    eckit
)

ecbuild_add_test( TARGET  eckit_test_runtime_metrics
                  SOURCES test_metrics.cc
                  LIBS    eckit
)

ecbuild_add_test( TARGET  eckit_test_runtime_rundir_run
                  SOURCES test_rundir.cc
                  ENVIRONMENT ECKIT_TEST_RUNDIR_EXPECT=run
                  LIBS    eckit
)

ecbuild_add_test( TARGET  eckit_test_runtime_rundir_etc
                  SOURCES test_rundir.cc
                  ENVIRONMENT ECKIT_TEST_RUNDIR_EXPECT=etc
                  LIBS    eckit
)
