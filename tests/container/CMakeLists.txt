ecbuild_add_test( TARGET   eckit_test_container_sharedmemarray
                  SOURCES  test_sharedmemarray.cc
                  LIBS     eckit ${RT_LIBRARIES} )

ecbuild_add_test( TARGET   eckit_test_container_btree
                  SOURCES  test_btree.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_bloomfilter
                  SOURCES  test_bloomfilter.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_trie
                  SOURCES  test_trie.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_queue
                  SOURCES  test_queue.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_densemap
                  SOURCES  test_densemap.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_denseset
                  SOURCES  test_denseset.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_cache_lru
                  SOURCES  test_cache_lru.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_cachemanager
                  SOURCES  test_cachemanager.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_container_benchmark_densemap
                  SOURCES  benchmark_densemap.cc
                  LIBS     eckit )
