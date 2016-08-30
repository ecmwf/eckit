ecbuild_add_test( TARGET      eckit_test_memory_counted
                  BOOST
                  SOURCES     test_counted.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_factory
                  BOOST
                  SOURCES     test_factory.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_scoped_ptr
                  BOOST
                  SOURCES     test_scoped_ptr.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_memory_shared_ptr
                  BOOST
                  SOURCES     test_shared_ptr.cc
                  LIBS        eckit )
