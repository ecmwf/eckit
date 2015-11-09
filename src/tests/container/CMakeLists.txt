ecbuild_add_test(   TARGET      eckit_test_container
					BOOST
					SOURCES     test_btree.cc test_densemap.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      eckit_test_benchmark_densemap
					BOOST
					SOURCES     benchmark_densemap.cc
					LIBS        eckit )
