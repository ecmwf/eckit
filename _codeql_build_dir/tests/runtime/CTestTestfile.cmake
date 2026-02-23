# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/runtime
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/runtime
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_runtime_producer "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/runtime/eckit_test_runtime_producer")
set_tests_properties(eckit_test_runtime_producer PROPERTIES  ENVIRONMENT "_TEST_ECKIT_HOME=/tmp/runner;OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;1;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;0;")
add_test(eckit_test_runtime_telemetry "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/runtime/eckit_test_runtime_telemetry")
set_tests_properties(eckit_test_runtime_telemetry PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;7;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;0;")
add_test(eckit_test_runtime_context "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/runtime/eckit_test_runtime_context")
set_tests_properties(eckit_test_runtime_context PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;12;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;0;")
add_test(eckit_test_runtime_taskinfo "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/runtime/eckit_test_runtime_taskinfo")
set_tests_properties(eckit_test_runtime_taskinfo PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;17;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;0;")
add_test(eckit_test_runtime_metrics "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/runtime/eckit_test_runtime_metrics")
set_tests_properties(eckit_test_runtime_metrics PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;22;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/runtime/CMakeLists.txt;0;")
