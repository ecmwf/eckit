# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/config
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/config
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_config_resource "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/config/eckit_test_config_resource" "-integer" "100" "-listlong" "88,99,11,22")
set_tests_properties(eckit_test_config_resource PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/config/CMakeLists.txt;1;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/config/CMakeLists.txt;0;")
add_test(eckit_test_config_configuration "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/config/eckit_test_config_configuration")
set_tests_properties(eckit_test_config_configuration PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/config/CMakeLists.txt;8;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/config/CMakeLists.txt;0;")
