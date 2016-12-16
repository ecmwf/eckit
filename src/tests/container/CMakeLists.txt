ecbuild_add_test( TARGET   eckit_test_sharedmemarray
                  BOOST
                  SOURCES  test_sharedmemarray.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_btree
                  BOOST
                  SOURCES  test_btree.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_densemap
                  BOOST
                  SOURCES  test_densemap.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_cache_lru
                  BOOST
                  SOURCES  test_cache_lru.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_benchmark_densemap
                  BOOST
                  SOURCES  benchmark_densemap.cc
                  LIBS     eckit )
