ecbuild_add_test( TARGET   eckit_test_sql
                  SOURCES  test_sql.cc
                  LIBS     eckit )


if( CMAKE_CXX_COMPILER_ID MATCHES "Cray" )
   # Disable warnings for test_value_integer due to following:
   #    "Integer conversion resulted in a change of sign."
   set_source_files_properties(test_value_integer.cc PROPERTIES COMPILE_FLAGS "-hmsglevel_4" )
endif()
