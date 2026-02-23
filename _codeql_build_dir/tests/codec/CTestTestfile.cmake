# CMake generated Testfile for 
# Source directory: /home/runner/work/eckit/eckit/tests/codec
# Build directory: /home/runner/work/eckit/eckit/_codeql_build_dir/tests/codec
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eckit_test_codec_encoding "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/codec/eckit_test_codec_encoding")
set_tests_properties(eckit_test_codec_encoding PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;9;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;0;")
add_test(eckit_test_codec_stream "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/codec/eckit_test_codec_stream")
set_tests_properties(eckit_test_codec_stream PROPERTIES  ENVIRONMENT "OMP_NUM_THREADS=1" LABELS "eckit;executable" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;470;add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;15;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;0;")
add_test(eckit_test_codec_record_COMPRESSION_none "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/codec/eckit_test_codec_record" "--suffix" ".none")
set_tests_properties(eckit_test_codec_record_COMPRESSION_none PROPERTIES  ENVIRONMENT "ECKIT_CODEC_COMPRESSION=none;OMP_NUM_THREADS=1" LABELS "eckit;script" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;468;add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;31;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;40;add_codec_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;0;")
add_test(eckit_test_codec_record_COMPRESSION_lz4 "/home/runner/work/eckit/eckit/_codeql_build_dir/tests/codec/eckit_test_codec_record" "--suffix" ".lz4")
set_tests_properties(eckit_test_codec_record_COMPRESSION_lz4 PROPERTIES  ENVIRONMENT "ECKIT_CODEC_COMPRESSION=lz4;OMP_NUM_THREADS=1" LABELS "eckit;script" _BACKTRACE_TRIPLES "/home/runner/work/eckit/ecbuild/cmake/ecbuild_add_test.cmake;468;add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;31;ecbuild_add_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;43;add_codec_test;/home/runner/work/eckit/eckit/tests/codec/CMakeLists.txt;0;")
