# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/thread
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/thread
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_thread_mutex "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/thread/eckit_test_thread_mutex")
set_tests_properties(eckit_test_thread_mutex PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/thread/CMakeLists.txt;9;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/thread/CMakeLists.txt;0;")
