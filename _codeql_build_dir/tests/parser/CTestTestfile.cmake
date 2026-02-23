# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/parser
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/parser
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_parser_json "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/parser/eckit_test_parser_json")
set_tests_properties(eckit_test_parser_json PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;1;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;0;")
add_test(eckit_test_parser_yaml "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/parser/eckit_test_parser_yaml")
set_tests_properties(eckit_test_parser_yaml PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;17;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;0;")
add_test(eckit_test_parser_stream_parser "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/parser/eckit_test_parser_stream_parser")
set_tests_properties(eckit_test_parser_stream_parser PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;21;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;0;")
add_test(eckit_test_parser_csv "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/parser/eckit_test_parser_csv")
set_tests_properties(eckit_test_parser_csv PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;25;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/parser/CMakeLists.txt;0;")
