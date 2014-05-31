ecbuild_add_test(   TARGET      test_memory
					BOOST
					SOURCES     test_counted.cc test_shared_ptr.cc test_scoped_ptr.cc
					LIBS        eckit )
