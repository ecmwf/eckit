# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/sql
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/sql
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_sql_select "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/sql/eckit_test_sql_select")
set_tests_properties(eckit_test_sql_select PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/sql/CMakeLists.txt;8;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/sql/CMakeLists.txt;0;")
add_test(eckit_test_sql_simple_functions "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/sql/eckit_test_sql_simple_functions")
set_tests_properties(eckit_test_sql_simple_functions PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/sql/CMakeLists.txt;8;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/sql/CMakeLists.txt;0;")
