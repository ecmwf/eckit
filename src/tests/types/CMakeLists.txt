ecbuild_add_test(   TARGET      test_cache
                    SOURCES     test_cache.cc
                    LIBS        eckit )

ecbuild_add_test(   TARGET      test_floatcompare
                    SOURCES     test_floatcompare.cc
                    LIBS        eckit )
                    
ecbuild_add_test(   TARGET      test_eckit_types
					BOOST
                    SOURCES     test_uuid.cc
                    LIBS        eckit )
