ecbuild_add_test( TARGET      eckit_test_string_tools
                  SOURCES     test_string_tools.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_translator
                  BOOST
                  SOURCES     test_translator.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_tokenizer
                  SOURCES     test_tokenizer.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_md5
                  BOOST
                  SOURCES     test_md5.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_rendezvoushash
                  BOOST
                  SOURCES     test_rendezvoushash.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_sha1
                  BOOST
                  CONDITION   ECKIT_HAVE_SSL
                  SOURCES     test_sha1.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_md4
                              BOOST
                              CONDITION   ECKIT_HAVE_SSL
                              SOURCES     test_md4.cc
                              LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_xxhash
                              BOOST
                              CONDITION   ECKIT_HAVE_XXHASH
                              SOURCES     test_xxhash.cc
                              LIBS        eckit )

ecbuild_add_test( TARGET      hash-performance
                  CONDITION   ECKIT_HAVE_SSL AND ECKIT_HAVE_XXHASH
                  INCLUDES
                      "${XXHASH_INCLUDE_DIRS}"
                      "${OPENSSL_INCLUDE_DIR}"
                  SOURCES     hash-performance.cc
                  LIBS        eckit )
