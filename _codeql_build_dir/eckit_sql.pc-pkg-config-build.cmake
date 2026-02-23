
file(READ /home/runner/work/eckit/eckit/_codeql_build_dir/eckit_sql.pc.tmp _content)

string(REPLACE "/home/runner/work/eckit/eckit/_codeql_build_dir/lib" "\${libdir}" _content "${_content}")
if(NOT "lib" STREQUAL "lib")
  string(REPLACE "/home/runner/work/eckit/eckit/_codeql_build_dir/lib" "\${libdir}" _content "${_content}")
endif()
string(REPLACE "/usr/local/lib" "\${libdir}" _content "${_content}")

string(REGEX REPLACE "%SHORTEN:lib" "%SHORTEN:" _content "${_content}")
string(REGEX REPLACE "\\.(a|so|dylib|dll|lib)(\\.[0-9]+)*%" "%" _content "${_content}")
string(REGEX REPLACE "%SHORTEN:([^%]+)%" "\\1" _content "${_content}")

file(WRITE /home/runner/work/eckit/eckit/_codeql_build_dir/lib/pkgconfig/eckit_sql.pc "${_content}")
