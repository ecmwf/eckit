ecbuild_add_test( TARGET      eckit_test_utils_string_tools
                  SOURCES     test_string_tools.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_translator
                  SOURCES     test_translator.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_tokenizer
                  SOURCES     test_tokenizer.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_md5
                  SOURCES     test_md5.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_rendezvoushash
                  SOURCES     test_rendezvoushash.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_compressor
                  SOURCES     test_compressor.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_sha1
                  CONDITION   ECKIT_HAVE_SSL
                  INCLUDES    "${OPENSSL_INCLUDE_DIR}"
                  SOURCES     test_sha1.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_md4
                  CONDITION   ECKIT_HAVE_SSL
                  INCLUDES    "${OPENSSL_INCLUDE_DIR}"
                  SOURCES     test_md4.cc
                  LIBS        eckit )

#ecbuild_add_test( TARGET      eckit_test_utils_xxhash
#                  CONDITION   ECKIT_HAVE_XXHASH
#                  INCLUDES    "${XXHASH_INCLUDE_DIRS}"
#                  SOURCES     test_xxhash.cc
#                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_hash_performance
                  SOURCES     hash-performance.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_compression_performance
                  SOURCES     compression-performance.cc
                  LIBS        eckit )
