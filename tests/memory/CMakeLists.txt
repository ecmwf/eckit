option( ENABLE_ECKIT-351 "Control if ECKIT-351 tests should be enabled" ON )

ecbuild_add_test( TARGET      eckit_test_memory_counted
                  SOURCES     test_counted.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_factory
                  SOURCES     test_factory.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_scoped_ptr
                  SOURCES     test_scoped_ptr.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_shared_ptr
                  SOURCES     test_shared_ptr.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_mmap
                  SOURCES     test_memory_mmap.cc
                  LIBS        eckit
                  CONDITION   ENABLE_ECKIT-351 )
