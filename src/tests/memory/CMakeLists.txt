ecbuild_add_test(   TARGET      test_counted
                    SOURCES     test_counted.cc
                    LIBS        eckit )

ecbuild_add_test(   TARGET      test_shared_ptr
					BOOST
                    SOURCES     test_shared_ptr.cc
                    LIBS        eckit )
                    
ecbuild_add_test(   TARGET      test_scoped_ptr
					BOOST
                    SOURCES     test_scoped_ptr.cc
                    LIBS        eckit )
                    
