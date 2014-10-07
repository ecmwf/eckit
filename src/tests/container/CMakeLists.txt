ecbuild_add_test(   TARGET      test_btree
					SOURCES     test_btree.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      test_kdtree
					SOURCES     test_kdtree.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      test_container
					BOOST
					SOURCES     test_densemap.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      benchmark_densemap
					SOURCES     benchmark_densemap.cc
					LIBS        eckit )