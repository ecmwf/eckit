# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/spec
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/spec
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_spec_custom "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/spec/eckit_test_spec_custom")
set_tests_properties(eckit_test_spec_custom PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/spec/CMakeLists.txt;2;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/spec/CMakeLists.txt;0;")
add_test(eckit_test_spec_layered "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/spec/eckit_test_spec_layered")
set_tests_properties(eckit_test_spec_layered PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/spec/CMakeLists.txt;2;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/spec/CMakeLists.txt;0;")
