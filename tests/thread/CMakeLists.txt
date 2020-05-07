
if( NOT ENABLE_WARNINGS )
  list( APPEND suppress_warnings
    $<$<CXX_COMPILER_ID:Cray>:-h nomessage=3140> # Disable optimisation warning in debug build due to use of intrinsics
  )
  set_source_files_properties( test_mutex.cc PROPERTIES COMPILE_OPTIONS "${suppress_warnings}" )
endif()

ecbuild_add_test( TARGET      eckit_test_thread_mutex
                  SOURCES     test_mutex.cc
                  LIBS        eckit )