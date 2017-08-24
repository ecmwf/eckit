ecbuild_add_test( TARGET   eckit_test_sharedmemarray
                  SOURCES  test_sharedmemarray.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_btree
                  SOURCES  test_btree.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_bloomfilter
                  SOURCES  test_bloomfilter.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_trie
                  SOURCES  test_trie.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_densemap
                  SOURCES  test_densemap.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_cache_lru
                  SOURCES  test_cache_lru.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_benchmark_densemap
                  SOURCES  benchmark_densemap.cc
                  LIBS     eckit )
