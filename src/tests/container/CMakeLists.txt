ecbuild_add_test(   TARGET      test_container
					BOOST
					SOURCES     test_btree.cc test_kdtree.cc test_densemap.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      benchmark_densemap
					BOOST
					SOURCES     benchmark_densemap.cc
					LIBS        eckit )
