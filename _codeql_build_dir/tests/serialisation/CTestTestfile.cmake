# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/serialisation
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/serialisation
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_serialisation_file_stream "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/serialisation/eckit_test_serialisation_file_stream")
set_tests_properties(eckit_test_serialisation_file_stream PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/serialisation/CMakeLists.txt;1;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/serialisation/CMakeLists.txt;0;")
add_test(eckit_test_serialisation_streamable "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/serialisation/eckit_test_serialisation_streamable")
set_tests_properties(eckit_test_serialisation_streamable PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/serialisation/CMakeLists.txt;5;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/serialisation/CMakeLists.txt;0;")
