# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/memory
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/memory
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_memory_counted "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/memory/eckit_test_memory_counted")
set_tests_properties(eckit_test_memory_counted PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/memory/CMakeLists.txt;3;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/memory/CMakeLists.txt;0;")
add_test(eckit_test_memory_factory "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/memory/eckit_test_memory_factory")
set_tests_properties(eckit_test_memory_factory PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/memory/CMakeLists.txt;7;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/memory/CMakeLists.txt;0;")
add_test(eckit_test_memory_mmap "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/memory/eckit_test_memory_mmap")
set_tests_properties(eckit_test_memory_mmap PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/memory/CMakeLists.txt;11;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/memory/CMakeLists.txt;0;")
