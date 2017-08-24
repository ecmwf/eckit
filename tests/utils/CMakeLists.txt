ecbuild_add_test( TARGET      eckit_test_string_tools
                  SOURCES     test_string_tools.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_translator
                  SOURCES     test_translator.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_tokenizer
                  SOURCES     test_tokenizer.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_md5
                  SOURCES     test_md5.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_rendezvoushash
                  SOURCES     test_rendezvoushash.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_sha1
                  CONDITION   ECKIT_HAVE_SSL
                  INCLUDES    "${OPENSSL_INCLUDE_DIR}"
                  SOURCES     test_sha1.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_md4
                  CONDITION   ECKIT_HAVE_SSL
                  INCLUDES    "${OPENSSL_INCLUDE_DIR}"
                  SOURCES     test_md4.cc
                  LIBS        eckit )

#ecbuild_add_test( TARGET      eckit_test_xxhash
#                  CONDITION   ECKIT_HAVE_XXHASH
#                  INCLUDES    "${XXHASH_INCLUDE_DIRS}"
#                  SOURCES     test_xxhash.cc
#                  LIBS        eckit )

ecbuild_add_test( TARGET      hash-performance
                  SOURCES     hash-performance.cc
                  LIBS        eckit )
